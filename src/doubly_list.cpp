#include <bits/stdc++.h>

struct Node {
    int val;
    Node *prev;
    Node *next;

    Node(int val) : val(val), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
  public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~DoublyLinkedList() {
        if (head != nullptr)
            for (auto ptr = head; ptr != nullptr;) {
                Node *next = ptr->next;
                delete ptr;
                ptr = next;
            }
    }

    void push_back(int val) {
        Node *newNode = new Node(val);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void print_forward() const {
        Node *cur = head;
        std::cout << "正向遍历: ";
        while (cur) {
            std::cout << cur->val << " <-> ";
            cur = cur->next;
        }
        std::cout << "NULL\n";
    }

  private:
    Node *head;
    Node *tail;
};

int main() {
    DoublyLinkedList list;

    // 尾插
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    list.print_forward();
}