// async_logger.hpp (single-file demo)
#include <bits/stdc++.h>

namespace alog {

enum class Level : uint8_t { TRACE, DEBUG_, INFO, WARN, ERROR, FATAL };

struct LogRecord {
    Level level;
    std::chrono::system_clock::time_point tp;
    std::thread::id tid;
    std::string msg; // 为了简单用 std::string；进一步可用固定大小缓冲池/SSO优化
};

class BoundedQueue {
  public:
    explicit BoundedQueue(size_t cap) : cap_(cap), dropped_(0) {
        // buf_.reserve(cap);
    }

    // 非阻塞入队：满了返回 false（让上层决定丢弃或合并）
    bool try_push(LogRecord &&r) {
        std::lock_guard<std::mutex> lk(mu_);
        if (buf_.size() >= cap_) {
            ++dropped_;
            return false;
        }
        buf_.emplace_back(std::move(r));
        cv_.notify_one();
        return true;
    }

    // 批量弹出：阻塞直到有数据或到达超时
    size_t pop_bulk(
        std::vector<LogRecord> &out,
        size_t max_batch,
        std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lk(mu_);
        if (buf_.empty()) {
            if (cv_.wait_for(lk, timeout, [&] { return !buf_.empty(); })
                == false)
                return 0;
        }
        size_t n = std::min(max_batch, buf_.size());
        out.clear();
        out.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            out.push_back(std::move(buf_.front()));
            buf_.pop_front();
        }
        return n;
    }

    size_t steal_drop_count() {
        std::lock_guard<std::mutex> lk(mu_);
        size_t x = dropped_;
        dropped_ = 0;
        return x;
    }

  private:
    size_t cap_;
    std::deque<LogRecord> buf_;
    std::mutex mu_;
    std::condition_variable cv_;
    size_t dropped_;
};

struct Options {
    std::string base_path = "app.log";             // 基础文件名
    size_t roll_bytes = 10 * 1024 * 1024;          // 按大小滚动
    size_t queue_capacity = 1 << 14;               // 队列容量
    size_t flush_batch = 1024;                     // 每次最多批量写多少条
    std::chrono::milliseconds flush_interval{200}; // 空闲时刷新周期
    Level min_level = Level::INFO;                 // 最低输出等级
    bool drop_low_level_when_full = true;          // 满时是否优先丢低等级
};

class AsyncLogger {
  public:
    explicit AsyncLogger(Options opt)
        : opt_(std::move(opt))
        , q_(opt_.queue_capacity)
        , stop_(false)
        , file_size_(0) {
        open_file();
        th_ = std::thread([this] { this->run(); });
    }

    ~AsyncLogger() {
        stop_.store(true, std::memory_order_release);
        // 触发消费者尽快醒来
        (void)q_.try_push(
            LogRecord{
                Level::INFO, std::chrono::system_clock::now(),
                std::this_thread::get_id(), "[logger stopping]"});
        if (th_.joinable()) th_.join();
        if (fp_) {
            std::fflush(fp_);
            std::fclose(fp_);
        }
    }

