# 데이터

- `raw/`: 다운로드한 원본 파일 (GitHub에 올리지 않음)
- `processed/`: 정리한 파일 (UTF-8, 프로그램이 읽는 파일)

## processed 파일

### air_2025_MM.csv (2025년 2·5·8·11월, 자치구 측정소 25곳 × 시간별)
출처: 서울시 기간별 시간평균 대기환경 정보 (OA-2221) 2025년 파일에서 해당 월만 추출

| 열 | 내용 | 단위 |
|---|---|---|
| datetime | 측정 시각 `YYYYMMDDHH` (예: 2025021509 = 2월 15일 9시) | |
| station_code | 측정소 코드 (stations.csv의 code와 연결) | |
| station_name | 측정소 이름 (자치구) | |
| pm10 | 미세먼지 1시간 | ㎍/㎥ |
| pm25 | 초미세먼지 | ㎍/㎥ |
| o3, no2, co, so2 | 오존, 이산화질소, 일산화탄소, 아황산가스 | ppm |

- **빈칸 = 측정값 없음** (점검·고장). 평균 낼 때 빼야 합니다.
- 2월 16,800행 (25곳 × 28일 × 24시간)

### stations.csv (측정소 위치)
출처: 서울시 대기오염 측정소 정보 (OA-23039), 좌표는 GRS80 UTM-K(EPSG:5179)를 위도·경도로 변환

| 열 | 내용 |
|---|---|
| code | 측정소 코드 (air 파일의 station_code와 같음) |
| name | 측정소 이름 |
| lat, lng | 위도, 경도 |
| address | 주소 |
| note | 비고 |

- **용산구, 관악구, 금천구는 측정소를 옮겨서 좌표가 비어 있음** → 채워질 때까지 빈 좌표인 측정소는 건너뛰고 계산

## 원본 데이터 링크

| 데이터 | 링크 |
|---|---|
| 서울시 기간별 시간평균 대기환경 정보 (OA-2221) | https://data.seoul.go.kr/dataList/OA-2221/S/1/datasetView.do |
| 서울시 대기오염 측정소 정보 (OA-23039) | https://data.seoul.go.kr/dataList/OA-23039/S/1/datasetView.do |
| 초미세먼지 간이측정망 (최종 발표용) | https://cleanair.seoul.go.kr/airquality/simpleAvg |
| S-DoT 환경정보 OA-15969 (최종 발표용) | https://data.seoul.go.kr/dataList/OA-15969/S/1/datasetView.do |
