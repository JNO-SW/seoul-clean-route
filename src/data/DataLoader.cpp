// 담당: 방지혁
#include "data/DataLoader.h"
#include "data/HashTable.h"
#include <fstream>
#include <sstream>
#include <cmath>

// CSV 한 줄 파싱 보조 함수
static void parseCSVLine(const std::string& line, DynamicArray<std::string>& tokens) {
    tokens.clear();
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t start = item.find_first_not_of(" \t\r\n");
        size_t end = item.find_last_not_of(" \t\r\n");
        if (start != std::string::npos && end != std::string::npos) {
            tokens.push_back(item.substr(start, end - start + 1));
        }
        else {
            tokens.push_back("");
        }
    }
}

// --------------------------------------------------------
// 1. loadStations (stations.csv 읽기)
// --------------------------------------------------------
bool DataLoader::loadStations(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line)) return false;

    DynamicArray<std::string> tokens;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        parseCSVLine(line, tokens);
        if (tokens.size() < 4) continue;

        Station st;
        st.name = tokens[1]; // 관측소 이름 (예: "종로구", "OA-2221")
        st.lat = std::stod(tokens[2]);
        st.lng = std::stod(tokens[3]); // lng 적용
        st.type = StationType::DISTRICT;

        stations_.push_back(st);
    }
    return true;
}

// 빈칸(결측값)이면 -1.0, 아니면 숫자로 변환
static double parseValue(const std::string& token) {
    if (token.empty()) return -1.0;
    return std::stod(token);
}

// --------------------------------------------------------
// 2. loadDistrict (대기 데이터 파싱 및 AVL 트리 삽입)
// 열 순서: datetime,station_code,station_name,pm10,pm25,o3,no2,co,so2
// 시각: YYYYMMDDHH (예: 2025020112)
// --------------------------------------------------------
bool DataLoader::loadDistrict(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line)) return false;   // 머리글 건너뛰기

    // 관측소 이름 → 관측소 목록에서의 위치 (해시 테이블, 체이닝)
    HashTable<std::string, int> stationIndexByName(64);
    for (int i = 0; i < static_cast<int>(stations_.size()); ++i) {
        stationIndexByName.put(stations_[i].name, i);
    }

    DynamicArray<std::string> tokens;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        parseCSVLine(line, tokens);
        if (tokens.size() < 5) continue;
        if (tokens[0].length() < 10) continue;      // 시각 형식이 아니면 건너뛰기

        // 관측소는 이름(station_name)으로 해시 테이블에서 찾기, O(1) 평균
        int* found = stationIndexByName.get(tokens[2]);
        if (found == nullptr) continue;
        int stationIndex = *found;

        Record r;
        r.stationIndex = stationIndex;
        r.time.year = std::stoi(tokens[0].substr(0, 4));
        r.time.month = std::stoi(tokens[0].substr(4, 2));
        r.time.day = std::stoi(tokens[0].substr(6, 2));
        r.time.hour = std::stoi(tokens[0].substr(8, 2));
        r.pm10 = parseValue(tokens[3]);
        r.pm25 = parseValue(tokens[4]);

        records_.insert(r);
    }
    return true;
}

bool DataLoader::loadSimple(const std::string& path) {
    return loadDistrict(path);
}

bool DataLoader::loadSdot(const std::string& path) {
    return loadDistrict(path);
}

// --------------------------------------------------------
// 3. averageStations (기간 평균 산출)
// --------------------------------------------------------
void DataLoader::averageStations(const DateTime& from, const DateTime& to, DynamicArray<Station>& out) const {
    out.clear();
    DynamicArray<Record> matched;
    records_.rangeSearch(from, to, matched);

    for (int i = 0; i < static_cast<int>(stations_.size()); ++i) {
        Station st = stations_[i];
        double sumPm25 = 0.0, sumPm10 = 0.0;
        int countPm25 = 0, countPm10 = 0;   // 결측값(음수)을 빼고 각각 따로 셈

        for (int j = 0; j < static_cast<int>(matched.size()); ++j) {
            if (matched[j].stationIndex != i) continue;
            if (matched[j].pm25 >= 0) { sumPm25 += matched[j].pm25; ++countPm25; }
            if (matched[j].pm10 >= 0) { sumPm10 += matched[j].pm10; ++countPm10; }
        }

        st.pm25 = (countPm25 > 0) ? sumPm25 / countPm25 : -1.0;
        st.pm10 = (countPm10 > 0) ? sumPm10 / countPm10 : -1.0;
        out.push_back(st);
    }
}

// --------------------------------------------------------
// 4. buildGrid (전역 함수 - Types.h의 Grid 구조체 호환)
// --------------------------------------------------------
void buildGrid(const DynamicArray<Station>& stations, double cellMeters, Grid& out) {
    if (stations.empty()) return;

    // 격자 범위: 측정소 위치가 아니라 서울 전체로 고정 (외곽 지역도 경로 탐색 가능하도록)
    const double minLat = 37.41, maxLat = 37.72;
    const double minLng = 126.73, maxLng = 127.27;

    // 위도 1도 ≈ 111,000m, 경도 1도 ≈ 111,000m × cos(위도) → 칸 한 변이 cellMeters가 되도록 간격 계산
    const double PI = 3.14159265358979323846;
    double metersPerLat = 111000.0;
    double metersPerLng = 111000.0 * std::cos(minLat * PI / 180.0);
    double latDegreeStep = cellMeters / metersPerLat;
    double lngDegreeStep = cellMeters / metersPerLng;

    out.rows = static_cast<int>((maxLat - minLat) / latDegreeStep) + 1;
    out.cols = static_cast<int>((maxLng - minLng) / lngDegreeStep) + 1;
    out.minLat = minLat;
    out.minLng = minLng;
    out.cellLat = latDegreeStep;
    out.cellLng = lngDegreeStep;

    int cellCount = out.rows * out.cols;
    out.pm25.clear();
    out.temp.clear();
    out.passable.clear();
    out.pm25.resize(cellCount, -1.0);
    out.temp.resize(cellCount, -999.0);   // 기온 데이터는 아직 없음 (값 없음 = -999)
    out.passable.resize(cellCount, true); // 중간발표: 모든 칸 통행 가능 (OSM 반영은 최종)

    // IDW (역거리 가중 보간법): Σ(측정값 / 거리²) / Σ(1 / 거리²)
    for (int r = 0; r < out.rows; ++r) {
        double cellLatPos = minLat + (r + 0.5) * latDegreeStep;   // 칸 중심
        for (int c = 0; c < out.cols; ++c) {
            double cellLngPos = minLng + (c + 0.5) * lngDegreeStep;

            double weightSum = 0.0;
            double valueSum = 0.0;
            bool foundExact = false;

            for (int i = 0; i < static_cast<int>(stations.size()); ++i) {
                if (stations[i].pm25 < 0) continue;

                // 위경도 차이를 미터로 바꿔서 거리 계산 (경도 1도가 위도 1도보다 짧음)
                double dy = (cellLatPos - stations[i].lat) * metersPerLat;
                double dx = (cellLngPos - stations[i].lng) * metersPerLng;
                double distSq = dx * dx + dy * dy;

                if (distSq < 1.0) {   // 1m 이내면 그 측정소 값 그대로
                    out.pm25[r * out.cols + c] = stations[i].pm25;
                    foundExact = true;
                    break;
                }

                double w = 1.0 / distSq;
                weightSum += w;
                valueSum += w * stations[i].pm25;
            }

            if (!foundExact && weightSum > 0) {
                out.pm25[r * out.cols + c] = valueSum / weightSum;
            }
        }
    }
}
