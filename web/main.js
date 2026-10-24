// 담당: 이준호
// 중간발표 데모: C++ 프로그램이 만든 web/data/grid.geojson, routes.geojson 을 읽어 지도에 표시
// 최종: WebAssembly(app.js) 함수를 직접 호출하도록 변경 예정
const map = L.map('map').setView([37.5665, 126.9780], 12);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
  maxZoom: 18,
  attribution: '&copy; OpenStreetMap contributors'
}).addTo(map);

// 미세먼지 농도 → 색 (좋음 / 보통 / 나쁨 / 매우나쁨)
function pmColor(pm) {
  if (pm <= 15) return '#2F80ED';
  if (pm <= 35) return '#27AE60';
  if (pm <= 75) return '#F2994A';
  return '#EB5757';
}

async function loadLayer(url, options) {
  try {
    const res = await fetch(url);
    if (!res.ok) return null;
    return L.geoJSON(await res.json(), options).addTo(map);
  } catch (e) { return null; }
}

loadLayer('data/grid.geojson', {
  style: f => ({ stroke: false, fillColor: pmColor(f.properties.pm25), fillOpacity: 0.35 })
});

loadLayer('data/routes.geojson', {
  style: f => f.properties.type === 'best'
    ? { color: '#0A9A0A', weight: 5 }
    : { color: '#555', weight: 3, dashArray: '6 6' },
  onEachFeature: (f, layer) => layer.bindPopup(
    `${f.properties.type === 'best' ? '최적 경로' : '최단 경로'}<br>` +
    `거리 ${Math.round(f.properties.distance)} m<br>노출량 ${f.properties.exposure.toFixed(1)}`)
});
