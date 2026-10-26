// 방지혁 담당 데이터 모듈 테스트 파일 ( HashTable, TimeAVL, DataLoader)//
#include <cstdio>
#include <cmath>
#include <fstream>
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
    // 격자 범위는 측정소 위치와 상관없이 서울 전체 (위도 37.41~37.72, 경도 126.73~127.27)
    CHECK(grid.minLat == 37.41 && grid.minLng == 126.73);
    CHECK(grid.minLat + grid.rows * grid.cellLat >= 37.72);
    CHECK(grid.minLng + grid.cols * grid.cellLng >= 127.27);

    // 4. 실제 CSV 형식 (YYYYMMDDHH, pm10/pm25 순서, 빈칸 = 결측값) 읽기
    {
        std::ofstream st("build/tests/test_stations.csv");
        st << "code,name,lat,lng,address,note\n"
           << "111123,종로구,37.571973,127.005082,서울 종로구,\n"
           << "111274,강동구,37.544963,127.135681,서울 강동구,\n";
        std::ofstream air("build/tests/test_air.csv");
        air << "datetime,station_code,station_name,pm10,pm25,o3,no2,co,so2\r\n"
            << "2025020100,111123,종로구,40,20,0.01,0.02,0.4,0.003\r\n"
            << "2025020101,111123,종로구,,30,0.01,0.02,0.4,0.003\r\n"     // pm10 빈칸
            << "2025020102,111123,종로구,60,,0.01,0.02,0.4,0.003\r\n"     // pm25 빈칸
            << "2025020100,111274,강동구,,,0.01,0.02,0.4,0.003\r\n"       // 둘 다 빈칸
            << "2025030100,111123,종로구,999,999,0.01,0.02,0.4,0.003\r\n" // 기간 밖
            << "2025020100,999999,없는구,10,10,0.01,0.02,0.4,0.003\r\n";  // 없는 측정소
    }
    DataLoader loader;
    CHECK(loader.loadStations("build/tests/test_stations.csv"));
    CHECK(loader.stations().size() == 2);
    CHECK(loader.loadDistrict("build/tests/test_air.csv"));
    DynamicArray<Station> avg;
    loader.averageStations({ 2025, 2, 1, 0 }, { 2025, 2, 28, 23 }, avg);
    CHECK(avg.size() == 2);
    CHECK(std::fabs(avg[0].pm25 - 25.0) < 1e-9);   // (20 + 30) / 2, 빈칸 제외
    CHECK(std::fabs(avg[0].pm10 - 50.0) < 1e-9);   // (40 + 60) / 2, 빈칸 제외
    CHECK(avg[1].pm25 == -1.0 && avg[1].pm10 == -1.0);   // 유효한 값이 없으면 -1

    std::printf(failed == 0 ? ">> 데이터 모듈 테스트 모두 통과!\n" : ">> 실패 %d개 발생\n", failed);
    return failed == 0 ? 0 : 1;
}