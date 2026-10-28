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

template <typename T>
void MinHeap<T>::push(double priority, const T& value) {
    // 1. 맨 끝에 새로운 아이템 추가
    Item newItem;
    newItem.priority = priority;
    newItem.value = value;
    heap_.push_back(newItem);

    // 2. 부모와 비교하며 위로 올리기 (Bubble up)
    int current = heap_.size() - 1;
    while (current > 0) {
        int parent = (current - 1) / 2;
        
        // 부모보다 내 우선순위가 작다면 교환(Swap)
        if (heap_[current].priority < heap_[parent].priority) {
            Item temp = heap_[current];
            heap_[current] = heap_[parent];
            heap_[parent] = temp;
            
            // 현재 위치를 부모 위치로 갱신
            current = parent;
        } else {
            // 부모가 더 작거나 같으면 올바른 위치이므로 반복 종료
            break;
        }
    }
}

template <typename T>
typename MinHeap<T>::Item MinHeap<T>::pop() {
    if (heap_.empty()) {
        std::fprintf(stderr, "[MinHeap] pop: 힙이 비어 있음\n");
        std::abort();
    }

    // 1. 맨 위(우선순위가 가장 작은 값) 저장
    Item root = heap_[0];

    // 2. 맨 끝 값을 맨 위로 옮기고, 맨 끝 값 제거
    heap_[0] = heap_.back();
    heap_.pop_back();

    // 3. 자식 중 더 작은 쪽과 교환하며 아래로 내리기 (Bubble down)
    int current = 0;
    int size = heap_.size();

    while (true) {
        int leftChild = 2 * current + 1;
        int rightChild = 2 * current + 2;
        int smallest = current;

        // 왼쪽 자식이 존재하고, 현재(smallest)보다 우선순위가 작다면 갱신
        if (leftChild < size && heap_[leftChild].priority < heap_[smallest].priority) {
            smallest = leftChild;
        }
        
        // 오른쪽 자식이 존재하고, 현재(smallest)보다 우선순위가 작다면 갱신
        if (rightChild < size && heap_[rightChild].priority < heap_[smallest].priority) {
            smallest = rightChild;
        }

        // 갱신되었다면 자식과 교환
        if (smallest != current) {
            Item temp = heap_[current];
            heap_[current] = heap_[smallest];
            heap_[smallest] = temp;
            
            current = smallest;
        } else {
            // 더 이상 작아질 자식이 없으면 올바른 위치이므로 반복 종료
            break;
        }
    }

    return root;
}

template <typename T>
const typename MinHeap<T>::Item& MinHeap<T>::top() const {
    if (heap_.empty()) {
        std::fprintf(stderr, "[MinHeap] top: 힙이 비어 있음\n");
        std::abort();
    }
    return heap_[0];
}

template <typename T>
int MinHeap<T>::size() const {
    return heap_.size();
}

template <typename T>
bool MinHeap<T>::empty() const {
    return heap_.empty();
}
