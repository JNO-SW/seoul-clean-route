// 담당: 이준호
// 테스트용 가짜 데이터. 다른 사람 모듈이 완성되기 전에도 자기 코드를 시험할 수 있게 합니다.
// 실제 측정값이 아니므로 테스트에만 사용하세요.
#pragma once
#include <string>
#include "common/Types.h"

// rows × cols 가짜 격자를 만듭니다.
// - 왼쪽 아래 모서리: 서울시청 근처, 칸 크기 약 100m
// - 미세먼지: 왼쪽 위가 높고 오른쪽 아래가 낮게 (10 ~ 60 사이)
// - 기온: 모두 15도
// - wall = true 이면 가운데 세로줄을 통행 불가로 막고, 맨 아래 한 칸만 열어둡니다 (길찾기 우회 테스트용)
inline Grid makeTestGrid(int rows, int cols, bool wall = false) {
    Grid g;
    g.rows = rows;
    g.cols = cols;
    g.minLat = 37.5600;
    g.minLng = 126.9700;
    g.cellLat = 0.0009;    // 약 100m
    g.cellLng = 0.00114;   // 서울 위도에서 약 100m
    int n = rows * cols;
    g.pm25.resize(n, 0.0);
    g.temp.resize(n, 15.0);
    g.passable.resize(n, true);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double a = (rows > 1) ? (double)r / (rows - 1) : 0.0;   // 위로 갈수록 1
            double b = (cols > 1) ? (double)c / (cols - 1) : 0.0;   // 오른쪽으로 갈수록 1
            g.pm25[r * cols + c] = 10.0 + 50.0 * (0.5 * a + 0.5 * (1.0 - b));
        }
    }
    if (wall && cols >= 3) {
        int mid = cols / 2;
        for (int r = 1; r < rows; ++r) g.passable[r * cols + mid] = false;  // 0행만 열림
    }
    return g;
}

// 가짜 관측소 목록 (자치구 이름 25개, 위치·값은 임의)
// 위치는 서울 범위 안에 5 × 5 격자 모양으로 퍼뜨린 가짜 좌표입니다.
inline DynamicArray<Station> makeTestStations() {
    const char* names[25] = {
        "종로구", "중구", "용산구", "성동구", "광진구",
        "동대문구", "중랑구", "성북구", "강북구", "도봉구",
        "노원구", "은평구", "서대문구", "마포구", "양천구",
        "강서구", "구로구", "금천구", "영등포구", "동작구",
        "관악구", "서초구", "강남구", "송파구", "강동구"};
    DynamicArray<Station> list;
    for (int i = 0; i < 25; ++i) {
        Station s;
        s.name = names[i];
        s.type = StationType::DISTRICT;
        s.lat = 37.45 + 0.05 * (i / 5);
        s.lng = 126.85 + 0.07 * (i % 5);
        s.pm25 = 12.0 + (i * 37 % 41);      // 12 ~ 52 사이, 서로 다르게
        s.pm10 = s.pm25 * 1.6;
        s.temp = 5.0 + (i % 7);
        s.humidity = 40.0 + (i % 5) * 5;
        s.windDir = (i * 45) % 360;
        s.windSpeed = 1.0 + (i % 4) * 0.5;
        list.push_back(s);
    }
    return list;
}
