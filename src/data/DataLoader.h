// 담당: 방지혁
// CSV 읽기, 결측·이상값 처리, 기간 평균, 격자 보간
#pragma once
#include "common/Types.h"
#include "data/TimeBST.h"

class DataLoader {
public:
    // CSV 파일들을 읽어 관측소 목록과 BST를 채움. 성공하면 true
    bool loadDistrict(const std::string& path);   // 자치구 측정소 (OA-2221)
    bool loadSimple(const std::string& path);     // 간이측정망
    bool loadSdot(const std::string& path);       // S-DoT

    // 기간 평균을 낸 관측소 목록을 out에 채움
    void averageStations(const DateTime& from, const DateTime& to, DynamicArray<Station>& out) const;

    const DynamicArray<Station>& stations() const { return stations_; }

private:
    DynamicArray<Station> stations_;   // 관측소 기본 정보 (이름, 위경도, 종류)
    TimeBST records_;                  // 시간별 측정 기록
};

// 서울 격자 생성 + 칸별 환경값 역거리 가중 보간(IDW)
// cellMeters: 격자 간격 (10 / 25 / 50m 비교 예정)
void buildGrid(const DynamicArray<Station>& stations, double cellMeters, Grid& out);
