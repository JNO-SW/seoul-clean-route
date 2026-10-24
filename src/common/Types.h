// 담당: 이준호
// 모듈 사이에서 주고받는 공통 데이터 형식입니다.
// 필드를 바꾸기 전에 팀에 먼저 공유해 주세요.
#pragma once
#include <string>
#include "graph/DynamicArray.h"

// 날짜와 시각. AVL 트리 키로 쓰기 위해 하나의 정수로 바꿀 수 있습니다. (예: 2025021509 = 2025년 2월 15일 9시)
struct DateTime {
    int year = 0, month = 0, day = 0, hour = 0;
    long long key() const { return ((year * 100LL + month) * 100 + day) * 100 + hour; }
};

// 관측소 종류
enum class StationType { DISTRICT, SIMPLE, SDOT };  // 자치구 측정소, 간이측정망, S-DoT

// 관측소 한 곳의 (기간 평균) 값
struct Station {
    std::string name;
    StationType type = StationType::DISTRICT;
    double lat = 0.0, lng = 0.0;   // 위도, 경도
    double pm25 = -1.0;            // 초미세먼지 (값이 없으면 음수)
    double pm10 = -1.0;            // 미세먼지
    double temp = -999.0;          // 기온
    double humidity = -1.0;        // 습도
    double windDir = -1.0;         // 풍향 (도)
    double windSpeed = -1.0;       // 풍속 (m/s)
};

// 서울 격자. 칸 번호 = row * cols + col
struct Grid {
    int rows = 0, cols = 0;
    double minLat = 0.0, minLng = 0.0;   // 왼쪽 아래 모서리
    double cellLat = 0.0, cellLng = 0.0; // 칸 한 변의 위도·경도 크기
    DynamicArray<double> pm25;           // 칸별 미세먼지
    DynamicArray<double> temp;           // 칸별 기온
    DynamicArray<bool> passable;         // 통행 가능 여부 (강 등은 false)
};

// 경로 탐색 결과
struct Route {
    DynamicArray<double> lats, lngs;  // 출발 → 도착 순서의 좌표
    double distance = 0.0;            // 총 거리 (m)
    double exposure = 0.0;            // 미세먼지 노출량 (거리 × 농도 합)
    double minutes = 0.0;             // 예상 소요 시간 (분)
};

// 경로 가중치 설정
struct WeightOption {
    double alpha = 1.0;   // 미세먼지 반영 비중 (0이면 거리만 = 최단 경로)
    double beta = 0.0;    // 기온 반영 비중 (확장)
};
