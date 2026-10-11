// 담당: 장데니스
// 크기 k 힙. 전체를 정렬하지 않고 점수 상위(또는 하위) k개만 유지합니다. O(n log k)
#pragma once
#include "graph/DynamicArray.h"

template <typename T>
class TopKHeap {
public:
    // keepSmallest = true: 점수가 작은 k개 유지 (예: 미세먼지가 낮은 Top k)
    TopKHeap(int k, bool keepSmallest);

    void offer(double score, const T& value);              // 후보 하나 넣기, O(log k)
    void result(DynamicArray<T>& out) const;                // 유지 중인 k개를 순서대로 out에 채움
    int size() const;

private:
    // TODO(장데니스): 내부 힙 배열
};

// TODO(장데니스): 아래에 구현 작성
