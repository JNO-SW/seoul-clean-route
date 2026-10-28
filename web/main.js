// 담당: 이준호
// 중간발표 데모: C++ 프로그램(./build/app)이 만든 web/data 파일을 읽어 지도에 표시
//   grid.geojson   칸별 미세먼지
//   routes.geojson 최적 경로(best) + 최단 경로(shortest)
//   ranking.json   Top / Worst 지역
// 최종: WebAssembly(app.js) 함수를 직접 호출하도록 변경 예정
const map = L.map('map').setView([37.5665, 126.9780], 11);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
  maxZoom: 18,
  attribution: '&copy; OpenStreetMap contributors'
}).addTo(map);

// 미세먼지(PM2.5) 농도 → 색. 데이터의 최솟값~최댓값을 파랑 → 초록 → 주황 → 빨강으로 이어서 칠함
// (한 달 평균은 값 차이가 작아서 환경부 4단계 기준으로 칠하면 서울 전체가 한 색이 됨)
const RAMP = [[47, 128, 237], [39, 174, 96], [242, 153, 74], [235, 87, 87]];
let pmMin = 0, pmMax = 100;
function pmColor(pm) {
  let t = pmMax > pmMin ? (pm - pmMin) / (pmMax - pmMin) : 0.5;
  t = Math.min(1, Math.max(0, t)) * (RAMP.length - 1);
  const i = Math.min(Math.floor(t), RAMP.length - 2), f = t - i;
  const c = RAMP[i].map((v, k) => Math.round(v + (RAMP[i + 1][k] - v) * f));
  return `rgb(${c[0]},${c[1]},${c[2]})`;
}

async function loadJSON(url) {
  try {
    const res = await fetch(url, { cache: 'no-store' });
    return res.ok ? await res.json() : null;
  } catch (e) { return null; }
}

function setHTML(id, html) { document.getElementById(id).innerHTML = html; }

async function main() {
  const [grid, routes, ranking] = await Promise.all([
    loadJSON('data/grid.geojson'), loadJSON('data/routes.geojson'), loadJSON('data/ranking.json')
  ]);

  if (!grid && !routes) {
    setHTML('result', '<h2>경로 비교</h2><p>결과 파일이 없습니다.<br>레포 폴더에서 <code>./build/app 출발지 목적지</code> 를 먼저 실행하세요.</p>');
  }

  if (grid) {
    const values = grid.features.map(f => f.properties.pm25);
    pmMin = Math.min(...values);
    pmMax = Math.max(...values);
    L.geoJSON(grid, {
      style: f => ({ stroke: false, fillColor: pmColor(f.properties.pm25), fillOpacity: 0.35 }),
      onEachFeature: (f, layer) => layer.bindTooltip(`PM2.5 ${f.properties.pm25} ㎍/㎥`)
    }).addTo(map);
  }

  if (routes) {
    const byType = {};
    routes.features.forEach(f => { byType[f.properties.type] = f; });
    // 최단 경로를 먼저 그리고, 최적 경로를 위에 겹쳐 그림
    const shortLayer = byType.shortest && L.geoJSON(byType.shortest, { style: { color: '#555', weight: 4, dashArray: '6 6' } }).addTo(map);
    const bestLayer = byType.best && L.geoJSON(byType.best, { style: { color: '#0A9A0A', weight: 5 } }).addTo(map);
    if (bestLayer) map.fitBounds(bestLayer.getBounds().pad(0.2));

    const b = byType.best && byType.best.properties;
    const s = byType.shortest && byType.shortest.properties;
    const coords = byType.best ? byType.best.geometry.coordinates : [];
    if (coords.length) {
      L.circleMarker([coords[0][1], coords[0][0]], { radius: 7, color: '#0A9A0A', fillOpacity: 1 }).addTo(map).bindTooltip('출발');
      const g = coords[coords.length - 1];
      L.circleMarker([g[1], g[0]], { radius: 7, color: '#EB5757', fillOpacity: 1 }).addTo(map).bindTooltip('도착');
    }
    if (b && s) {
      const cut = s.exposure > 0 ? (s.exposure - b.exposure) / s.exposure * 100 : 0;
      const more = s.distance > 0 ? (b.distance - s.distance) / s.distance * 100 : 0;
      const km = m => (m / 1000).toFixed(2) + ' km';
      setHTML('result', `
        <h2>경로 비교</h2>
        <table class="cmp">
          <tr><th></th><th><span class="sw best"></span>최적</th><th><span class="sw short"></span>최단</th></tr>
          <tr><td>거리</td><td>${km(b.distance)}</td><td>${km(s.distance)}</td></tr>
          <tr><td>소요 시간</td><td>${Math.round(b.minutes)}분</td><td>${Math.round(s.minutes)}분</td></tr>
          <tr><td>노출량</td><td>${Math.round(b.exposure).toLocaleString()}</td><td>${Math.round(s.exposure).toLocaleString()}</td></tr>
        </table>
        <p class="summary">미세먼지 노출 <b>${cut.toFixed(1)}% 감소</b> · 거리 ${more >= 0 ? '+' : ''}${more.toFixed(1)}%</p>`);
    }
  }

  if (ranking) {
    const list = items => items.map(r => `<li>${r.name} <span>${r.score.toFixed(1)}</span></li>`).join('');
    setHTML('ranking', `
      <h2>Top / Worst 지역 <small>PM2.5 평균</small></h2>
      <div class="rank"><div><h3>깨끗한 곳</h3><ol>${list(ranking.top)}</ol></div>
      <div><h3>나쁜 곳</h3><ol>${list(ranking.worst)}</ol></div></div>`);
  }

  // 범례
  const legend = L.control({ position: 'bottomleft' });
  legend.onAdd = () => {
    const div = L.DomUtil.create('div', 'legend');
    const stops = [0, 0.25, 0.5, 0.75, 1].map(t => pmColor(pmMin + (pmMax - pmMin) * t)).join(',');
    div.innerHTML = '<b>PM2.5 (㎍/㎥)</b>' +
      `<div class="bar" style="background:linear-gradient(to right,${stops})"></div>` +
      `<div class="ends"><span>${pmMin.toFixed(0)}</span><span>${pmMax.toFixed(0)}</span></div>`;
    return div;
  };
  legend.addTo(map);
}

main();
