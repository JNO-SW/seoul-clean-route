// 담당: 방지혁
// 해시 테이블 (체이닝). 격자 칸 번호 → 그 칸 주변의 관측소 목록(공간 버킷)에 사용합니다.
// 템플릿이므로 구현도 이 파일 안에 작성합니다.
#pragma once

#include "graph/DynamicArray.h"
#include <string>

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
    // 체이닝 방식을 위한 엔트리 구조체
    struct Entry {
        K key;
        V value;
        Entry* next;
        Entry(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    Entry** buckets_;
    int bucketCount_;
    int size_;

    // std::string 키에 대한 해시 함수 (DJB2 변형)
    unsigned int hash(const std::string& key) const {
        unsigned int h = 0;
        for (char c : key) {
            h = h * 31 + static_cast<unsigned char>(c);
        }
        return h % bucketCount_;
    }

    // 정수형 키에 대한 기본 해시 함수
    unsigned int hash(int key) const {
        return static_cast<unsigned int>(key) % bucketCount_;
    }
};

// ==========================================
// 구현부 (HashTable 메서드 정의)
// ==========================================

template <typename K, typename V>
HashTable<K, V>::HashTable(int bucketCount) : bucketCount_(bucketCount), size_(0) {
    buckets_ = new Entry * [bucketCount_]();
}

template <typename K, typename V>
HashTable<K, V>::~HashTable() {
    for (int i = 0; i < bucketCount_; ++i) {
        Entry* curr = buckets_[i];
        while (curr) {
            Entry* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] buckets_;
}

template <typename K, typename V>
void HashTable<K, V>::put(const K& key, const V& value) {
    unsigned int index = hash(key);
    Entry* curr = buckets_[index];

    // 이미 존재하는 키라면 값 업데이트
    while (curr) {
        if (curr->key == key) {
            curr->value = value;
            return;
        }
        curr = curr->next;
    }

    // 신규 키라면 맨 앞에 삽입 (체이닝)
    Entry* newEntry = new Entry(key, value);
    newEntry->next = buckets_[index];
    buckets_[index] = newEntry;
    size_++;
}

template <typename K, typename V>
V* HashTable<K, V>::get(const K& key) {
    unsigned int index = hash(key);
    Entry* curr = buckets_[index];
    while (curr) {
        if (curr->key == key) {
            return &(curr->value);
        }
        curr = curr->next;
    }
    return nullptr;
}

template <typename K, typename V>
bool HashTable<K, V>::contains(const K& key) const {
    unsigned int index = hash(key);
    Entry* curr = buckets_[index];
    while (curr) {
        if (curr->key == key) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

template <typename K, typename V>
int HashTable<K, V>::size() const {
    return size_;
}