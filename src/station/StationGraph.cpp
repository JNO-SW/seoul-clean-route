// 담당: 장데니스
#include "station/StationGraph.h"
#include "station/TopKHeap.h"

StationGraph::StationGraph(const DynamicArray<Station>& stations, int k) : stations_(stations) {
    // TODO: 관측소마다 가까운 k개 연결
    (void)k;
}

void StationGraph::topK(int k, DynamicArray<RankItem>& out) const {
    // TODO
    (void)k; (void)out;
}

void StationGraph::worstK(int k, DynamicArray<RankItem>& out) const {
    // TODO
    (void)k; (void)out;
}

void StationGraph::estimateByWind(int steps, DynamicArray<double>& outPm25) const {
    // TODO
    (void)steps; (void)outPm25;
}
