// 담당: 이준호
// 동적 배열. std::vector 대신 모든 모듈에서 사용합니다.
// 템플릿이므로 구현도 이 파일 안에 있습니다.
#pragma once
#include <cstdio>
#include <cstdlib>

template <typename T>
class DynamicArray {
public:
    DynamicArray() {}

    explicit DynamicArray(int initialCapacity) { reserve(initialCapacity); }

    // 깊은 복사: 새 메모리를 잡고 값을 하나씩 복사
    DynamicArray(const DynamicArray& other) {
        reserve(other.size_);
        for (int i = 0; i < other.size_; ++i) data_[i] = other.data_[i];
        size_ = other.size_;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;
        DynamicArray copy(other);   // 복사본을 만든 뒤 내용을 맞바꿈
        swap(copy);
        return *this;
    }

    ~DynamicArray() { delete[] data_; }

    // 끝에 추가. 꽉 차면 용량을 2배로 늘림 (상각 O(1))
    void push_back(const T& value) {
        if (size_ == capacity_) reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        data_[size_++] = value;
    }

    // 끝 원소 제거 O(1)
    void pop_back() {
        if (size_ == 0) fail("pop_back: 배열이 비어 있음");
        --size_;
    }

    // 인덱스 접근 O(1), 범위를 벗어나면 오류 메시지 후 종료
    T& operator[](int index) {
        if (index < 0 || index >= size_) failIndex(index);
        return data_[index];
    }
    const T& operator[](int index) const {
        if (index < 0 || index >= size_) failIndex(index);
        return data_[index];
    }

    T& back() {
        if (size_ == 0) fail("back: 배열이 비어 있음");
        return data_[size_ - 1];
    }

    int size() const { return size_; }
    bool empty() const { return size_ == 0; }
    int capacity() const { return capacity_; }

    // 원소만 비움 (메모리는 유지)
    void clear() { size_ = 0; }

    // 크기를 newSize로 바꾸고, 새로 생긴 칸은 fill로 채움
    void resize(int newSize, const T& fill = T()) {
        if (newSize < 0) fail("resize: 음수 크기");
        if (newSize > capacity_) reserve(newSize);
        for (int i = size_; i < newSize; ++i) data_[i] = fill;
        size_ = newSize;
    }

    // 용량을 최소 newCapacity로 확보 (값은 그대로 유지)
    void reserve(int newCapacity) {
        if (newCapacity <= capacity_) return;
        T* next = new T[newCapacity];
        for (int i = 0; i < size_; ++i) next[i] = data_[i];
        delete[] data_;
        data_ = next;
        capacity_ = newCapacity;
    }

    void swap(DynamicArray& other) {
        T* d = data_; data_ = other.data_; other.data_ = d;
        int s = size_; size_ = other.size_; other.size_ = s;
        int c = capacity_; capacity_ = other.capacity_; other.capacity_ = c;
    }

private:
    T* data_ = nullptr;
    int size_ = 0;
    int capacity_ = 0;

    static void fail(const char* msg) {
        std::fprintf(stderr, "[DynamicArray] %s\n", msg);
        std::abort();
    }
    void failIndex(int index) const {
        std::fprintf(stderr, "[DynamicArray] 범위를 벗어난 접근: index=%d, size=%d\n", index, size_);
        std::abort();
    }
};
