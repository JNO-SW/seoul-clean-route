// 담당: 조효근
// 스택. 다익스트라의 prev 기록을 거꾸로 따라가 경로를 출발 → 도착 순서로 복원할 때 사용합니다.
#pragma once
#include "graph/DynamicArray.h"

template <typename T>
class Stack {
public:
    void push(const T& value);   // O(1)
    void pop();                  // O(1)
    T& top();                    // O(1)
    int size() const;
    bool empty() const;

private:
    DynamicArray<T> data_;
};

// TODO(조효근): 아래에 구현 작성
