// 담당: 이준호
// 트라이. 지역명을 글자 단위 노드로 저장해 앞 글자만 입력해도 후보를 찾습니다.
#pragma once
#include <string>
#include "graph/DynamicArray.h"

struct Place {
    std::string name;
    double lat = 0.0, lng = 0.0;
};

class Trie {
public:
    Trie();
    ~Trie();

    void insert(const Place& place);                                         // O(이름 길이)
    void searchPrefix(const std::string& prefix, int limit,
                      DynamicArray<Place>& out) const;                       // O(입력 길이 + 후보 수)

private:
    struct Node;
    Node* root_ = nullptr;

    static void destroy(Node* n);
    static void collect(const Node* n, int limit, DynamicArray<Place>& out);

    Trie(const Trie&) = delete;              // 노드를 직접 관리하므로 복사 금지
    Trie& operator=(const Trie&) = delete;
};
