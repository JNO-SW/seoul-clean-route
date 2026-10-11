// 담당: 방지혁
// 해시 테이블 (체이닝). 격자 칸 번호 → 그 칸 주변의 관측소 목록(공간 버킷)에 사용합니다.
// 템플릿이므로 구현도 이 파일 안에 작성합니다.
#pragma once
#include "graph/DynamicArray.h"

template <typename K, typename V>
class HashTable {
public:
    explicit HashTable(int bucketCount = 1024);
    ~HashTable();

    void put(const K& key, const V& value);   // O(1) 평균
    V* get(const K& key);                     // 없으면 nullptr, O(1) 평균
    bool contains(const K& key) const;
    int size() const;

private:
    // TODO(방지혁): 버킷 배열, 해시 함수
};

// TODO(방지혁): 아래에 구현 작성
