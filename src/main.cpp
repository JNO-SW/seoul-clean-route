// 담당: 이준호
// 통합. 콘솔 실행용 main과, 웹(WebAssembly)에서 부를 함수들을 둡니다.
#include <cstdio>
#include <cstdlib>
#include "common/Types.h"
#include "data/DataLoader.h"
#include "graph/GridGraph.h"
#include "route/Router.h"
#include "station/StationGraph.h"
#include "search/Trie.h"
#include "common/Export.h"

// 웹에서 부를 함수 (WebAssembly로 내보냄). 결과는 JSON 문자열로 돌려줍니다.
// TODO(이준호): 각 모듈이 완성되면 연결
extern "C" {
    // 기간 선택: "2025-02" 같은 형식
    const char* api_set_period(const char* period) { (void)period; return "{}"; }

    // 출발·도착 위경도와 가중치로 최적 경로 + 최단 경로
    const char* api_find_route(double sLat, double sLng, double gLat, double gLng, double alpha) {
        (void)sLat; (void)sLng; (void)gLat; (void)gLng; (void)alpha;
        return "{}";
    }

    // 지도 색칠용 칸별 오염도
    const char* api_get_grid() { return "{}"; }

    // Top/Worst k 지역
    const char* api_get_ranking(int k) { (void)k; return "{}"; }

    // 지역명 자동완성
    const char* api_search(const char* prefix) { (void)prefix; return "[]"; }
}

// ───────────────────────── 중간발표 데모 ─────────────────────────
// 실행: ./build/app [출발지] [목적지] [alpha]     예) ./build/app 종로구 강남구 1
//       (레포 맨 위 폴더에서 실행. 이름은 앞 글자만 써도 됨: ./build/app 마포 송파)
// 결과: web/data/grid.geojson, routes.geojson, ranking.json → web/index.html 에서 지도로 확인

static const char* STATIONS_CSV = "data/processed/stations.csv";
static const char* AIR_CSV      = "data/processed/air_2025_02.csv";
static const double CELL_METERS = 100.0;   // 중간발표: 100m 격자
static const int    GRID_STEP   = 5;       // 지도 표시용으로 5×5칸(500m)씩 묶어서 저장

// 지역명(앞 글자) → 위치. 못 찾으면 false
static bool findPlace(const Trie& trie, const char* name, Place& out) {
    DynamicArray<Place> hits;
    trie.searchPrefix(name, 5, hits);
    if (hits.empty()) {
        std::printf("  '%s' 에 해당하는 지역이 없습니다.\n", name);
        return false;
    }
    out = hits[0];
    return true;
}

static void printRoute(const char* title, const Route& r) {
    std::printf("  %s: 거리 %.0f m, 약 %.0f분, 미세먼지 노출량 %.1f, 칸 %d개\n",
                title, r.distance, r.minutes, r.exposure, r.lats.size());
}

static void printRanking(const char* title, const DynamicArray<RankItem>& list) {
    std::printf("  %s:", title);
    for (int i = 0; i < list.size(); ++i) std::printf(" %d.%s(%.1f)", i + 1, list[i].name.c_str(), list[i].score);
    std::printf("\n");
}

