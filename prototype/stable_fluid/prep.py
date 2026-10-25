# 서울 모양 격자와 관측소 입력을 만드는 준비 스크립트
# - seoul_gu.json(구 경계)으로 100m 격자의 서울 마스크(구 번호) 생성
# - 저장소의 실제 측정소 25곳 위치 + 2025년 2월 평균 PM2.5
# - 바람은 임의 값 (서북서풍 기준 + 관측소마다 흔들림)
import csv, json, math, os, random, sys
sys.stdout.reconfigure(encoding='utf-8')

REPO = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'data', 'processed')   # 저장소 data/processed
CELL = 100.0            # 격자 간격 (m)
MARGIN_KM = 3.0         # 서울 바깥 여유 영역
M_LAT = 110990.0

gu = json.load(open('seoul_gu.json', encoding='utf-8'))['features']
rings = []   # (구 번호, [(lng, lat), ...])
for k, f in enumerate(gu):
    geom = f['geometry']
    polys = [geom['coordinates']] if geom['type'] == 'Polygon' else geom['coordinates']
    for poly in polys:
        rings.append((k + 1, poly[0]))

lats = [p[1] for _, r in rings for p in r]
lngs = [p[0] for _, r in rings for p in r]
midLat = (min(lats) + max(lats)) / 2
M_LNG = 111320.0 * math.cos(math.radians(midLat))
dlat, dlng = CELL / M_LAT, CELL / M_LNG
minLat = min(lats) - MARGIN_KM * 1000 / M_LAT
minLng = min(lngs) - MARGIN_KM * 1000 / M_LNG
ny = int(math.ceil((max(lats) + MARGIN_KM * 1000 / M_LAT - minLat) / dlat))
nx = int(math.ceil((max(lngs) + MARGIN_KM * 1000 / M_LNG - minLng) / dlng))

# 스캔라인으로 칸 중심이 어느 구 안에 있는지 표시 (0 = 서울 밖)
mask = bytearray(nx * ny)
for gid, ring in rings:
    for j in range(ny):
        y = minLat + (j + 0.5) * dlat
        xs = []
        for (x1, y1), (x2, y2) in zip(ring, ring[1:] + ring[:1]):
            if (y1 <= y < y2) or (y2 <= y < y1):
                xs.append(x1 + (y - y1) * (x2 - x1) / (y2 - y1))
        xs.sort()
        for a, b in zip(xs[0::2], xs[1::2]):
            i0 = max(0, int(math.ceil((a - minLng) / dlng - 0.5)))
            i1 = min(nx - 1, int(math.floor((b - minLng) / dlng - 0.5)))
            for i in range(i0, i1 + 1):
                mask[j * nx + i] = gid
open('mask.bin', 'wb').write(bytes(mask))

# 측정소 위치 + 2월 평균 PM2.5 (실제 값)
pm_sum, pm_cnt = {}, {}
for r in csv.DictReader(open(f'{REPO}/air_2025_02.csv', encoding='utf-8')):
    if r['pm25']:
        pm_sum[r['station_code']] = pm_sum.get(r['station_code'], 0) + float(r['pm25'])
        pm_cnt[r['station_code']] = pm_cnt.get(r['station_code'], 0) + 1

random.seed(7)
BASE_DIR = 290.0   # 풍향 290도 = 서북서에서 불어오는 바람 (겨울철 북서풍 느낌)
stations = []
for r in csv.DictReader(open(f'{REPO}/stations.csv', encoding='utf-8')):
    wd = BASE_DIR + random.uniform(-35, 35)          # 임의 풍향
    ws = random.uniform(1.5, 4.5)                    # 임의 풍속 (m/s)
    u = -ws * math.sin(math.radians(wd))             # 기상 풍향은 '불어오는 방향'이라 부호 반대
    v = -ws * math.cos(math.radians(wd))
    pm = pm_sum[r['code']] / pm_cnt[r['code']]
    stations.append(dict(name=r['name'], lat=float(r['lat']), lng=float(r['lng']),
                         wd=round(wd, 1), ws=round(ws, 2), u=u, v=v, pm25=round(pm, 1)))

with open('input.txt', 'w', encoding='utf-8') as f:
    f.write(f'{nx} {ny} {minLat:.8f} {minLng:.8f} {dlat:.10f} {dlng:.10f} {CELL}\n{len(stations)}\n')
    for s in stations:
        f.write(f"{s['lat']:.6f} {s['lng']:.6f} {s['u']:.4f} {s['v']:.4f} {s['pm25']:.2f}\n")

inside = sum(1 for m in mask if m)
print(f'격자 {nx} x {ny} = {nx*ny}칸 (서울 안 {inside}칸 ≈ {inside*CELL*CELL/1e6:.0f} km²)')
for s in stations:
    print(f"  {s['name']:5s} 풍향 {s['wd']:5.1f}° 풍속 {s['ws']:.1f} m/s  PM2.5 {s['pm25']}")

# 화면(view.html)용: 구 경계, 관측소, 미세먼지 프레임과 같은 간격(3칸)으로 뽑은 서울 마스크
PM_DS = 3
mask_ds = ''.join('1' if mask[j * nx + i] else '0' for j in range(0, ny, PM_DS) for i in range(0, nx, PM_DS))
gu_out = [dict(name=f['properties']['name'],
               rings=[[[round(x, 5), round(y, 5)] for x, y in ring] for gid, ring in rings if gid == k + 1])
          for k, f in enumerate(gu)]
meta = dict(nx=nx, ny=ny, minLat=minLat, minLng=minLng, dlat=dlat, dlng=dlng, cell=CELL,
            stations=stations, gu=gu_out, maskDs=mask_ds, pmDs=PM_DS)
with open('meta.js', 'w', encoding='utf-8') as f:
    f.write('window.META=' + json.dumps(meta, ensure_ascii=False, separators=(',', ':')) + ';\n')
print('meta.js 저장')
