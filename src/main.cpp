// 담당: 이준호
// 통합. 콘솔 실행용 main과, 웹(WebAssembly)에서 부를 함수들을 둡니다.
#include <cstdio>
#include "common/Types.h"
#include "data/DataLoader.h"
#include "graph/GridGraph.h"
#include "route/Router.h"
#include "station/StationGraph.h"
#include "search/Trie.h"

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

int main() {
    std::printf("Seoul Clean Route - skeleton build OK\n");
    return 0;
}
