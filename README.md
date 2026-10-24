# Seoul Clean Route

서울시 기후 데이터를 반영해 출발지와 목적지 사이의 최적 산책·러닝 경로를 추천하는 웹 서비스입니다.
(광운대학교 2026-2 자료구조 팀 프로젝트)

## 프로젝트 개요

사용자가 분석 기간(2025년 2·5·8·11월)과 출발지·목적지를 고르면

1. 해당 기간 관측 데이터를 AVL 트리로 꺼내 관측소별 평균을 구하고
2. 서울을 격자로 나눠 칸마다 미세먼지·기온을 보간한 뒤
3. 칸을 정점으로 하는 격자 그래프에서 최소 힙 기반 다익스트라로 최적 경로를 찾아
4. 웹 지도에 최적 경로와 최단 경로를 함께 보여줍니다.

서브기능: Top/Worst 지역 순위(노드 그래프 + 크기 k 힙), 바람 기반 미세먼지 예측(노드 그래프 + Stable Fluid), 지역명 자동완성(트라이)

## 담당

| 폴더 | 담당 | 내용 | 자료구조 |
|---|---|---|---|
| `src/common`, `src/search`, `src/main.cpp`, `web/` | 이준호 | 공통 형식, 통합, 웹, 지역명 검색 | 트라이 |
| `src/data` | 방지혁 | CSV 읽기, 기간 탐색, 관측소 평균, 격자 보간 | AVL 트리, 해시 테이블 |
| `src/graph` | 김재겸 | 동적 배열, 격자 그래프, 가중치, (최종) 바람 예측 | 동적 배열, 격자 그래프 |
| `src/route` | 조효근 | 다익스트라, 출발·도착 칸 보정, 경로 복원, 경로 비교 | 최소 힙, 스택, 큐 |
| `src/station` | 장데니스 | Top/Worst 순위, 관측소 바람 보정 | 노드 그래프, 크기 k 힙 |

## 중간발표(11/3)까지 목표

**작은 범위에서 "데이터 → 격자 → 최적 경로"가 한 번 돌아가 웹 지도에 그려지는 것**까지 만듭니다.
범위: 2025년 2월 자치구 측정소 25곳, 100m 격자 (간이측정망·S-DoT·OSM·Stable Fluid는 최종 발표까지)

| 담당 | 파일 | 할 일 | 마감 |
|---|---|---|---|
| 이준호 | `src/graph/DynamicArray.h` | 동적 배열 (**완료**, 모두 바로 사용 가능) | 완료 |
| 김재겸 | `src/graph/GridGraph.*` | 이웃 칸 계산, 가중치, 위경도 ↔ 칸 변환 | 10/26 |
| 방지혁 | `src/data/TimeAVL.*`, `HashTable.h`, `DataLoader.*` | 2월 데이터 읽기, AVL 삽입·범위 탐색, 관측소 평균, 100m 격자 보간 | 10/26 |
| 조효근 | `src/route/MinHeap.h`, `Stack.h`, `Queue.h`, `Router.*` | 힙·스택·큐, 다익스트라, 최단 경로와 비교 (+ 힙 유무 실행 시간 비교) | 10/26 |
| 장데니스 | `src/station/TopKHeap.h`, `StationGraph.*` | 크기 k 힙, 자치구 Top/Worst 5 출력 | 10/26 |
| 이준호 | `src/main.cpp`, `src/common/Export.*`, `web/` | 모듈 연결, 결과를 GeoJSON으로 저장, 웹 지도 표시, 트라이 | 10/31 |

- 다른 사람 모듈이 아직 없으면 `src/common/TestData.h`의 가짜 데이터로 먼저 테스트하세요.
  - `makeTestGrid(10, 10)`: 10×10 가짜 격자 (`makeTestGrid(10, 10, true)`는 가운데에 벽이 있는 격자)
  - `makeTestStations()`: 가짜 관측소 25개
- 각자 브랜치에서 작업하세요: 방지혁 `feature/data`, 김재겸 `feature/graph`, 조효근 `feature/route`, 장데니스 `feature/station`, 이준호 `feature/web`
- 각자 `tests/test_이름.cpp`로 자기 자료구조 테스트를 만들어 두면 발표 자료(구현 현황)에 바로 쓸 수 있습니다.
- 11/1 데모 화면 캡처, 11/2 리허설

