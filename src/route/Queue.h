// 담당: 조효근
// 원형 큐. 출발·도착 칸이 통행 불가 칸일 때 BFS로 가장 가까운 통행 가능 칸을 찾는 데 사용합니다.
// 템플릿이므로 구현도 이 파일 안에 작성합니다.
#pragma once
#include "graph/DynamicArray.h"

template <typename T>
class Queue {
public:
    explicit Queue(int capacity = 16);

    void enqueue(const T& value);   // O(1), 꽉 차면 용량 2배로 늘리고 순서대로 다시 배치
    void dequeue();                 // O(1)
    T& front();                     // O(1)
    int size() const;
    bool empty() const;

private:
    DynamicArray<T> data_;
    int head_ = 0;    // 맨 앞 위치
    int count_ = 0;   // 들어 있는 개수
};

// TODO(조효근): 아래에 구현 작성

template <typename T>
Queue<T>::Queue(int capacity) {
    if (capacity < 1) capacity = 16;
    data_.resize(capacity);
    head_ = 0;
    count_ = 0;
}

template <typename T>
void Queue<T>::enqueue(const T& value) {
    int cap = data_.capacity();
    if (cap == 0) {
        cap = 16;
        data_.resize(cap);
    }

    // 꽉 찼으면 2배 재배치
    if (count_ == cap) {
        int newCap = cap * 2;
        DynamicArray<T> newData;
        newData.resize(newCap);

        for (int i = 0; i < count_; ++i) {
            newData[i] = data_[(head_ + i) % cap];
        }

        data_ = newData;
        head_ = 0;
        cap = newCap;
    }

    int tail = (head_ + count_) % cap;
    data_[tail] = value;
    ++count_;
}

template <typename T>
void Queue<T>::dequeue() {
    if (count_ > 0) {
        int cap = data_.capacity();
        head_ = (head_ + 1) % cap;
        --count_;
    }
}

template <typename T>
T& Queue<T>::front() {
    return data_[head_];
}

template <typename T>
int Queue<T>::size() const {
    return count_;
}

template <typename T>
bool Queue<T>::empty() const {
    return count_ == 0;
}
