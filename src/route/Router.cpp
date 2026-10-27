// 담당: 조효근
#include <cstdio>
#include "route/Router.h"
#include "route/MinHeap.h"
#include "route/Stack.h"
#include "route/Queue.h"

int nearestPassable(const GridGraph& graph, int c) {
    int n = graph.vertexCount();
    if (c < 0 || c >= n) return -1;
    
    // 이미 통행 가능하면 자기 자신 반환
    if (graph.grid().passable[c]) {
        return c;
    }

    // BFS를 위한 방문 처리 배열
    DynamicArray<bool> visited;
    visited.resize(n, false);

    Queue<int> q;
    q.enqueue(c);
    visited[c] = true;

    DynamicArray<int> nbors;

    // BFS 탐색
    while (!q.empty()) {
        int u = q.front();
        q.dequeue();

        // 큐에서 꺼낸 칸이 통행 가능하면 가장 가까운 칸이므로 즉시 반환
        if (graph.grid().passable[u]) {
            return u;
        }

        nbors.clear();
        graph.neighbors(u, nbors);

        for (int i = 0; i < nbors.size(); ++i) {
            int v = nbors[i];
            if (!visited[v]) {
                visited[v] = true;
                q.enqueue(v);
            }
        }
    }
    
    return -1; // 통행 가능한 칸을 찾지 못한 경우
}

bool findRoute(const GridGraph& graph, int start, int goal, const WeightOption& opt, Route& out) {
    int n = graph.vertexCount();
    if (start < 0 || start >= n || goal < 0 || goal >= n) return false;

    // 다익스트라 초기화
    double INF = 1e18; // 무한대를 나타내는 아주 큰 값
    DynamicArray<double> dist;
    dist.resize(n, INF);
    DynamicArray<int> prev;
    prev.resize(n, -1);

    dist[start] = 0.0;

    MinHeap<int> pq;
    pq.push(0.0, start);

    DynamicArray<int> nbors;

    // 다익스트라 메인 루프
    while (!pq.empty()) {
        MinHeap<int>::Item item = pq.pop();
        double d = item.priority;
        int u = item.value;

        // 목표 칸에 도달하면 탐색 종료
        if (u == goal) break;

        // 큐에서 꺼낸 거리가 현재 기록된 거리보다 크면 무시 (최적화)
        if (d > dist[u]) continue;

        nbors.clear();
        graph.neighbors(u, nbors);

        for (int i = 0; i < nbors.size(); ++i) {
            int v = nbors[i];
            double w = graph.weight(u, v, opt);

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push(dist[v], v);
            }
        }
    }

    // 목표지에 도달할 수 없는 경우 (경로 없음)
    if (dist[goal] == INF && start != goal) {
        return false;
    }

    // 도착지부터 역추적하여 스택에 삽입
    Stack<int> pathStack;
    int curr = goal;
    while (curr != -1) {
        pathStack.push(curr);
        curr = prev[curr];
    }

    // 출력 데이터 초기화
    out.lats.clear();
    out.lngs.clear();
    out.distance = 0.0;
    out.exposure = 0.0;
    out.minutes = 0.0;

    int prev_node = -1;

    // 스택에서 꺼내며 출발지 -> 도착지 순서로 복원 및 수치 계산
    while (!pathStack.empty()) {
        int u = pathStack.top();
        pathStack.pop();

        double lat = 0.0, lng = 0.0;
        graph.cellCenter(u, lat, lng);
        out.lats.push_back(lat);
        out.lngs.push_back(lng);

        // 첫 번째 칸(출발지)이 아니면 거리 및 미세먼지 계산 누적
        if (prev_node != -1) {
            double len = graph.length(prev_node, u);
            out.distance += len;
            
            double pm_prev = graph.grid().pm25[prev_node];
            double pm_u = graph.grid().pm25[u];
            out.exposure += len * ((pm_prev + pm_u) / 2.0); // 거리 × 평균 미세먼지
        }
        prev_node = u;
    }

    // 시간 계산: 4.5km/h = 분당 75m 이동 (4500 / 60 = 75)
    out.minutes = out.distance / 75.0;

    return true;
}

bool compareRoutes(const GridGraph& graph, int start, int goal, const WeightOption& opt,
                   Route& best, Route& shortest) {
    // 1. 주어진 가중치 비중(opt)을 적용한 최적 경로 탐색
    bool foundBest = findRoute(graph, start, goal, opt, best);
    
    // 2. 미세먼지 비중을 0으로 강제 설정하여 최단 경로(거리 기준) 탐색
    WeightOption shortOpt;
    shortOpt.alpha = 0.0;
    shortOpt.beta = 0.0;
    bool foundShortest = findRoute(graph, start, goal, shortOpt, shortest);
    
    return foundBest && foundShortest;
}