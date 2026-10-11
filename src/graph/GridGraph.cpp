// 담당: 김재겸
#include "graph/GridGraph.h"

GridGraph::GridGraph(const Grid& grid) : grid_(grid) {}

int GridGraph::vertexCount() const {
    // TODO
    return 0;
}

void GridGraph::neighbors(int u, DynamicArray<int>& out) const {
    // TODO: 8방향 이웃 중 격자 안 + 통행 가능한 칸만 out에 추가
    (void)u; (void)out;
}

double GridGraph::weight(int u, int v, const WeightOption& opt) const {
    // TODO: length(u, v) × (1 + alpha × 미세먼지 비율 [+ beta × 기온 비율])
    (void)u; (void)v; (void)opt;
    return 0.0;
}

double GridGraph::length(int u, int v) const {
    // TODO
    (void)u; (void)v;
    return 0.0;
}

int GridGraph::cellAt(double lat, double lng) const {
    // TODO
    (void)lat; (void)lng;
    return -1;
}

void GridGraph::cellCenter(int cell, double& lat, double& lng) const {
    // TODO
    (void)cell; lat = 0.0; lng = 0.0;
}
