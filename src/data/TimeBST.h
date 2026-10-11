// 담당: 방지혁
// 측정 시각을 키로 하는 이진 탐색 트리. 선택한 기간의 기록만 빠르게 꺼냅니다.
#pragma once
#include "common/Types.h"

// 한 시각, 한 관측소의 측정 기록
struct Record {
    DateTime time;
    int stationIndex = -1;   // 관측소 목록에서의 위치
    double pm25 = -1.0, pm10 = -1.0, temp = -999.0, humidity = -1.0, windDir = -1.0, windSpeed = -1.0;
};

class TimeBST {
public:
    TimeBST();
    ~TimeBST();

    void insert(const Record& r);                                      // O(log n) 평균
    void rangeSearch(const DateTime& from, const DateTime& to,
                     DynamicArray<Record>& out) const;                 // O(log n + k)
    int size() const;

private:
    struct Node {
        long long key;
        DynamicArray<Record> records;   // 같은 시각의 기록들
        Node* left = nullptr;
        Node* right = nullptr;
    };
    Node* root_ = nullptr;
    int size_ = 0;
};
