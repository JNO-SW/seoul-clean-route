// 담당: 이준호
// 계산 결과를 웹 지도에서 읽을 수 있는 GeoJSON 파일로 저장합니다. (중간발표 데모용)
#pragma once
#include <string>
#include "common/Types.h"

// 칸별 미세먼지를 사각형(Polygon)으로 저장. step: 몇 칸마다 하나씩 저장할지 (파일 크기 조절)
bool writeGridGeoJSON(const Grid& grid, const std::string& path, int step = 1);

// 최적 경로와 최단 경로를 선(LineString)으로 저장. 거리·노출량도 속성으로 함께 저장
bool writeRoutesGeoJSON(const Route& best, const Route& shortest, const std::string& path);
