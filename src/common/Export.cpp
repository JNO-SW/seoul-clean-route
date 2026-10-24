// 담당: 이준호
#include "common/Export.h"
#include <fstream>
#include <cstdio>

// 소수점 자릿수를 고정해서 숫자를 문자열로 (6자리 ≈ 0.1m)
static std::string num(double v, int digits = 6) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*f", digits, v);
    return buf;
}

// JSON 문자열 안에 넣을 수 있게 따옴표·역슬래시 처리
static std::string quote(const std::string& s) {
    std::string r = "\"";
    for (int i = 0; i < (int)s.size(); ++i) {
        char c = s[i];
        if (c == '"' || c == '\\') r += '\\';
        r += c;
    }
    return r + "\"";
}

bool writeGridGeoJSON(const Grid& grid, const std::string& path, int step) {
    if (step < 1) step = 1;
    if (grid.rows <= 0 || grid.cols <= 0 || grid.pm25.size() < grid.rows * grid.cols) return false;
    std::ofstream f(path);
    if (!f) return false;

    f << "{\"type\":\"FeatureCollection\",\"features\":[\n";
    bool first = true;
    for (int r = 0; r < grid.rows; r += step) {
        for (int c = 0; c < grid.cols; c += step) {
            double sum = 0.0;
            int cnt = 0;
            for (int dr = 0; dr < step && r + dr < grid.rows; ++dr) {
                for (int dc = 0; dc < step && c + dc < grid.cols; ++dc) {
                    int k = (r + dr) * grid.cols + (c + dc);
                    bool ok = grid.passable.size() <= k || grid.passable[k];
                    if (ok && grid.pm25[k] >= 0) { sum += grid.pm25[k]; ++cnt; }
                }
            }
            if (cnt == 0) continue;

            int r2 = (r + step < grid.rows) ? r + step : grid.rows;
            int c2 = (c + step < grid.cols) ? c + step : grid.cols;
            double lat1 = grid.minLat + r * grid.cellLat, lat2 = grid.minLat + r2 * grid.cellLat;
            double lng1 = grid.minLng + c * grid.cellLng, lng2 = grid.minLng + c2 * grid.cellLng;

            if (!first) f << ",\n";
            first = false;
            // GeoJSON 좌표 순서는 [경도, 위도]
            f << "{\"type\":\"Feature\",\"properties\":{\"pm25\":" << num(sum / cnt, 1) << "},"
              << "\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[["
              << "[" << num(lng1) << "," << num(lat1) << "],"
              << "[" << num(lng2) << "," << num(lat1) << "],"
              << "[" << num(lng2) << "," << num(lat2) << "],"
              << "[" << num(lng1) << "," << num(lat2) << "],"
              << "[" << num(lng1) << "," << num(lat1) << "]]]}}";
        }
    }
    f << "\n]}\n";
    return (bool)f;
}

static void writeRouteFeature(std::ofstream& f, const Route& r, const char* type) {
    f << "{\"type\":\"Feature\",\"properties\":{"
      << "\"type\":\"" << type << "\","
      << "\"distance\":" << num(r.distance, 1) << ","
      << "\"exposure\":" << num(r.exposure, 1) << ","
      << "\"minutes\":" << num(r.minutes, 1) << "},"
      << "\"geometry\":{\"type\":\"LineString\",\"coordinates\":[";
    int n = r.lats.size() < r.lngs.size() ? r.lats.size() : r.lngs.size();
    for (int i = 0; i < n; ++i) {
        if (i > 0) f << ",";
        f << "[" << num(r.lngs[i]) << "," << num(r.lats[i]) << "]";
    }
    f << "]}}";
}

bool writeRoutesGeoJSON(const Route& best, const Route& shortest, const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;
    f << "{\"type\":\"FeatureCollection\",\"features\":[\n";
    writeRouteFeature(f, best, "best");
    f << ",\n";
    writeRouteFeature(f, shortest, "shortest");
    f << "\n]}\n";
    return (bool)f;
}

static void writeRankList(std::ofstream& f, const DynamicArray<RankItem>& list) {
    f << "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) f << ",";
        f << "{\"name\":" << quote(list[i].name) << ",\"score\":" << num(list[i].score, 1) << "}";
    }
    f << "]";
}

bool writeRankingJSON(const DynamicArray<RankItem>& top, const DynamicArray<RankItem>& worst,
                      const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;
    f << "{\"top\":";
    writeRankList(f, top);
    f << ",\"worst\":";
    writeRankList(f, worst);
    f << "}\n";
    return (bool)f;
}
