// 담당: 조효근
#include "route/Router.h"
#include "route/MinHeap.h"
#include "route/Stack.h"
#include "route/Queue.h"

int nearestPassable(const GridGraph& graph, int c) {
    // TODO: c가 통행 가능하면 c, 아니면 Queue로 BFS
    (void)graph;
    return c;
}

bool findRoute(const GridGraph& graph, int start, int goal, const WeightOption& opt, Route& out) {
    // TODO: 최소 힙 기반 다익스트라 → prev를 Stack으로 뒤집어 경로 복원 → 거리·노출량·시간 계산
    (void)graph; (void)start; (void)goal; (void)opt; (void)out;
    return false;
}

bool compareRoutes(const GridGraph& graph, int start, int goal, const WeightOption& opt,
                   Route& best, Route& shortest) {
    // TODO
    (void)graph; (void)start; (void)goal; (void)opt; (void)best; (void)shortest;
    return false;
}
