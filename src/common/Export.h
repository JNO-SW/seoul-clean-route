// 담당: 이준호
// 계산 결과를 웹 지도에서 읽을 수 있는 GeoJSON / JSON 파일로 저장합니다. (중간발표 데모용)
#pragma once
#include <string>
#include "common/Types.h"
#include "station/StationGraph.h"

// 칸별 미세먼지를 사각형(Polygon)으로 저장. step: step×step 칸을 하나로 묶어 저장 (파일 크기 조절)
// 묶은 칸 중 통행 가능하고 값이 있는 칸의 평균을 씀. 그런 칸이 없으면 저장하지 않음
bool writeGridGeoJSON(const Grid& grid, const std::string& path, int step = 1);

// 최적 경로와 최단 경로를 선(LineString)으로 저장. 거리·노출량·소요 시간도 속성으로 함께 저장
bool writeRoutesGeoJSON(const Route& best, const Route& shortest, const std::string& path);

// Top / Worst 지역 순위를 JSON으로 저장 {"top":[{"name":..,"score":..}], "worst":[...]}
bool writeRankingJSON(const DynamicArray<RankItem>& top, const DynamicArray<RankItem>& worst,
                      const std::string& path);
