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
