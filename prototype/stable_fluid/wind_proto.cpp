// 서울 Stable Fluid 프로토타입 (CPU, 단일 스레드, STL 컨테이너 없음)
// - 관측소 바람을 외력(nudging)으로 넣어 100m 격자 바람장 계산
// - 바람장으로 미세먼지(PM2.5)를 이류시켜 3시간 뒤까지 예측
// 입력: input.txt (prep.py가 생성)   출력: sim.js (view.html이 읽음)
#include <cstdio>
#include <cmath>
#include <chrono>
#include "graph/DynamicArray.h"

typedef DynamicArray<double> Field;

// ───── 설정 ─────
const double DT = 30.0;            // 시간 간격 (s)
const int    STEPS = 360;          // 360 × 30s = 3시간
const int    FRAME_EVERY = 10;     // 5분마다 화면용 프레임 저장
const int    JACOBI = 60;          // 압력 Jacobi 반복 횟수 (이전 스텝 압력에서 시작)
const double NUDGE_SIGMA = 10.0;   // 관측소 영향 반경 σ (칸) = 1km
const double NUDGE_TAU = 60.0;     // 관측 바람으로 끌어당기는 시간 상수 (s)
const double PM_HAZE = 35.0;       // 시나리오: 바깥에서 들어오는 고농도 공기 덩어리의 최대 추가 농도 (㎍/㎥)
const double HAZE_BAND = 45.0;     // 유입 경계에서 공기 덩어리가 깔려 있는 폭 (칸) = 4.5km
const double PM_DECAY = 1.0 / (12 * 3600.0);   // 침적·소멸 (12시간에 1/e)
const int    PM_DS = 3;            // 화면용 미세먼지 해상도 (3칸 = 300m)
const int    VEL_DS = 8;           // 화면용 바람 해상도 (8칸 = 800m)

int nx, ny;
double minLat, minLng, dlat, dlng, H;
double meanU = 0, meanV = 0;   // 관측소 평균 바람 (공기 덩어리가 흘러오는 방향)
double simTime = 0;            // 시뮬레이션 시각 (s)
Field pmBase;                  // 관측소 IDW 농도 (배경)

inline int ID(int i, int j) { return j * nx + i; }

// 쌍선형 보간. 격자 밖 좌표는 가장자리로 고정
double bilerp(const Field& f, double x, double y) {
    if (x < 0) x = 0;
    if (x > nx - 1.0001) x = nx - 1.0001;
    if (y < 0) y = 0;
    if (y > ny - 1.0001) y = ny - 1.0001;
    int i = (int)x, j = (int)y;
    double tx = x - i, ty = y - j;
    return f[ID(i, j)] * (1 - tx) * (1 - ty) + f[ID(i + 1, j)] * tx * (1 - ty)
         + f[ID(i, j + 1)] * (1 - tx) * ty + f[ID(i + 1, j + 1)] * tx * ty;
}

// 구름 모양 무늬 0~1: 파장 4~12km 사인파 6개를 겹친 뒤 부드럽게 잘라 덩어리로 만듦
double haze(double x, double y) {
    static const double DIR[6] = {0.3, 1.4, 2.2, 3.1, 4.0, 5.3};
    static const double LEN[6] = {120, 85, 60, 100, 45, 70};
    static const double PHASE[6] = {0.7, 2.9, 4.1, 1.3, 5.6, 3.3};
    double n = 0;
    for (int k = 0; k < 6; ++k)
        n += std::sin((x * std::cos(DIR[k]) + y * std::sin(DIR[k])) * 6.2831853 / LEN[k] + PHASE[k]);
    n /= std::sqrt(3.0);                       // 표준편차 약 1
    double t = (n + 0.4) / 1.4;                // -0.4 ~ 1.0 구간을 0~1로
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return t * t * (3 - 2 * t);                // smoothstep
}

// 유입 경계에 가까울수록 1 (경계에서 HAZE_BAND칸 안쪽까지 부드럽게 줄어듦), 유입이 약한 경계는 약하게
double inflowWeight(double x, double y) {
    double sp = std::sqrt(meanU * meanU + meanV * meanV) + 1e-9;
    double edges[4][2] = {{x, meanU}, {nx - 1 - x, -meanU}, {y, meanV}, {ny - 1 - y, -meanV}};  // (경계까지 거리, 그 경계로 들어오는 바람 성분)
    double w = 0;
    for (int e = 0; e < 4; ++e) {
        double strength = edges[e][1] / sp;     // 바람이 그 경계로 들어오는 정도 (0~1)
        if (strength <= 0) continue;
        double t = 1.0 - edges[e][0] / HAZE_BAND;
        if (t <= 0) continue;
        double ww = strength * t * t * (3 - 2 * t);
        if (ww > w) w = ww;
    }
    return w;
}

