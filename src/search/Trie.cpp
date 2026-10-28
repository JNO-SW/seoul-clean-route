// 담당: 이준호
// UTF-8 문자열을 글자 단위로 나눠 노드 하나에 한 글자씩 저장합니다.
// 한글 한 글자는 3바이트이므로 바이트 단위가 아니라 글자 단위로 나눠야 "강남"처럼 앞 글자 검색이 됩니다.
#include "search/Trie.h"

struct Trie::Node {
    struct Child {
        std::string ch;        // 글자 하나 (UTF-8 1~4바이트)
        Node* node = nullptr;
    };
    DynamicArray<Child> children;   // 자식 노드 목록 (글자 수가 적어 선형 탐색)
    DynamicArray<Place> places;     // 이 노드에서 이름이 끝나는 장소들

    Node* find(const std::string& ch) const {
        for (int i = 0; i < children.size(); ++i)
            if (children[i].ch == ch) return children[i].node;
        return nullptr;
    }
};

// s의 pos 위치에서 시작하는 UTF-8 글자 한 개의 바이트 수
static int charLength(const std::string& s, int pos) {
    unsigned char c = (unsigned char)s[pos];
    int len = 1;
    if (c >= 0xF0) len = 4;
    else if (c >= 0xE0) len = 3;
    else if (c >= 0xC0) len = 2;
    if (pos + len > (int)s.size()) len = (int)s.size() - pos;   // 잘린 글자 방어
    return len;
}

Trie::Trie() : root_(new Node()) {}

Trie::~Trie() { destroy(root_); }

void Trie::destroy(Node* n) {
    if (n == nullptr) return;
    for (int i = 0; i < n->children.size(); ++i) destroy(n->children[i].node);
    delete n;
}

void Trie::insert(const Place& place) {
    Node* cur = root_;
    const std::string& s = place.name;
    int pos = 0;
    while (pos < (int)s.size()) {
        int len = charLength(s, pos);
        std::string ch = s.substr(pos, len);
        Node* next = cur->find(ch);
        if (next == nullptr) {
            next = new Node();
            Node::Child child;
            child.ch = ch;
            child.node = next;
            cur->children.push_back(child);
        }
        cur = next;
        pos += len;
    }
    // 같은 이름이 이미 있으면 위치만 갱신
    for (int i = 0; i < cur->places.size(); ++i) {
        if (cur->places[i].name == place.name) { cur->places[i] = place; return; }
    }
    cur->places.push_back(place);
}

// 깊이 우선으로 내려가며 이름이 끝나는 장소를 limit개까지 모음
void Trie::collect(const Node* n, int limit, DynamicArray<Place>& out) {
    for (int i = 0; i < n->places.size() && out.size() < limit; ++i) out.push_back(n->places[i]);
    for (int i = 0; i < n->children.size() && out.size() < limit; ++i) collect(n->children[i].node, limit, out);
}

void Trie::searchPrefix(const std::string& prefix, int limit, DynamicArray<Place>& out) const {
    out.clear();
    if (limit <= 0) return;
    const Node* cur = root_;
    int pos = 0;
    while (pos < (int)prefix.size()) {
        int len = charLength(prefix, pos);
        cur = cur->find(prefix.substr(pos, len));
        if (cur == nullptr) return;   // 일치하는 이름 없음
        pos += len;
    }
    collect(cur, limit, out);
}
