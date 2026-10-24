// 트라이, GeoJSON 저장 테스트 (make test 로 실행)
#include <cstdio>
#include <fstream>
#include <string>
#include "search/Trie.h"
#include "common/Export.h"
#include "common/TestData.h"

static int failed = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf("  실패: %s (%s:%d)\n", #cond, __FILE__, __LINE__); ++failed; } } while (0)

static std::string readAll(const char* path) {
    std::ifstream f(path);
    std::string s, line;
    while (std::getline(f, line)) s += line;
    return s;
}

static int countOf(const std::string& s, const std::string& word) {
    int n = 0;
    for (size_t p = s.find(word); p != std::string::npos; p = s.find(word, p + 1)) ++n;
    return n;
}

int main() {
    std::printf("[Trie 테스트]\n");
    Trie trie;
    DynamicArray<Station> st = makeTestStations();
    for (int i = 0; i < st.size(); ++i) {
        Place p; p.name = st[i].name; p.lat = st[i].lat; p.lng = st[i].lng;
        trie.insert(p);
    }
    DynamicArray<Place> out;
    trie.searchPrefix("강", 10, out);           // 강북구, 강서구, 강남구, 강동구
    CHECK(out.size() == 4);
    trie.searchPrefix("강남", 10, out);
    CHECK(out.size() == 1 && out[0].name == "강남구");
    trie.searchPrefix("동", 10, out);           // 동대문구, 동작구
    CHECK(out.size() == 2);
    trie.searchPrefix("중", 10, out);           // 중구, 중랑구
    CHECK(out.size() == 2);
    trie.searchPrefix("서울", 10, out);
    CHECK(out.empty());
    trie.searchPrefix("", 3, out);              // 빈 입력이면 limit개까지
    CHECK(out.size() == 3);
    Place again; again.name = "강남구"; again.lat = 1; again.lng = 2;
    trie.insert(again);                         // 같은 이름은 덮어쓰기
    trie.searchPrefix("강남구", 10, out);
    CHECK(out.size() == 1 && out[0].lat == 1);

    std::printf("[Export 테스트]\n");
    Grid g = makeTestGrid(10, 10, true);
    CHECK(writeGridGeoJSON(g, "build/tests/grid.geojson", 1));
    std::string grid = readAll("build/tests/grid.geojson");
    CHECK(countOf(grid, "\"Polygon\"") == 91);   // 100칸 - 벽 9칸
    CHECK(writeGridGeoJSON(g, "build/tests/grid5.geojson", 5));
    CHECK(countOf(readAll("build/tests/grid5.geojson"), "\"Polygon\"") == 4);

    Route best, shortest;
    best.lats.push_back(37.56); best.lngs.push_back(126.97);
    best.lats.push_back(37.57); best.lngs.push_back(126.98);
    best.distance = 1500; best.exposure = 30000; best.minutes = 20;
    shortest = best;
    CHECK(writeRoutesGeoJSON(best, shortest, "build/tests/routes.geojson"));
    std::string routes = readAll("build/tests/routes.geojson");
    CHECK(countOf(routes, "\"LineString\"") == 2);
    CHECK(countOf(routes, "[126.970000,37.560000]") == 2);   // [경도, 위도] 순서

    DynamicArray<RankItem> top, worst;
    RankItem a; a.name = "강남구"; a.score = 12.5;
    top.push_back(a);
    CHECK(writeRankingJSON(top, worst, "build/tests/ranking.json"));
    CHECK(readAll("build/tests/ranking.json") == "{\"top\":[{\"name\":\"강남구\",\"score\":12.5}],\"worst\":[]}");

    std::printf(failed == 0 ? "  모두 통과\n" : "  실패 %d개\n", failed);
    return failed == 0 ? 0 : 1;
}