// 바깥 공기의 농도: 배경 + 구름 무늬(평균 바람을 따라 이동) × 유입 경계 가중치
double externalPm(int i, int j) {
    double sx = i - meanU * simTime / H, sy = j - meanV * simTime / H;   // 무늬가 바람을 타고 흘러옴
    return pmBase[ID(i, j)] + PM_HAZE * haze(sx, sy) * inflowWeight(i, j);
}

// 가장자리 칸 e번째의 좌표 (아래 줄 → 위 줄 → 왼쪽 열 → 오른쪽 열)
inline void edgeCell(int e, int& i, int& j) {
    if (e < nx) { i = e; j = 0; return; }
    e -= nx;
    if (e < nx) { i = e; j = ny - 1; return; }
    e -= nx;
    if (e < ny - 2) { i = 0; j = e + 1; return; }
    e -= ny - 2;
    i = nx - 1; j = e + 1;
}

// 바람이 들어오는 경계(유입)인지: 바깥 방향 법선과 배경 바람의 내적이 음수
bool inflowAt(const Field& ub, const Field& vb, int i, int j) {
    double nxo = (i == 0) ? -1 : (i == nx - 1) ? 1 : 0;
    double nyo = (j == 0) ? -1 : (j == ny - 1) ? 1 : 0;
    return ub[ID(i, j)] * nxo + vb[ID(i, j)] * nyo < 0;
}

// 경계 조건: 유입 쪽은 배경 바람·유입 농도 고정, 유출 쪽은 안쪽 값 복사
void boundary(Field& u, Field& v, Field* pm, const Field& ub, const Field& vb) {
    for (int e = 0; e < 2 * (nx + ny) - 4; ++e) {
        int i, j;
        edgeCell(e, i, j);
        int k = ID(i, j);
        if (inflowAt(ub, vb, i, j)) {
            u[k] = ub[k]; v[k] = vb[k];
            if (pm) (*pm)[k] = externalPm(i, j);
        } else {
            int ii = i == 0 ? 1 : i == nx - 1 ? nx - 2 : i;
            int jj = j == 0 ? 1 : j == ny - 1 ? ny - 2 : j;
            u[k] = u[ID(ii, jj)]; v[k] = v[ID(ii, jj)];
            if (pm) (*pm)[k] = (*pm)[ID(ii, jj)];
        }
    }
}

// 압력 경계: 유출 쪽 압력 0, 유입 쪽은 안쪽 값 복사(기울기 0)
void boundaryPressure(Field& p, const Field& ub, const Field& vb) {
    for (int e = 0; e < 2 * (nx + ny) - 4; ++e) {
        int i, j;
        edgeCell(e, i, j);
        int ii = i == 0 ? 1 : i == nx - 1 ? nx - 2 : i;
        int jj = j == 0 ? 1 : j == ny - 1 ? ny - 2 : j;
        p[ID(i, j)] = inflowAt(ub, vb, i, j) ? p[ID(ii, jj)] : 0.0;
    }
}

double maxDivergence(const Field& u, const Field& v) {
    double m = 0;
    for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i) {
        double d = std::fabs(0.5 * (u[ID(i + 1, j)] - u[ID(i - 1, j)] + v[ID(i, j + 1)] - v[ID(i, j - 1)]));
        if (d > m) m = d;
    }
    return m;
}

// 발산 제거: div 계산 → 압력 Jacobi → 압력 기울기 빼기
void project(Field& u, Field& v, Field& p, Field& pt, Field& div, const Field& ub, const Field& vb) {
    for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i)
        div[ID(i, j)] = 0.5 * (u[ID(i + 1, j)] - u[ID(i - 1, j)] + v[ID(i, j + 1)] - v[ID(i, j - 1)]);
    for (int it = 0; it < JACOBI; ++it) {
        for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i)
            pt[ID(i, j)] = (p[ID(i - 1, j)] + p[ID(i + 1, j)] + p[ID(i, j - 1)] + p[ID(i, j + 1)] - div[ID(i, j)]) * 0.25;
        p.swap(pt);
        boundaryPressure(p, ub, vb);
    }
    for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i) {
        u[ID(i, j)] -= 0.5 * (p[ID(i + 1, j)] - p[ID(i - 1, j)]);
        v[ID(i, j)] -= 0.5 * (p[ID(i, j + 1)] - p[ID(i, j - 1)]);
    }
}

