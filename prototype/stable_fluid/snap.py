# sim.js 결과를 PNG로 저장 (0분, 90분, 180분)
import json, math, sys
from PIL import Image, ImageDraw
sys.stdout.reconfigure(encoding='utf-8')
def load(p, key):
    s = open(p, encoding='utf-8').read(); return json.loads(s[s.index('{'):s.rindex('}') + 1])
M, S = load('meta.js', 'META'), load('sim.js', 'SIM')
NX, NY, DS, VDS = M['nx'], M['ny'], S['pmDs'], S['velDs']
PW, PH, VW, VH = -(-NX // DS), -(-NY // DS), -(-NX // VDS), -(-NY // VDS)
SC = 2  # 픽셀/칸
STOPS = [(0, (47, 128, 237)), (15, (39, 174, 96)), (35, (242, 153, 74)), (75, (235, 87, 87))]
def color(v):
    if v >= 75: return STOPS[3][1]
    for (a, ca), (b, cb) in zip(STOPS, STOPS[1:]):
        if v <= b:
            t = (v - a) / (b - a); return tuple(int(ca[k] + (cb[k] - ca[k]) * t) for k in range(3))
mask = M['maskDs']
def to_px(gx, gy): return gx * SC, (NY - gy) * SC
def ll(lng, lat): return (lng - M['minLng']) / M['dlng'], (lat - M['minLat']) / M['dlat']
tiles = []
for idx in (0, 18, 36):
    f = S['frames'][idx]
    small = Image.new('RGB', (PW, PH))
    px = small.load()
    for j in range(PH):
        for i in range(PW):
            c = color(f['pm'][j * PW + i] / 4)
            if mask[j * PW + i] == '0': c = tuple(int(x * 0.45 + 255 * 0.55) for x in c)
            px[i, PH - 1 - j] = c
    img = small.resize((PW * DS * SC, PH * DS * SC), Image.BILINEAR).crop((0, PH * DS * SC - NY * SC, NX * SC, PH * DS * SC))
    d = ImageDraw.Draw(img)
    for g in M['gu']:
        for ring in g['rings']:
            d.line([to_px(*ll(*p)) for p in ring + ring[:1]], fill=(40, 40, 50), width=1)
    for j in range(1, VH):
        for i in range(1, VW):
            u, v = f['u'][j * VW + i] / 100, f['v'][j * VW + i] / 100
            x, y = to_px(i * VDS, j * VDS); L = 2.6 * SC
            d.line([(x, y), (x + u * L, y - v * L)], fill=(20, 20, 30), width=1)
            d.ellipse([x - 1, y - 1, x + 1, y + 1], fill=(20, 20, 30))
    for s in M['stations']:
        x, y = to_px(*ll(s['lng'], s['lat']))
        d.line([(x, y), (x + s['u'] * 8, y - s['v'] * 8)], fill=(255, 255, 255), width=3)
        d.ellipse([x - 4, y - 4, x + 4, y + 4], fill=(255, 255, 255), outline=(0, 0, 0))
    d.rectangle([0, 0, 120, 26], fill=(255, 255, 255)); d.text((8, 7), f"+{f['t']:.0f} min", fill=(0, 0, 0))
    tiles.append(img)
W, H = tiles[0].size
out = Image.new('RGB', (W * 3 + 20, H), (255, 255, 255))
for k, im in enumerate(tiles): out.paste(im, (k * (W + 10), 0))
out.save('snapshots.png'); print(out.size)
