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
    // out은 먼저 비우고 채움. 대각선은 양옆 두 칸이 모두 통행 가능할 때만 포함 (건물 모서리 뚫기 방지)
    void neighbors(int u, DynamicArray<int>& out) const;

    // 칸 u → v 간선 비용 = 길이 × (1 + alpha × PM/35 + beta × |기온 − 15| / 10)
    // PM·기온은 두 칸의 평균. 값이 없는 칸(음수)은 기준값(PM 35, 기온 15도)으로 봄
    double weight(int u, int v, const WeightOption& opt) const;

    double length(int u, int v) const;              // 두 칸 중심 사이 거리 (m)

    int cellAt(double lat, double lng) const;       // 위경도 → 가장 가까운 칸 번호 (범위 밖이면 -1)
    void cellCenter(int cell, double& lat, double& lng) const;  // 칸 번호 → 중심 위경도

    const Grid& grid() const { return grid_; }

private:
    const Grid& grid_;
    double metersPerLat_;   // 위도 1도의 길이 (m)
    double metersPerLng_;   // 격자 가운데 위도에서 경도 1도의 길이 (m)

    // passable이 비어 있으면(OSM 반영 전) 모든 칸을 통행 가능으로 봄
    bool isPassable(int cell) const;
    bool isPassable(int row, int col) const;
    double pmAt(int cell) const;
    double tempAt(int cell) const;
};
