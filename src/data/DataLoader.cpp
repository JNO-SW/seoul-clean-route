// 담당: 방지혁
#include "data/DataLoader.h"
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

    DynamicArray<std::string> tokens;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        parseCSVLine(line, tokens);
        if (tokens.size() < 5) continue;
        if (tokens[0].length() < 10) continue;      // 시각 형식이 아니면 건너뛰기

        // 관측소는 이름(station_name)으로 찾기
        const std::string& name = tokens[2];
        int stationIndex = -1;
        for (int i = 0; i < static_cast<int>(stations_.size()); ++i) {
            if (stations_[i].name == name) {
                stationIndex = i;
                break;
            }
        }
        if (stationIndex == -1) continue;

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

    double minLat = stations[0].lat, maxLat = stations[0].lat;
    double minLng = stations[0].lng, maxLng = stations[0].lng;

    for (int i = 1; i < static_cast<int>(stations.size()); ++i) {
        if (stations[i].lat < minLat) minLat = stations[i].lat;
        if (stations[i].lat > maxLat) maxLat = stations[i].lat;
        if (stations[i].lng < minLng) minLng = stations[i].lng;
        if (stations[i].lng > maxLng) maxLng = stations[i].lng;
    }

    // 위도 1도 약 111,000m 기준 간격 계산
    double latDegreeStep = cellMeters / 111000.0;
    double lngDegreeStep = cellMeters / (111000.0 * std::cos(minLat * 3.14159265358979323846 / 180.0));

    out.rows = static_cast<int>((maxLat - minLat) / latDegreeStep) + 1;
    out.cols = static_cast<int>((maxLng - minLng) / lngDegreeStep) + 1;
    out.minLat = minLat;
    out.minLng = minLng;
    out.cellLat = latDegreeStep;
    out.cellLng = lngDegreeStep;

    out.pm25.resize(out.rows * out.cols, 0.0);

    // IDW (역거리 가중 보간법)
    for (int r = 0; r < out.rows; ++r) {
        double cellLatPos = minLat + r * latDegreeStep;
        for (int c = 0; c < out.cols; ++c) {
            double cellLngPos = minLng + c * lngDegreeStep;

            double weightSum = 0.0;
            double valueSum = 0.0;
            bool foundExact = false;

            for (int i = 0; i < static_cast<int>(stations.size()); ++i) {
                if (stations[i].pm25 < 0) continue;

                double dLat = cellLatPos - stations[i].lat;
                double dLng = cellLngPos - stations[i].lng;
                double distSq = dLat * dLat + dLng * dLng;

                if (distSq < 1e-12) {
                    out.pm25[r * out.cols + c] = stations[i].pm25;
                    foundExact = true;
                    break;
                }

                double w = 1.0 / distSq;
                weightSum += w;
                valueSum += w * stations[i].pm25;
            }

            if (!foundExact) {
                out.pm25[r * out.cols + c] = (weightSum > 0) ? (valueSum / weightSum) : 0.0;
            }
        }
    }
}