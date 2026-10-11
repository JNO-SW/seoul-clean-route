// 담당: 조효근
// 다익스트라로 최적 경로 탐색, 경로 복원, 최단 경로와 비교
#pragma once
#include "common/Types.h"
#include "graph/GridGraph.h"

// start → goal 최소 비용 경로를 찾아 out에 채움. 경로가 없으면 false
bool findRoute(const GridGraph& graph, int start, int goal, const WeightOption& opt, Route& out);

// 최적 경로(opt)와 최단 경로(alpha = 0)를 함께 계산
bool compareRoutes(const GridGraph& graph, int start, int goal, const WeightOption& opt,
                   Route& best, Route& shortest);
