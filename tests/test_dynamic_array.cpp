// 동적 배열 테스트 (make test 로 실행)
#include <cstdio>
#include "graph/DynamicArray.h"
#include "common/TestData.h"

static int failed = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf("  실패: %s (%s:%d)\n", #cond, __FILE__, __LINE__); ++failed; } } while (0)

int main() {
    std::printf("[DynamicArray 테스트]\n");

    DynamicArray<int> a;
    CHECK(a.empty());
    for (int i = 0; i < 100; ++i) a.push_back(i);
    CHECK(a.size() == 100);
    for (int i = 0; i < 100; ++i) CHECK(a[i] == i);

    DynamicArray<int> b(a);          // 복사 생성자
    b[0] = 999;
    CHECK(a[0] == 0);                // 원본은 그대로

    DynamicArray<int> c;
    c = a;                           // 대입 연산자
    c.pop_back();
    CHECK(c.size() == 99);
    CHECK(a.size() == 100);

    a.resize(120, -1);
    CHECK(a.size() == 120 && a[119] == -1 && a[99] == 99);
    a.clear();
    CHECK(a.empty());

    Grid g = makeTestGrid(10, 10, true);
    CHECK(g.pm25.size() == 100);
    CHECK(!g.passable[1 * 10 + 5] && g.passable[0 * 10 + 5]);

    DynamicArray<Station> s = makeTestStations();
    CHECK(s.size() == 25);

    std::printf(failed == 0 ? "  모두 통과\n" : "  실패 %d개\n", failed);
    return failed == 0 ? 0 : 1;
}