    // 线程安全、非阻塞：满时根据策略丢弃
    void log(Level lv, std::string_view msg) {
        if (lv < opt_.min_level) return;

        // 满时是否只允许高等级
        if (opt_.drop_low_level_when_full
            && likely_full_.load(std::memory_order_relaxed)
            && lv <= Level::INFO) {
            dropped_low_.fetch_add(1, std::memory_order_relaxed);
            return;
        }

        LogRecord r;
        r.level = lv;
        r.tp = std::chrono::system_clock::now();
        r.tid = std::this_thread::get_id();
        r.msg.assign(msg.data(), msg.size());

        if (!q_.try_push(std::move(r))) {
            // 标出“似乎满了”的状态（优化：减少后续 try_push 竞争）
            likely_full_.store(true, std::memory_order_relaxed);
            if (lv <= Level::INFO && opt_.drop_low_level_when_full) {
                dropped_low_.fetch_add(1, std::memory_order_relaxed);
                return;
            }
            // 高等级再尝试一次，否则统计丢弃
            if (!q_.try_push(
                    LogRecord{
                        lv, std::chrono::system_clock::now(),
                        std::this_thread::get_id(),
                        std::string("[queue full] ") + std::string(msg)})) {
                dropped_high_.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }

  private:
    void run() {
        std::vector<LogRecord> batch;
        batch.reserve(opt_.flush_batch);

        while (!stop_.load(std::memory_order_acquire)) {
            size_t n =
                q_.pop_bulk(batch, opt_.flush_batch, opt_.flush_interval);
            size_t dropped = q_.steal_drop_count();
            if (dropped) write_drop_notice(dropped);

            if (auto dlow = dropped_low_.exchange(0); dlow)
                write_drop_notice_level(dlow, "low");
            if (auto dhigh = dropped_high_.exchange(0); dhigh)
                write_drop_notice_level(dhigh, "high");

            if (n == 0) {
                flush();
                continue;
            }

            // 一旦成功取到数据，认为队列“可能不满”
            likely_full_.store(false, std::memory_order_relaxed);

            // 批量格式化+写入
            for (auto &r : batch) { format_and_write(r); }

            // 刷新策略：1) 批量后 2) 周期 3) 高等级立即刷（在 format_and_write
            // 内可触发）
            flush();
        }

        // 退出前尽量清空
        while (
            q_.pop_bulk(batch, opt_.flush_batch, std::chrono::milliseconds(0))
            > 0) {
            for (auto &r : batch) format_and_write(r);
        }
        flush();
    }

    void format_and_write(const LogRecord &r) {
        char ts[32];
        format_time(r.tp, ts, sizeof(ts));
        const char *lv = level_str(r.level);

        // 简单文本格式：时间 等级 线程id | 消息
        // 注意：真实项目建议使用固定缓冲（如 thread_local char[]）避免频繁分配
        std::string line;
        line.reserve(r.msg.size() + 64);
        line.append(ts).append(" ").append(lv).append(" [");
        line.append(thread_id_str(r.tid));
        line.append("] ").append(r.msg).append("\n");

        write_raw(line.data(), line.size());

        // 高等级可立即 flush（折中：避免每条都刷）
        if (r.level >= Level::ERROR) flush();
    }

    void
    format_time(std::chrono::system_clock::time_point tp, char *out, size_t n) {
        using namespace std::chrono;
        auto t = system_clock::to_time_t(tp);
        auto us = duration_cast<microseconds>(tp.time_since_epoch()).count()
                  % 1000000;
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::snprintf(
            out, n, "%04d-%02d-%02d %02d:%02d:%02d.%06lld", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            static_cast<long long>(us));
    }

    static const char *level_str(Level lv) {
        switch (lv) {
            case Level::TRACE:
                return "TRACE";
            case Level::DEBUG_:
                return "DEBUG";
            case Level::INFO:
                return "INFO ";
            case Level::WARN:
                return "WARN ";
            case Level::ERROR:
                return "ERROR";
            case Level::FATAL:
                return "FATAL";
        }
        return "UNKWN";
    }

    static std::string thread_id_str(std::thread::id id) {
        // 简化：把 thread::id 转字符串；生产环境可缓存/映射为整数
        std::ostringstream oss;
        oss << id;
        return oss.str();
    }

    void write_drop_notice(size_t n) {
        char buf[128];
        int m = std::snprintf(
            buf, sizeof(buf), "~~~ dropped %zu log(s) due to full queue ~~~\n",
            n);
        write_raw(buf, (size_t)m);
    }
    void write_drop_notice_level(size_t n, const char *which) {
        char buf[128];
        int m = std::snprintf(
            buf, sizeof(buf), "~~~ dropped %zu %s-priority log(s) ~~~\n", n,
            which);
        write_raw(buf, (size_t)m);
    }

    void write_raw(const char *data, size_t len) {
        if (!fp_) return;
        std::fwrite(data, 1, len, fp_);
        file_size_ += len;
        if (file_size_ >= opt_.roll_bytes) roll_file();
    }

    void flush() {
        if (fp_) std::fflush(fp_);
    }

    void open_file() {
        fp_ = std::fopen(opt_.base_path.c_str(), "ab");
        file_size_ = 0;
        if (fp_) {
            std::filesystem::path p(opt_.base_path);
            if (std::filesystem::exists(p))
                file_size_ = std::filesystem::file_size(p);
        }
    }

    void roll_file() {
        if (!fp_) return;
        std::fclose(fp_);
        fp_ = nullptr;

        // 重命名：app.log -> app-YYYYmmdd-HHMMSS.log
        auto stamp = time_stamp_for_name();
        auto new_name = opt_.base_path;
        auto dot = new_name.rfind('.');
        std::string rolled;
        if (dot == std::string::npos) {
            rolled = new_name + "-" + stamp + ".log";
        } else {
            rolled =
                new_name.substr(0, dot) + "-" + stamp + new_name.substr(dot);
        }
        std::filesystem::rename(opt_.base_path, rolled);
        open_file();
    }

    static std::string time_stamp_for_name() {
        auto now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        char buf[32];
        std::snprintf(
            buf, sizeof(buf), "%04d%02d%02d-%02d%02d%02d", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        return buf;
    }

  private:
    Options opt_;
    BoundedQueue q_;
    std::atomic<bool> stop_;
    std::atomic<bool> likely_full_{false};
    std::atomic<size_t> dropped_low_{0}, dropped_high_{0};
    std::thread th_;

    // file
    std::FILE *fp_{nullptr};
    size_t file_size_;
};

// 便捷宏
#define ALOG(logger, lvl, msg) \
    do { (logger).log((lvl), (msg)); } while (0)

} // namespace alog

// ================== demo ==================
// #include "async_logger.hpp"
int main() {
    alog::Options opt;
    opt.base_path = "demo.log";
    opt.min_level = alog::Level::DEBUG_;
    alog::AsyncLogger logger(opt);

    std::vector<std::thread> ths;
    for (int i = 0; i < 8; ++i) {
        ths.emplace_back([&, i] {
            for (int k = 0; k < 20000; ++k) {
                logger.log(
                    alog::Level::INFO, "hello from " + std::to_string(i)
                                           + " k=" + std::to_string(k));
            }
        });
    }
    for (auto &t : ths) t.join();
    logger.log(alog::Level::ERROR, "error sample");
    logger.log(alog::Level::FATAL, "fatal sample");
    return 0;
}
