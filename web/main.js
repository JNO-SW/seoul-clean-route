// 담당: 이준호
// TODO: WebAssembly 모듈(app.js) 로드 후 C++ 함수 연결
const map = L.map('map').setView([37.5665, 126.9780], 12);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
  maxZoom: 18,
  attribution: '&copy; OpenStreetMap contributors'
}).addTo(map);

let start = null, goal = null;
map.on('click', (e) => {
  if (!start) { start = L.marker(e.latlng).addTo(map); }
  else if (!goal) { goal = L.marker(e.latlng).addTo(map); /* TODO: api_find_route 호출 */ }
});