## 개발 규칙

- **자기 폴더의 파일만 수정합니다.** 다른 사람 코드에 문제가 있으면 단톡방이나 Issue로 알려주세요.
- **STL 컨테이너와 알고리즘은 사용하지 않습니다.** (`vector`, `list`, `queue`, `stack`, `map`, `unordered_map`, `priority_queue`, `sort` 등)
  파일 읽기(`fstream`), 문자열(`string`), 수학 함수(`cmath`)는 사용합니다.
- 템플릿 클래스는 헤더 파일(`.h`)에 구현까지 작성합니다.
- `src/common/Types.h`의 데이터 형식과 각 헤더의 함수 선언(이름, 입력, 출력)은 바꾸기 전에 팀에 먼저 공유합니다.
- 다른 모듈이 아직 완성되지 않았으면, 작은 가짜 데이터로 먼저 개발하고 테스트합니다.
- **`DynamicArray`(김재겸)는 모든 모듈이 사용하므로 가장 먼저 완성합니다.**

## 브랜치와 커밋

1. `main`에서 자기 브랜치를 만듭니다. 예: `feature/data`, `feature/graph`, `feature/route`, `feature/station`
2. 작업 후 커밋하고 자기 브랜치에 push 합니다.
3. GitHub에서 Pull Request를 만들면 이준호가 확인 후 `main`에 합칩니다.

## 빌드와 실행

```bash
make          # g++로 콘솔 버전 빌드 → ./build/app
make test     # tests/ 폴더의 테스트 빌드·실행
make wasm     # Emscripten(em++)으로 WebAssembly 빌드 → web/app.js, web/app.wasm
```

웹 화면은 `web/index.html`을 로컬 서버로 열어 확인합니다. (예: `python3 -m http.server -d web`)
최종 결과물은 GitHub Pages로 배포합니다.

## 사용 데이터

| 구분 | 데이터 | 링크 |
|---|---|---|
| 기본 | 서울시 실시간 자치구별 대기환경 현황 (OA-1200) | https://data.seoul.go.kr/dataList/OA-1200/S/1/datasetView.do |
| 기본 (과거 자료) | 서울시 기간별 시간평균 대기환경 정보 (OA-2221) | https://data.seoul.go.kr/dataList/OA-2221/S/1/datasetView.do |
| 추가 1 | 서울특별시 대기환경정보 초미세먼지(간이측정망) | https://cleanair.seoul.go.kr/airquality/simpleAvg |
| 추가 2 | 스마트서울 도시데이터 센서(S-DoT) 환경정보 (OA-15969) | https://data.seoul.go.kr/dataList/OA-15969/S/1/datasetView.do |
| 추가 3 | OpenStreetMap | https://www.openstreetmap.org |

분석 기간: 2025년 2월, 5월, 8월, 11월

## 자료구조 활용

| 자료구조 | 적용 대상 | 주요 연산 |
|---|---|---|
| 격자 그래프 | 서울 지도 격자 | 칸 = 정점, 8방향 이웃 = 간선, 이웃 조회 O(1) |
| 최소 힙 | 다익스트라 | push·pop O(log V), 전체 O(E log V) |
| 스택 | 경로 복원 | prev를 따라 push 후 pop, O(L) |
| 큐 | 출발·도착 칸 보정 | BFS로 가장 가까운 통행 가능 칸 탐색 |
| 노드 그래프 | 관측소 분석 | 관측소 = 노드, 인접 관측소 간 간선 |
| 크기 k 힙 | Top/Worst 순위 | O(M log k) |
| 트라이 | 지역명 검색 | 접두사 검색 O(입력 길이 + 후보 수) |
| AVL 트리 | 기간 데이터 | 회전으로 균형 유지, 범위 탐색 O(log T + k) |
| 동적 배열, 해시 테이블 | 데이터·격자 저장, 칸별 관측소 버킷 | 접근 O(1) |

## 핵심 로직

(구현 후 정리 예정)
