// 담당: 방지혁
// 측정 시각을 키로 하는 AVL 트리 (균형 이진 탐색 트리)
// CSV가 시각 순으로 정렬되어 있어 일반 BST에 넣으면 한쪽으로 치우치므로, 회전으로 높이를 O(log T)로 유지합니다.
#pragma once
#include "common/Types.h"

// 한 시각, 한 관측소의 측정 기록
struct Record {
    DateTime time;
    int stationIndex = -1;   // 관측소 목록에서의 위치
    double pm25 = -1.0, pm10 = -1.0, temp = -999.0, humidity = -1.0, windDir = -1.0, windSpeed = -1.0;
};

class TimeAVL {
public:
    TimeAVL();
    ~TimeAVL();

    void insert(const Record& r);                                      // O(log T), 같은 시각이면 그 노드의 목록에 추가
    void rangeSearch(const DateTime& from, const DateTime& to,
                     DynamicArray<Record>& out) const;                 // O(log T + k)
    int size() const;                                                  // 전체 기록 수
    int height() const;                                                // 트리 높이 (균형 확인·테스트용)

private:
    struct Node {
        long long key;
        DynamicArray<Record> records;   // 같은 시각의 기록들
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;
    };
    Node* root_ = nullptr;
    int size_ = 0;

    // TODO(방지혁): 아래 보조 함수 구현
    static int heightOf(Node* n);
    static Node* rotateLeft(Node* x);
    static Node* rotateRight(Node* y);
    static Node* rebalance(Node* n);
    Node* insertNode(Node* n, const Record& r);
};
