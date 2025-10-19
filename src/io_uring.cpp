#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <liburing.h>

int main() {
    struct io_uring ring;
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    char buf[128] = {0};

    io_uring_queue_init(1, &ring, 0);

    int fd = open("../../test.txt", O_RDONLY);
    if (fd < 0) return 1;

    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, sizeof(buf) - 1, 0);
    io_uring_submit(&ring);

    io_uring_wait_cqe(&ring, &cqe);
    if (cqe->res >= 0) printf("Read %d bytes: %s\n", cqe->res, buf);
    else
        fprintf(stderr, "Read error: %d\n", cqe->res);

    io_uring_cqe_seen(&ring, cqe);
    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}
