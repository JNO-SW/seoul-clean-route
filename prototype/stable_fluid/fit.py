# 관측소 위치에서 격자 바람(마지막 프레임)과 관측 바람 비교
import json, math, sys
sys.stdout.reconfigure(encoding='utf-8')
def load(p):
    s = open(p, encoding='utf-8').read(); return json.loads(s[s.index('{'):s.rindex('}') + 1])
M, S = load('meta.js'), load(sys.argv[1] if len(sys.argv) > 1 else 'sim.js')
NX, NY, V = M['nx'], M['ny'], S['velDs']; VW, VH = -(-NX // V), -(-NY // V)
f = S['frames'][-1]
def samp(a, gx, gy):
    x, y = gx / V, gy / V; i, j = int(x), int(y); tx, ty = x - i, y - j; k = j * VW + i
    return (a[k] * (1 - tx) * (1 - ty) + a[k + 1] * tx * (1 - ty) + a[k + VW] * (1 - tx) * ty + a[k + VW + 1] * tx * ty) / 100
ang, spd = [], []
for s in M['stations']:
    gx = (s['lng'] - M['minLng']) / M['dlng'] - 0.5; gy = (s['lat'] - M['minLat']) / M['dlat'] - 0.5
    u, v = samp(f['u'], gx, gy), samp(f['v'], gx, gy)
    d = math.degrees(math.atan2(v, u) - math.atan2(s['v'], s['u'])); d = (d + 180) % 360 - 180
    ang.append(abs(d)); spd.append(abs(math.hypot(u, v) - s['ws']))
print(f"관측소 위치 오차: 풍향 평균 {sum(ang)/len(ang):.1f}° (최대 {max(ang):.1f}°), 풍속 평균 {sum(spd)/len(spd):.2f} m/s (최대 {max(spd):.2f})")
