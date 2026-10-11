// 담당: 김재겸
// 격자 그래프. 칸 = 정점, 상하좌우·대각선 8방향 이웃 = 간선
// 이웃을 따로 저장하지 않고 칸 번호로 바로 계산합니다. (메모리 절약)
#pragma once
#include "common/Types.h"

class GridGraph {
public:
    explicit GridGraph(const Grid& grid);

    int vertexCount() const;                        // 전체 칸 수

    // 칸 u의 통행 가능한 이웃 칸 번호를 out에 채움 (최대 8개)
    void neighbors(int u, DynamicArray<int>& out) const;

    // 칸 u → v 간선 비용 (거리 × 환경 비용)
    double weight(int u, int v, const WeightOption& opt) const;

    double length(int u, int v) const;              // 두 칸 중심 사이 거리 (m)

    int cellAt(double lat, double lng) const;       // 위경도 → 가장 가까운 칸 번호 (범위 밖이면 -1)
    void cellCenter(int cell, double& lat, double& lng) const;  // 칸 번호 → 중심 위경도

    const Grid& grid() const { return grid_; }

private:
    const Grid& grid_;
};