void writeFrame(FILE* out, const Field& pm, const Field& u, const Field& v, double minutes, bool first) {
    std::fprintf(out, "%s{\"t\":%.0f,\"pm\":[", first ? "" : ",\n", minutes);
    for (int j = 0; j < ny; j += PM_DS) for (int i = 0; i < nx; i += PM_DS)
        std::fprintf(out, "%s%d", (i == 0 && j == 0) ? "" : ",", (int)std::lround(pm[ID(i, j)] * 4));
    std::fprintf(out, "],\"u\":[");
    for (int j = 0; j < ny; j += VEL_DS) for (int i = 0; i < nx; i += VEL_DS)
        std::fprintf(out, "%s%d", (i == 0 && j == 0) ? "" : ",", (int)std::lround(u[ID(i, j)] * 100));
    std::fprintf(out, "],\"v\":[");
    for (int j = 0; j < ny; j += VEL_DS) for (int i = 0; i < nx; i += VEL_DS)
        std::fprintf(out, "%s%d", (i == 0 && j == 0) ? "" : ",", (int)std::lround(v[ID(i, j)] * 100));
    std::fprintf(out, "]}");
}

int main() {
    FILE* in = std::fopen("input.txt", "r");
    if (!in) { std::printf("input.txt 없음 (prep.py 먼저 실행)\n"); return 1; }
    int ns;
    if (std::fscanf(in, "%d %d %lf %lf %lf %lf %lf %d", &nx, &ny, &minLat, &minLng, &dlat, &dlng, &H, &ns) != 8) return 1;
    DynamicArray<double> sx, sy, su, sv, spm;
    for (int s = 0; s < ns; ++s) {
        double lat, lng, uu, vv, pm;
        if (std::fscanf(in, "%lf %lf %lf %lf %lf", &lat, &lng, &uu, &vv, &pm) != 5) return 1;
        sx.push_back((lng - minLng) / dlng - 0.5);   // 칸 좌표 (칸 중심 = 정수)
        sy.push_back((lat - minLat) / dlat - 0.5);
        su.push_back(uu); sv.push_back(vv); spm.push_back(pm);
        meanU += uu / ns; meanV += vv / ns;
    }
    std::fclose(in);

    int n = nx * ny;
    Field u0, u1, v0, v1, p, pt, div, pm0, pm1, ub, vb, ut, vt, a;
    Field* all[] = {&u0, &u1, &v0, &v1, &p, &pt, &div, &pm0, &pm1, &ub, &vb, &ut, &vt, &a};
    for (Field* f : all) f->resize(n, 0.0);

    auto t0 = std::chrono::steady_clock::now();

    // 배경 바람·초기 미세먼지: 관측소 역거리 가중 보간(IDW). 외력 목표: 관측소 주변 가우시안 가중 평균
    for (int j = 0; j < ny; ++j) for (int i = 0; i < nx; ++i) {
        double w = 0, wu = 0, wv = 0, wpm = 0, g = 0, gu = 0, gv = 0;
        for (int s = 0; s < ns; ++s) {
            double d2 = (i - sx[s]) * (i - sx[s]) + (j - sy[s]) * (j - sy[s]);
            double iw = 1.0 / (d2 + 1.0);
            w += iw; wu += iw * su[s]; wv += iw * sv[s]; wpm += iw * spm[s];
            double gg = std::exp(-d2 / (2 * NUDGE_SIGMA * NUDGE_SIGMA));
            g += gg; gu += gg * su[s]; gv += gg * sv[s];
        }
        int k = ID(i, j);
        ub[k] = wu / w; vb[k] = wv / w; pm0[k] = wpm / w;
        ut[k] = g > 1e-9 ? gu / g : ub[k];
        vt[k] = g > 1e-9 ? gv / g : vb[k];
        a[k] = g < 1.0 ? g : 1.0;
        u0[k] = ub[k]; v0[k] = vb[k];
    }
    // 초기 미세먼지: 관측소 IDW + 유입 경계 쪽에 깔린 고농도 공기 덩어리
    pmBase = pm0;
    for (int j = 0; j < ny; ++j) for (int i = 0; i < nx; ++i) pm0[ID(i, j)] = externalPm(i, j);
    double divBefore = maxDivergence(u0, v0);
    for (int r = 0; r < 10; ++r) {   // 처음 바람장을 비압축성으로
        project(u0, v0, p, pt, div, ub, vb);
        boundary(u0, v0, nullptr, ub, vb);
    }
    double divAfter = maxDivergence(u0, v0);
    double setupMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();

    FILE* out = std::fopen("sim.js", "w");
    std::fprintf(out, "window.SIM={\"nx\":%d,\"ny\":%d,\"pmDs\":%d,\"velDs\":%d,\"pmHaze\":%.1f,\"frames\":[\n",
                 nx, ny, PM_DS, VEL_DS, PM_HAZE);
    writeFrame(out, pm0, u0, v0, 0, true);

    double stepMs = 0, divStepMax = 0;
    double r = DT / NUDGE_TAU;
    for (int step = 1; step <= STEPS; ++step) {
        auto s0 = std::chrono::steady_clock::now();
        simTime = step * DT;
        // 1. 외력: 관측소 근처 바람을 관측값 쪽으로 당김
        for (int k = 0; k < n; ++k) {
            u0[k] += r * a[k] * (ut[k] - u0[k]);
            v0[k] += r * a[k] * (vt[k] - v0[k]);
        }
        // 2. 이류: 최신 속도(u0)로 거슬러 올라가 값을 가져옴 (semi-Lagrangian)
        for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i) {
            int k = ID(i, j);
            double x = i - DT * u0[k] / H, y = j - DT * v0[k] / H;
            u1[k] = bilerp(u0, x, y);
            v1[k] = bilerp(v0, x, y);
        }
        boundary(u1, v1, nullptr, ub, vb);
        // 3. 투영: 발산 제거
        project(u1, v1, p, pt, div, ub, vb);
        boundary(u1, v1, nullptr, ub, vb);
        // 4. 미세먼지: 새 바람으로 이류 + 감쇠, 경계 유입
        double keep = 1.0 / (1.0 + PM_DECAY * DT);
        for (int j = 1; j < ny - 1; ++j) for (int i = 1; i < nx - 1; ++i) {
            int k = ID(i, j);
            pm1[k] = bilerp(pm0, i - DT * u1[k] / H, j - DT * v1[k] / H) * keep;
        }
        boundary(u1, v1, &pm1, ub, vb);
        u0.swap(u1); v0.swap(v1); pm0.swap(pm1);
        stepMs += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - s0).count();

        if (step % FRAME_EVERY == 0) {
            double d = maxDivergence(u0, v0);
            if (d > divStepMax) divStepMax = d;
            writeFrame(out, pm0, u0, v0, step * DT / 60.0, false);
        }
    }
    std::fprintf(out, "]};\n");
    std::fclose(out);

    // 결과 요약
    double maxSpeed = 0, meanSpeed = 0;
    for (int k = 0; k < n; ++k) {
        double sp = std::sqrt(u0[k] * u0[k] + v0[k] * v0[k]);
        meanSpeed += sp; if (sp > maxSpeed) maxSpeed = sp;
    }
    std::printf("격자 %d x %d = %d칸, 관측소 %d곳\n", nx, ny, n, ns);
    std::printf("준비(보간·초기 투영): %.0f ms\n", setupMs);
    std::printf("시뮬레이션 %d스텝 (%.1f시간): 총 %.2f s, 스텝당 %.2f ms (약 %.0f 스텝/초)\n",
                STEPS, STEPS * DT / 3600.0, stepMs / 1000, stepMs / STEPS, 1000.0 / (stepMs / STEPS));
    std::printf("최대 발산(칸당 m/s): 초기 보간 %.3f → 초기 투영 후 %.3f, 시뮬레이션 중 최대 %.3f\n",
                divBefore, divAfter, divStepMax);
    std::printf("최종 풍속: 평균 %.2f m/s, 최대 %.2f m/s\n", meanSpeed / n, maxSpeed);
    return 0;
}
