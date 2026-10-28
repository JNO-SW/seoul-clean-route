// 담당: 장데니스
#include "station/StationGraph.h"
#include "station/TopKHeap.h"

StationGraph::StationGraph(
    const DynamicArray<Station>& stations,
    int k
) : stations_(stations) {
    // 이웃 연결은 최종 발표 때 구현
    (void)k;
}

void StationGraph::topK(
    int k,
    DynamicArray<RankItem>& out
) const {
    out.clear();

    if (k <= 0) return;

    // 낮은 PM2.5 값 k개 유지
    TopKHeap<RankItem> heap(k, true);

    for (int i = 0; i < stations_.size(); ++i) {
        const Station& station = stations_[i];

        // PM2.5 값이 없으면 순위에서 제외
        if (station.pm25 < 0.0) {
            continue;
        }

        RankItem item;
        item.name = station.name;
        item.score = station.pm25;

        heap.offer(item.score, item);
    }

    heap.result(out);
}

void StationGraph::worstK(
    int k,
    DynamicArray<RankItem>& out
) const {
    out.clear();

    if (k <= 0) return;

    // 높은 PM2.5 값 k개 유지
    TopKHeap<RankItem> heap(k, false);

    for (int i = 0; i < stations_.size(); ++i) {
        const Station& station = stations_[i];

        // PM2.5 값이 없으면 순위에서 제외
        if (station.pm25 < 0.0) {
            continue;
        }

        RankItem item;
        item.name = station.name;
        item.score = station.pm25;

        heap.offer(item.score, item);
    }

    heap.result(out);
}

void StationGraph::estimateByWind(
    int steps,
    DynamicArray<double>& outPm25
) const {
    // 최종 발표 때 구현 예정
    (void)steps;
    (void)outPm25;
}
