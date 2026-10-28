// 격자 그래프 테스트 (make test 로 실행)
#include <cstdio>
#include <cmath>
#include "graph/GridGraph.h"
#include "common/TestData.h"

static int failed = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf("  실패: %s (%s:%d)\n", #cond, __FILE__, __LINE__); ++failed; } } while (0)

static bool contains(const DynamicArray<int>& a, int x) {
    for (int i = 0; i < a.size(); ++i) if (a[i] == x) return true;
    return false;
}

int main() {
    std::printf("[GridGraph 테스트]\n");

    // 10 × 10 격자 (벽 없음), 칸 번호 = row × 10 + col, row 0 = 남쪽
    Grid g = makeTestGrid(10, 10);
    GridGraph graph(g);
    CHECK(graph.vertexCount() == 100);

    // 칸 중심 → 칸 번호 왕복
    for (int k = 0; k < 100; ++k) {
        double lat, lng;
        graph.cellCenter(k, lat, lng);
        CHECK(graph.cellAt(lat, lng) == k);
    }
    double lat, lng;
    graph.cellCenter(0, lat, lng);
    CHECK(std::fabs(lat - (g.minLat + 0.5 * g.cellLat)) < 1e-12);
    CHECK(std::fabs(lng - (g.minLng + 0.5 * g.cellLng)) < 1e-12);

    // 범위 밖: 격자 바로 아래·왼쪽(반 칸)과 위·오른쪽 끝 경계
    CHECK(graph.cellAt(g.minLat - 0.5 * g.cellLat, g.minLng + 0.5 * g.cellLng) == -1);
    CHECK(graph.cellAt(g.minLat + 0.5 * g.cellLat, g.minLng - 0.5 * g.cellLng) == -1);
    CHECK(graph.cellAt(g.minLat + 10.001 * g.cellLat, g.minLng + 0.5 * g.cellLng) == -1);
    CHECK(graph.cellAt(g.minLat + 0.5 * g.cellLat, g.minLng + 10.001 * g.cellLng) == -1);

    // 이웃: 모서리 3개, 가운데 8개
    DynamicArray<int> nb;
    graph.neighbors(0, nb);
    CHECK(nb.size() == 3 && contains(nb, 1) && contains(nb, 10) && contains(nb, 11));
    graph.neighbors(55, nb);
    CHECK(nb.size() == 8);   // 이전 결과가 남아 있지 않아야 함

    // 맨 왼쪽 열(3행 0열 = 30)에서 왼쪽으로 옆 줄(29, 39, 19)로 넘어가지 않아야 함
    graph.neighbors(30, nb);
    CHECK(nb.size() == 5 && !contains(nb, 29) && !contains(nb, 19) && !contains(nb, 39));
    // 맨 오른쪽 열(3행 9열 = 39)
    graph.neighbors(39, nb);
    CHECK(nb.size() == 5 && !contains(nb, 40) && !contains(nb, 30) && !contains(nb, 50));

    // 거리: 칸 한 변 약 100m, 대각선은 약 √2배
    double h = graph.length(0, 1), v = graph.length(0, 10), d = graph.length(0, 11);
    CHECK(h > 95 && h < 105);
    CHECK(v > 95 && v < 105);
    CHECK(std::fabs(d - std::sqrt(h * h + v * v)) < 1e-9);
    CHECK(graph.length(0, 0) == 0.0);

    // 가중치: alpha = 0 이면 거리와 같고, alpha가 크면 미세먼지가 높은 쪽이 더 비쌈
    WeightOption shortest; shortest.alpha = 0.0;
    WeightOption clean;    clean.alpha = 1.0;
    CHECK(std::fabs(graph.weight(0, 1, shortest) - h) < 1e-9);
    CHECK(graph.weight(0, 1, clean) > h);
    // makeTestGrid: 위·왼쪽이 미세먼지 높음 → 맨 위 줄 간선이 맨 아래 줄 간선보다 비쌈
    CHECK(graph.weight(90, 91, clean) > graph.weight(0, 1, clean));

    // 값이 없는 칸(음수)은 기준값 35로 봄 → 비용이 길이보다 작아지거나 음수가 되면 안 됨
    Grid missing = makeTestGrid(3, 3);
    for (int k = 0; k < 9; ++k) missing.pm25[k] = -1.0;
    GridGraph mg(missing);
    WeightOption strong; strong.alpha = 100.0;
    CHECK(std::fabs(mg.weight(0, 1, clean) - 2.0 * mg.length(0, 1)) < 1e-9);
    CHECK(mg.weight(0, 1, strong) > 0);

    // beta: 기온 15도(쾌적)이면 영향 없음, 멀어지면 비싸짐
    WeightOption hot; hot.alpha = 0.0; hot.beta = 1.0;
    CHECK(std::fabs(graph.weight(0, 1, hot) - h) < 1e-9);
    Grid warm = makeTestGrid(3, 3);
    for (int k = 0; k < 9; ++k) warm.temp[k] = 25.0;
    GridGraph wg(warm);
    CHECK(std::fabs(wg.weight(0, 1, hot) - 2.0 * wg.length(0, 1)) < 1e-9);

    // 벽이 있는 격자: 5열이 1~9행에서 막히고 0행(칸 5)만 열림
    Grid w = makeTestGrid(10, 10, true);
    GridGraph wall(w);
    wall.neighbors(54, nb);                  // 5행 4열: 오른쪽 벽
    CHECK(!contains(nb, 55) && !contains(nb, 45) && !contains(nb, 65));
    CHECK(nb.size() == 5);
    wall.neighbors(4, nb);                   // 0행 4열: 오른쪽(5)은 열린 칸
    CHECK(contains(nb, 5));
    wall.neighbors(14, nb);                  // 1행 4열 → 0행 5열 대각선: 옆 칸 15가 벽이라 모서리 뚫기 금지
    CHECK(!contains(nb, 5));
    wall.neighbors(16, nb);                  // 1행 6열 → 0행 5열 대각선도 마찬가지
    CHECK(!contains(nb, 5));
    wall.neighbors(5, nb);                   // 열린 칸 5에서는 4, 6으로 갈 수 있음
    CHECK(contains(nb, 4) && contains(nb, 6) && !contains(nb, 15));

    // passable이 비어 있으면(OSM 반영 전) 모든 칸 통행 가능
    Grid open = makeTestGrid(10, 10);
    open.passable.clear();
    GridGraph og(open);
    og.neighbors(55, nb);
    CHECK(nb.size() == 8);

    std::printf(failed == 0 ? "  모두 통과\n" : "  실패 %d개\n", failed);
    return failed == 0 ? 0 : 1;
}
