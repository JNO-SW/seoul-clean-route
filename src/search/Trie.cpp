// 담당: 이준호
#include "search/Trie.h"

struct Trie::Node {
    // TODO: 자식 노드 목록, 이 노드에서 끝나는 장소
};

Trie::Trie() {}
Trie::~Trie() {
    // TODO: 메모리 해제
}

void Trie::insert(const Place& place) {
    // TODO: UTF-8 한글은 글자 단위로 나눠서 저장
    (void)place;
}

void Trie::searchPrefix(const std::string& prefix, int limit, DynamicArray<Place>& out) const {
    // TODO
    (void)prefix; (void)limit; (void)out;
}
