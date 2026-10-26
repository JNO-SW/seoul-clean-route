// 방지혁 담당 데이터 모듈 테스트 파일 ( HashTable, TimeAVL, DataLoader)//
#include <cstdio>
#include "data/HashTable.h"
#include "data/TimeAVL.h"
#include "data/DataLoader.h"
#include "common/TestData.h"

static int failed = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf(" 실패: %s (%s:%d)\n", #cond, __FILE__, __LINE__); ++failed; } } while (0)

int main() {
    std::printf("[Data Module 테스트 시작]\n");

    // 1. HashTable 테스트
    HashTable<std::string, int> ht;
    ht.put("OA-2221", 10);
    ht.put("OA-1200", 20);

    CHECK(ht.size() == 2);
    int* val1 = ht.get("OA-2221");
    CHECK(val1 != nullptr && *val1 == 10);
    CHECK(ht.get("NONE") == nullptr);

    // 2. TimeAVL 테스트
    TimeAVL avl;
    Record r1;
    r1.time = { 2025, 2, 1, 12 };
    r1.stationIndex = 0;
    r1.pm25 = 15.5;
    avl.insert(r1);

    CHECK(avl.size() == 1);
    CHECK(avl.height() >= 1);

    DynamicArray<Record> searchOut;
    avl.rangeSearch({ 2025, 2, 1, 0 }, { 2025, 2, 1, 23 }, searchOut);
    CHECK(searchOut.size() == 1);
    CHECK(searchOut[0].pm25 == 15.5);

    // 3. DataLoader & buildGrid 테스트 (TestData 활용)
    DynamicArray<Station> fakeStations = makeTestStations();
    CHECK(fakeStations.size() == 25);

    Grid grid;
    buildGrid(fakeStations, 100.0, grid);
    CHECK(grid.rows > 0);
    CHECK(grid.cols > 0);
    CHECK(grid.pm25.size() == grid.rows * grid.cols);

    std::printf(failed == 0 ? ">> 데이터 모듈 테스트 모두 통과!\n" : ">> 실패 %d개 발생\n", failed);
    return failed == 0 ? 0 : 1;
}