// 담당: 방지혁
#include "data/TimeAVL.h"

TimeAVL::TimeAVL() {}
TimeAVL::~TimeAVL() {
    // TODO: 노드 메모리 해제 (후위 순회)
}

void TimeAVL::insert(const Record& r) {
    // TODO: root_ = insertNode(root_, r);
    (void)r;
}

void TimeAVL::rangeSearch(const DateTime& from, const DateTime& to, DynamicArray<Record>& out) const {
    // TODO: from ~ to 범위에 걸치는 노드만 중위 순회
    (void)from; (void)to; (void)out;
}

int TimeAVL::size() const { return size_; }
int TimeAVL::height() const { return heightOf(root_); }

int TimeAVL::heightOf(Node* n) { return n ? n->height : 0; }

TimeAVL::Node* TimeAVL::rotateLeft(Node* x) {
    // TODO
    return x;
}

TimeAVL::Node* TimeAVL::rotateRight(Node* y) {
    // TODO
    return y;
}

TimeAVL::Node* TimeAVL::rebalance(Node* n) {
    // TODO: 균형 인수(왼쪽 높이 - 오른쪽 높이)가 ±2이면 LL/LR/RR/RL 회전
    return n;
}

TimeAVL::Node* TimeAVL::insertNode(Node* n, const Record& r) {
    // TODO
    (void)r;
    return n;
}
