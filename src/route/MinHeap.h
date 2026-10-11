// 담당: 조효근
// 최소 힙 (우선순위 큐). 다익스트라에서 누적 비용이 가장 작은 칸을 꺼낼 때 사용합니다.
// 템플릿이므로 구현도 이 파일 안에 작성합니다.
#pragma once
#include "graph/DynamicArray.h"

template <typename T>
class MinHeap {
public:
    struct Item {
        double priority;   // 작을수록 먼저 나옴
        T value;
    };

    void push(double priority, const T& value);   // O(log n)
    Item pop();                                   // 가장 작은 항목을 꺼냄, O(log n)
    const Item& top() const;                      // O(1)
    int size() const;
    bool empty() const;

private:
    DynamicArray<Item> heap_;   // 완전 이진 트리를 배열로 저장 (부모 = (i-1)/2)
};

// TODO(조효근): 아래에 구현 작성
