// 담당: 김재겸
#include "graph/GridGraph.h"
#include <cmath>

namespace {
const double PI = 3.14159265358979323846;

const double PM_REF = 35.0;        // 초미세먼지 기준값 (㎍/㎥, '보통' 상한)
const double TEMP_COMFORT = 15.0;  // 쾌적 기온 (℃)
const double TEMP_REF = 10.0;      // 기온 차이 기준값 (℃)

// 8방향: 상하좌우 4개 다음 대각선 4개
const int DR[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
const int DC[8] = {0, 0, -1, 1, -1, 1, -1, 1};
}  // namespace

GridGraph::GridGraph(const Grid& grid) : grid_(grid) {
    // 위도 1도 ≈ 110,990m (서울 위도 기준), 경도 1도 ≈ 111,320m × cos(위도)
    double midLat = grid_.minLat + grid_.rows * grid_.cellLat / 2.0;
    metersPerLat_ = 110990.0;
    metersPerLng_ = 111320.0 * std::cos(midLat * PI / 180.0);
}

int GridGraph::vertexCount() const {
    return grid_.rows * grid_.cols;
}

bool GridGraph::isPassable(int cell) const {
    return cell >= grid_.passable.size() || grid_.passable[cell];
}

bool GridGraph::isPassable(int row, int col) const {
    return isPassable(row * grid_.cols + col);
}

double GridGraph::pmAt(int cell) const {
    if (cell >= grid_.pm25.size() || grid_.pm25[cell] < 0) return PM_REF;
    return grid_.pm25[cell];
}

double GridGraph::tempAt(int cell) const {
    if (cell >= grid_.temp.size() || grid_.temp[cell] < -100) return TEMP_COMFORT;
    return grid_.temp[cell];
}

void GridGraph::neighbors(int u, DynamicArray<int>& out) const {
    out.clear();
    int r = u / grid_.cols;
    int c = u % grid_.cols;

    for (int d = 0; d < 8; ++d) {
        int nr = r + DR[d];
        int nc = c + DC[d];
        // 칸 번호 ±1로 계산하면 맨 왼쪽/오른쪽 열에서 옆 줄로 넘어가므로 행·열로 경계 검사
        if (nr < 0 || nr >= grid_.rows || nc < 0 || nc >= grid_.cols) continue;
        if (!isPassable(nr, nc)) continue;
        bool diagonal = (DR[d] != 0 && DC[d] != 0);
        if (diagonal && (!isPassable(r, nc) || !isPassable(nr, c))) continue;
        out.push_back(nr * grid_.cols + nc);
    }
}

double GridGraph::length(int u, int v) const {
    int dRow = u / grid_.cols - v / grid_.cols;
    int dCol = u % grid_.cols - v % grid_.cols;
    double dy = dRow * grid_.cellLat * metersPerLat_;
    double dx = dCol * grid_.cellLng * metersPerLng_;
    return std::sqrt(dx * dx + dy * dy);
}

double GridGraph::weight(int u, int v, const WeightOption& opt) const {
    double pm = (pmAt(u) + pmAt(v)) / 2.0;
    double temp = (tempAt(u) + tempAt(v)) / 2.0;
    double cost = 1.0 + opt.alpha * pm / PM_REF
                      + opt.beta * std::fabs(temp - TEMP_COMFORT) / TEMP_REF;
    return length(u, v) * cost;
}

int GridGraph::cellAt(double lat, double lng) const {
    if (grid_.cellLat <= 0 || grid_.cellLng <= 0) return -1;
    // (int) 변환은 0 쪽으로 버려서 -0.5가 0이 되므로 floor 사용
    int r = (int)std::floor((lat - grid_.minLat) / grid_.cellLat);
    int c = (int)std::floor((lng - grid_.minLng) / grid_.cellLng);
    if (r < 0 || r >= grid_.rows || c < 0 || c >= grid_.cols) return -1;
    return r * grid_.cols + c;
}

void GridGraph::cellCenter(int cell, double& lat, double& lng) const {
    int r = cell / grid_.cols;
    int c = cell % grid_.cols;
    lat = grid_.minLat + (r + 0.5) * grid_.cellLat;
    lng = grid_.minLng + (c + 0.5) * grid_.cellLng;
}