int main(int argc, char** argv) {
    const char* startName = argc > 1 ? argv[1] : "종로구";
    const char* goalName  = argc > 2 ? argv[2] : "강남구";
    double alpha          = argc > 3 ? std::atof(argv[3]) : 1.0;   // 미세먼지 반영 비중

    // 1. 데이터 읽기 (방지혁)
    std::printf("[1] 데이터 읽기\n");
    DataLoader loader;
    if (!loader.loadStations(STATIONS_CSV)) { std::printf("  측정소 파일 읽기 실패: %s\n", STATIONS_CSV); return 1; }
    if (!loader.loadDistrict(AIR_CSV))      { std::printf("  대기 데이터 읽기 실패: %s\n", AIR_CSV); return 1; }
    std::printf("  측정소 %d곳\n", loader.stations().size());

    // 2. 기간 평균 (AVL 범위 검색)
    DateTime from; from.year = 2025; from.month = 2; from.day = 1;  from.hour = 0;
    DateTime to;   to.year   = 2025; to.month   = 2; to.day   = 28; to.hour   = 23;
    DynamicArray<Station> stations;
    loader.averageStations(from, to, stations);
    if (stations.empty()) { std::printf("  기간 평균 결과가 비어 있습니다.\n"); return 1; }

    // 3. 지역명 검색 (트라이)
    std::printf("[2] 지역명 검색\n");
    Trie trie;
    for (int i = 0; i < stations.size(); ++i) {
        Place p;
        p.name = stations[i].name;
        p.lat = stations[i].lat;
        p.lng = stations[i].lng;
        trie.insert(p);
    }
    Place startPlace, goalPlace;
    if (!findPlace(trie, startName, startPlace) || !findPlace(trie, goalName, goalPlace)) return 1;
    std::printf("  출발 %s → 도착 %s\n", startPlace.name.c_str(), goalPlace.name.c_str());

    // 4. 격자 생성 + IDW 보간 (방지혁)
    std::printf("[3] 격자 생성 (%.0fm)\n", CELL_METERS);
    Grid grid;
    buildGrid(stations, CELL_METERS, grid);
    if (grid.rows <= 0 || grid.cols <= 0) { std::printf("  격자가 비어 있습니다.\n"); return 1; }
    std::printf("  %d × %d = %d칸\n", grid.rows, grid.cols, grid.rows * grid.cols);

    // 5. 격자 그래프 (김재겸) + 경로 탐색 (조효근)
    std::printf("[4] 경로 탐색\n");
    GridGraph graph(grid);
    int start = graph.cellAt(startPlace.lat, startPlace.lng);
    int goal  = graph.cellAt(goalPlace.lat, goalPlace.lng);
    if (start < 0 || goal < 0) { std::printf("  출발지나 목적지가 격자 밖입니다.\n"); return 1; }
    start = nearestPassable(graph, start);
    goal  = nearestPassable(graph, goal);
    if (start < 0 || goal < 0) { std::printf("  통행 가능한 칸을 찾지 못했습니다.\n"); return 1; }

    WeightOption opt;
    opt.alpha = alpha;
    Route best, shortest;
    if (!compareRoutes(graph, start, goal, opt, best, shortest)) { std::printf("  경로를 찾지 못했습니다.\n"); return 1; }
    printRoute("최적 경로", best);
    printRoute("최단 경로", shortest);
    if (shortest.exposure > 0) {
        double cut = (shortest.exposure - best.exposure) / shortest.exposure * 100.0;
        double more = shortest.distance > 0 ? (best.distance - shortest.distance) / shortest.distance * 100.0 : 0.0;
        std::printf("  → 노출량 %.1f%% 감소, 거리 %.1f%% 증가\n", cut, more);
    }

    // 6. Top / Worst 지역 (장데니스)
    std::printf("[5] 지역 순위\n");
    StationGraph sg(stations, 3);
    DynamicArray<RankItem> top, worst;
    sg.topK(5, top);
    sg.worstK(5, worst);
    printRanking("Top 5  ", top);
    printRanking("Worst 5", worst);

    // 7. 웹 지도용 파일 저장
    std::printf("[6] 웹 지도 파일 저장\n");
    bool ok = writeGridGeoJSON(grid, "web/data/grid.geojson", GRID_STEP)
           && writeRoutesGeoJSON(best, shortest, "web/data/routes.geojson")
           && writeRankingJSON(top, worst, "web/data/ranking.json");
    std::printf(ok ? "  web/data 에 저장 완료\n" : "  저장 실패 (web/data 폴더가 있는지 확인)\n");
    return ok ? 0 : 1;
}
