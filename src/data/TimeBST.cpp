// 담당: 방지혁
#include "data/TimeBST.h"

TimeBST::TimeBST() {}
TimeBST::~TimeBST() {
    // TODO: 노드 메모리 해제
}

void TimeBST::insert(const Record& r) {
    // TODO
    (void)r;
}

void TimeBST::rangeSearch(const DateTime& from, const DateTime& to, DynamicArray<Record>& out) const {
    // TODO: from ~ to 범위의 노드만 중위 순회
    (void)from; (void)to; (void)out;
}

int TimeBST::size() const { return size_; }
