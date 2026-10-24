// 담당: 방지혁
// CSV 읽기, 결측·이상값 처리, 기간 평균, 격자 보간
#pragma once
#include "common/Types.h"
#include "data/TimeAVL.h"

class DataLoader {
public:
    // 측정소 위치 파일(stations.csv)을 읽어 관측소 목록(stations_)을 채움. 가장 먼저 호출
    bool loadStations(const std::string& path);

    // 대기 데이터 파일(air_2025_MM.csv)을 읽어 AVL 트리에 기록을 넣음. 성공하면 true
    // station_code로 관측소 목록에서 위치(stationIndex)를 찾아 Record에 저장
    bool loadDistrict(const std::string& path);   // 자치구 측정소 (OA-2221)
    bool loadSimple(const std::string& path);     // 간이측정망
    bool loadSdot(const std::string& path);       // S-DoT

    // 기간 평균을 낸 관측소 목록을 out에 채움
    void averageStations(const DateTime& from, const DateTime& to, DynamicArray<Station>& out) const;

    const DynamicArray<Station>& stations() const { return stations_; }

private:
    DynamicArray<Station> stations_;   // 관측소 기본 정보 (이름, 위경도, 종류)
    TimeAVL records_;                  // 시간별 측정 기록 (AVL 트리)
};

// 서울 격자 생성 + 칸별 환경값 역거리 가중 보간(IDW)
// cellMeters: 격자 간격 (중간발표: 100m로 시작, 이후 10 / 25 / 50m 비교)
void buildGrid(const DynamicArray<Station>& stations, double cellMeters, Grid& out);
