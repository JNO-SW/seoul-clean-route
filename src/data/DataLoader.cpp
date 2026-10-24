// 담당: 방지혁
#include "data/DataLoader.h"

bool DataLoader::loadStations(const std::string& path) {
    // TODO: code,name,lat,lng,address,note 형식. 첫 줄(머리글)은 건너뛰기
    (void)path;
    return false;
}

bool DataLoader::loadDistrict(const std::string& path) {
    // TODO
    (void)path;
    return false;
}

bool DataLoader::loadSimple(const std::string& path) {
    // TODO: 주소 → 위경도 변환이 필요할 수 있음
    (void)path;
    return false;
}

bool DataLoader::loadSdot(const std::string& path) {
    // TODO
    (void)path;
    return false;
}

void DataLoader::averageStations(const DateTime& from, const DateTime& to, DynamicArray<Station>& out) const {
    // TODO: records_.rangeSearch → 관측소별 평균 (결측값 제외)
    (void)from; (void)to; (void)out;
}

void buildGrid(const DynamicArray<Station>& stations, double cellMeters, Grid& out) {
    // TODO: 서울 범위를 cellMeters 간격으로 나누고, 칸마다 주변 관측소 값으로 IDW 보간
    (void)stations; (void)cellMeters; (void)out;
}
