#include <iostream>
#include <mutex>

struct Node
{
	int value;
	Node* next;
	std::mutex* node_mutex;

    Node(int val) : value(val), next(nullptr), node_mutex(new std::mutex()) {}
    ~Node() { delete node_mutex; }
};

class FineGrainedQueue
{
private:
    Node* head;
    std::mutex* queue_mutex;

public:
    FineGrainedQueue() { 
        head = new Node(0);
        queue_mutex = new std::mutex;
    }
    ~FineGrainedQueue() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        delete queue_mutex;
    }
    void insertIntoMiddle(int value, int pos) {
        if (head == nullptr || pos <= 0) {
            return; 
        }

        Node* prev = head;
        std::unique_lock<std::mutex> lock_prev(*prev->node_mutex);

        Node* curr = prev->next;
        std::unique_lock<std::mutex> lock_curr;
        int index = 1;

        while (curr != nullptr && index < pos) {
            lock_curr = std::unique_lock<std::mutex>(*curr->node_mutex);
            lock_prev.unlock();

            prev = curr;
            curr = curr->next;
            lock_prev = std::move(lock_curr);
            index++;
        }

        Node* new_node = new Node (value);
        new_node->next = curr;
        prev->next = new_node;
    }

    void print() {
        Node* curr = head;
        while (curr != nullptr) {
            std::cout << curr->value << "--";
            curr = curr->next;
        }
        std::cout << "end";
    }
};

int main() {
    FineGrainedQueue queue;

    queue.insertIntoMiddle(3, 1);
    queue.insertIntoMiddle(11, 2);
    queue.insertIntoMiddle(34, 3);
    queue.insertIntoMiddle(40, 2);

    queue.print();

    return 0; 
}