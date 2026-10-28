// 담당: 장데니스
// 크기 k 힙. 전체를 정렬하지 않고 점수 상위(또는 하위) k개만 유지합니다. O(n log k)
#pragma once
#include "graph/DynamicArray.h"

template <typename T>
class TopKHeap {
public:
    // keepSmallest = true:
    // 점수가 작은 k개 유지 → 내부는 최대 힙
    //
    // keepSmallest = false:
    // 점수가 큰 k개 유지 → 내부는 최소 힙
    TopKHeap(int k, bool keepSmallest)
        : k_(k < 0 ? 0 : k),
          keepSmallest_(keepSmallest) {}

    void offer(double score, const T& value) {
        if (k_ <= 0) return;

        Entry candidate;
        candidate.score = score;
        candidate.value = value;

        // 아직 k개가 안 찼으면 그냥 추가
        if (heap_.size() < k_) {
            heap_.push_back(candidate);
            siftUp(heap_.size() - 1);
            return;
        }

        // 새 후보가 현재 힙의 root보다 더 좋은 값이면 교체
        if (better(candidate, heap_[0])) {
            heap_[0] = candidate;
            siftDown(0);
        }
    }

    // 좋은 순서대로 out에 저장
    // keepSmallest=true  → 낮은 점수부터
    // keepSmallest=false → 높은 점수부터
    // 점수가 같으면 이름 오름차순
    void result(DynamicArray<T>& out) const {
        out.clear();

        // 원본 힙을 건드리지 않도록 복사
        TopKHeap<T> copy(*this);

        int n = copy.size();
        out.resize(n);

        // root에는 현재 남아 있는 것 중 "가장 나쁜 값"이 있으므로
        // 뒤에서부터 채우면 최종 결과는 좋은 순서가 됨
        for (int i = n - 1; i >= 0; --i) {
            Entry item = copy.removeRoot();
            out[i] = item.value;
        }
    }

    int size() const {
        return heap_.size();
    }

private:
    struct Entry {
        double score = 0.0;
        T value;
    };

    int k_ = 0;
    bool keepSmallest_ = true;
    DynamicArray<Entry> heap_;

    // a가 b보다 최종 순위에서 더 좋은가?
    bool better(const Entry& a, const Entry& b) const {
        if (a.score != b.score) {
            if (keepSmallest_) {
                return a.score < b.score;
            } else {
                return a.score > b.score;
            }
        }

        // 점수가 같으면 이름 순서
        return a.value.name < b.value.name;
    }

    // a가 b보다 더 나쁜가?
    // 힙의 root에 "현재 유지 중 가장 나쁜 값"을 두기 위해 사용
    bool worse(const Entry& a, const Entry& b) const {
        return better(b, a);
    }

    void swapEntries(int a, int b) {
        Entry temp = heap_[a];
        heap_[a] = heap_[b];
        heap_[b] = temp;
    }

    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (!worse(heap_[index], heap_[parent])) {
                break;
            }

            swapEntries(index, parent);
            index = parent;
        }
    }

    void siftDown(int index) {
        while (true) {
            int left = index * 2 + 1;
            int right = left + 1;
            int worstIndex = index;

            if (left < heap_.size() &&
                worse(heap_[left], heap_[worstIndex])) {
                worstIndex = left;
            }

            if (right < heap_.size() &&
                worse(heap_[right], heap_[worstIndex])) {
                worstIndex = right;
            }

            if (worstIndex == index) {
                break;
            }

            swapEntries(index, worstIndex);
            index = worstIndex;
        }
    }

    Entry removeRoot() {
        Entry root = heap_[0];

        heap_[0] = heap_.back();
        heap_.pop_back();

        if (!heap_.empty()) {
            siftDown(0);
        }

        return root;
    }
};
