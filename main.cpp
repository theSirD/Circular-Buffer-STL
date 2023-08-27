#include "iostream"

template<typename T>
class CircularBuffer {
public:
    T* buffer;
    int capacity;
    int size;
    int head;
    int tail;

public:
    CircularBuffer(int capacity): buffer(new T[capacity]), capacity(capacity), size(0), head(0), tail(0) {}

    ~CircularBuffer() {
        delete[] buffer;
    }

    bool isEmpty() const {
        return size == 0;
    }

    bool isFull() const {
        return size == capacity;
    }

    void push_back(const T& value) {
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        if (head == tail) {
            head = (head + 1) % capacity;
        }
        ++size;
    }

    void pop_back() {
        if (isEmpty()) {
            throw std::out_of_range("CircularBuffer is empty");
        }
        buffer[tail] = T();
        tail = (tail + capacity - 1) % capacity;
        --size;
    }

    void push_front(const T& value) {
        if (isFull()) tail = (tail + capacity - 1) % capacity;
        else size++;

        head = (head + capacity - 1) % capacity;
        buffer[head] = value;
    }

    void pop_front() {
        if (isEmpty()) {
            throw std::out_of_range("CircularBuffer is Empty");
        }
        buffer[head] = T();
        head = (head + 1) % capacity;
        --size;
    }

    class BufferIterator : public std::iterator<std::random_access_iterator_tag, T> {
    private: T *p;
    public:
        BufferIterator(T *buffer) {
            p = buffer;
        }
        BufferIterator operator+(int n) const {
            return BufferIterator(p + n);
        }
        T operator-(BufferIterator buf) const {
            return  p - buf.p;
        }
        BufferIterator& operator++() {
            ++p;
            return *this;
        }
        BufferIterator operator++(int) {
            BufferIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        BufferIterator& operator+=(int n) {
            p += n;
            return *this;
        }
        BufferIterator& operator--() {
            --p;
            return *this;
        }
        bool operator==(const BufferIterator &buf) const {
            return p == buf.p;
        }
        bool operator!=(const BufferIterator &buf) const {
            return p != buf.p;
        }
        T &operator*() const {
            return *p;
        }
        bool operator<(const BufferIterator &buf) const {
            return p < buf.p;
        }
        bool operator<=(const BufferIterator &buf) const {
            return p <= buf.p;
        }
        bool operator>(const BufferIterator &buf) const {
            return p > buf.p;
        }
        bool operator>=(const BufferIterator &buf) const {
            return p >= buf.p;
        }
    };

    void insertByIterator(const T& value, BufferIterator position) {
        int index = 0;
        for (auto it = begin(); it != position; ++it) {
            ++index;
        }

        index = (head + index) % capacity;
        if (index > size) {
            std::cout << "CircularBuffer is full" << '\n';
            return;
        }

        if (isFull()) {
            head = (head + 1) % capacity;
        }
        else {
            ++size;
        }

        int insert_index = (head + index) % capacity;
        int shift_count = (size - index - 1 + capacity) % capacity;

        if (shift_count > 0) {
            int source_index = (insert_index + shift_count - 1) % capacity;
            int dest_index = (source_index + 1) % capacity;
            int move_count = (size - index) % capacity;

            while (move_count > 0) {
                buffer[dest_index] = buffer[source_index];
                source_index = (source_index + capacity - 1) % capacity;
                dest_index = (dest_index + capacity - 1) % capacity;
                --move_count;
            }
        }

        buffer[insert_index] = value;
        tail = (tail + 1) % capacity;
    }

    void deleteByIterator(BufferIterator position) {
        if (isEmpty()) {
            throw std::out_of_range("CircularBuffer is empty");
        }

        int index = 0;
        for (auto it = begin(); it != position; ++it) {
            ++index;
        }

        index = (head + index) % capacity;
        for (int i = index; i < size - 1; ++i) {
            buffer[(head + i) % capacity] = buffer[(head + i + 1) % capacity];
        }

        --size;

        buffer[capacity-1] = T();
        tail = (tail + capacity - 1) % capacity;


    }

    BufferIterator begin() {
        return buffer + head;
    }

    BufferIterator end() {
        return buffer + tail;
    }

    T& operator[] (int index) {
        return buffer[index % capacity];
    }

    void changeCapacity(int newCapacity) {
        if (newCapacity == capacity) {
            return;
        }
        T* new_buffer = new T[newCapacity];
        int count = 0;
        int new_size = (size < newCapacity) ? size : newCapacity;
        for (auto it = begin(); count < new_size; ++it) {
            new_buffer[count++] = *it;
        }
        delete[] buffer;
        buffer = new_buffer;
        capacity = newCapacity;
        head = 0;
        tail = new_size-1;
        size = new_size;
    }
};

template<typename T>
void print(CircularBuffer<T> &a) {
    std::cout << "Elements: ";
    for (auto it = a.begin();it <= a.end(); it++){
        std::cout << *it << ' ';
    }
    std::cout << '\n';
    std::cout << "Head: " << a.head << " " << "Tail: " << a.tail << '\n';
}

int main() {
    CircularBuffer<int> buffer(4);

    buffer.push_back(2);
    buffer.push_back(1);
    buffer.push_back(5);
    buffer.push_back(6);
    buffer.push_front(3);
    print(buffer); // 3 1 5 6

    buffer.pop_back();
    print(buffer); // 3 1 5

    std::sort(buffer.begin(), buffer.end()+1);
    print(buffer); // 1 3 5

    buffer.pop_front();
    print(buffer); //3 5

    buffer.push_front(6);
    print(buffer); // 6 3 5

    buffer.deleteByIterator(buffer.begin()+1);
    print(buffer); // 6 5

    std::cout << buffer[1] << '\n'; // 5

    auto it = buffer.begin();
    std::cout << *it << '\n'; // 6

    buffer.insertByIterator(4, buffer.end());
    print(buffer); // 6 4 5

    buffer.changeCapacity(2);
    print(buffer); // 6 4

    return 0;
}