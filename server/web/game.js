/* 游戏页面入口：负责沙粒网格、方块控制、游戏循环、主动结束和成绩提交。 */
const API_BASE = window.TETRIS_API_BASE || 'http://127.0.0.1:8080';
const canvas = document.getElementById('game'), ctx = canvas.getContext('2d');
const W = 80, H = 152, CELL = 5, COLS = 10, ROWS = 19;
const colors = ['#46a0e8', '#ffd642', '#54b565', '#eb554d', '#9b62e8', '#f39b32', '#35d9c4'];
const shapes = [
  [[0, 0], [1, 0], [-1, 0], [2, 0]], [[0, 0], [1, 0], [0, 1], [1, 1]],
  [[-1, 0], [0, 0], [1, 0], [0, 1]], [[0, 0], [1, 0], [-1, 1], [0, 1]],
  [[-1, 0], [0, 0], [0, 1], [1, 1]], [[-1, 0], [-1, 1], [0, 1], [1, 1]],
  [[1, 0], [-1, 1], [0, 1], [1, 1]]
];
let sand, current, next, x, y, rotation, score, layers, level, dropTimer, over,
    last = 0, scanLeft = false, scoreSubmitted = false;
const el = id => document.getElementById(id), idx = (x, y) => y * W + x;
const token = localStorage.getItem('tetris_token');
let user = null;
try {
  user = JSON.parse(localStorage.getItem('tetris_user') || 'null')
} catch (e) {
}
if (user && token) {
  el('mode').textContent = user.nickname || '已登录';
  el('gameNote').textContent = '已登录，游戏结束后成绩会自动上传排行榜。'
}
async function saveScore() {
  if (!token) {
    el('saveStatus').textContent = '本局已结束；登录后才能保存成绩';
    return
  }
  if (scoreSubmitted) return;
  scoreSubmitted = true;
  el('saveStatus').textContent = '正在保存成绩…';
  try {
    const r = await fetch(`${API_BASE}/api/scores`, {
      method: 'POST',
      headers: {'Content-Type': 'application/json', Authorization: `Bearer ${token}`},
      body: JSON.stringify({score, level, layers})
    });
    const d = await r.json();
    if (!r.ok) {
      if (r.status === 401) {
        localStorage.removeItem('tetris_token');
        localStorage.removeItem('tetris_user')
      }
      throw new Error(d.error || '保存失败')
    }
    el('saveStatus').textContent = `成绩已保存：${score} 分`;
    el('finishGame').disabled = true;
    localStorage.setItem(
        'tetris_last_score',
        JSON.stringify({score, level, layers, createdAt: new Date().toISOString()}))
  } catch (e) {
    el('saveStatus').textContent = `成绩保存失败：${e.message}`;
    scoreSubmitted = false
  }
}
function finishGame() {
  if (over) return;
  over = true;
  el('saveStatus').textContent =
      token ? '本局结束，正在保存成绩…' : '本局已结束；登录后才能保存成绩';
  el('finishGame').disabled = true;
  saveScore()
}
function rotatedCells(type, r = 0) {
  return shapes[type].map(([x0, y0]) => {
    let a = x0, b = y0;
    for (let i = 0; i < r; i++) [a, b] = [-b, a];
    return [a, b]
  })
}
function grainAt(x0, y0) {
  return x0 >= 0 && x0 < W && y0 >= 0 && y0 < H && sand[idx(x0, y0)] !== -1
}
function pieceFits(type, px, py, r = rotation) {
  return rotatedCells(type, r).every(([a, b]) => {
    const gx = (px + a) * 8, gy = (py + b) * 8;
    for (let yy = 0; yy < 8; yy++)
      for (let xx = 0; xx < 8; xx++)
        if (grainAt(gx + xx, gy + yy)) return false;
    return gx >= 0 && gx + 7 < W && gy + 7 < H
  });
}
function spawn() {
  current = next ?? Math.floor(Math.random() * 7);
  next = Math.floor(Math.random() * 7);
  x = 4;
  y = 0;
  rotation = 0;
  el('next').textContent = ['I', 'O', 'T', 'S', 'Z', 'J', 'L'][next];
  if (!pieceFits(current, x, y)) {
    over = true;
    saveScore()
  }
}
function emitPiece() {
  for (const [a, b] of rotatedCells(current)) {
    const gx = (x + a) * 8, gy = (y + b) * 8;
    for (let yy = 0; yy < 8; yy++)
      for (let xx = 0; xx < 8; xx++) {
        const nx = gx + xx, ny = gy + yy;
        if (nx >= 0 && nx < W && ny >= 0 && ny < H && Math.random() < 0.42)
          sand[idx(nx, ny)] = current
      }
  }
}
function updateSand() {
  for (let yy = H - 2; yy >= 0; yy--) {
    const start = scanLeft ? 0 : W - 1, end = scanLeft ? W : -1, step = scanLeft ? 1 : -1;
    for (let xx = start; xx !== end; xx += step) {
      const p = idx(xx, yy);
      if (sand[p] === -1) continue;
      let nx = xx;
      if (!grainAt(xx, yy + 1)) {
        nx = xx
      } else {
        const first = scanLeft ? -1 : 1, second = -first;
        if (!grainAt(xx + first, yy + 1))
          nx = xx + first;
        else if (!grainAt(xx + second, yy + 1))
          nx = xx + second;
        else
          continue
      }
      sand[idx(nx, yy + 1)] = sand[p];
      sand[p] = -1
    }
  }
  scanLeft = !scanLeft
}
function clearLayers() {
  for (let yy = H - 1; yy >= 0; yy--) {
    let filled = 0;
    for (let xx = 0; xx < W; xx++)
      if (sand[idx(xx, yy)] !== -1) filled++;
    if (filled >= W * .94) {
      for (let y2 = yy; y2 > 0; y2--)
        for (let xx = 0; xx < W; xx++) sand[idx(xx, y2)] = sand[idx(xx, y2 - 1)];
      for (let xx = 0; xx < W; xx++) sand[idx(xx, 0)] = -1;
      layers++;
      score += 100 * level;
      level = 1 + Math.floor(layers / 5);
      yy++
    }
  }
  el('score').textContent = score;
  el('layers').textContent = layers;
  el('level').textContent = level
}
function settle() {
  emitPiece();
  clearLayers();
  spawn()
}
function update(dt) {
  if (over) return;
  dropTimer += dt;
  if (dropTimer > 1 - level * .015) {
    dropTimer = 0;
    if (pieceFits(current, x, y + 1))
      y++;
    else
      settle()
  }
  for (let i = 0; i < 2; i++) updateSand()
}
function draw() {
  ctx.fillStyle = '#05070b';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  for (let yy = 0; yy < ROWS; yy++)
    for (let xx = 0; xx < COLS; xx++) {
      ctx.fillStyle = '#101b32';
      ctx.fillRect(xx * 40 + 15, yy * 40 + 15, 10, 10)
    }
  for (let yy = 0; yy < H; yy++)
    for (let xx = 0; xx < W; xx++) {
      const t = sand[idx(xx, yy)];
      if (t !== -1) {
        ctx.fillStyle = colors[t];
        ctx.fillRect(xx * CELL, yy * CELL, CELL, CELL)
      }
    }
  for (let i = 0; i < COLS; i++) {
    ctx.fillStyle = '#ef3d46';
    ctx.fillRect(i * 40 + 4, 3 * 40 + 17, 31, 5)
  }
  if (!over)
    for (const [a, b] of rotatedCells(current)) {
      ctx.fillStyle = colors[current];
      ctx.fillRect((x + a) * 40 + 3, (y + b) * 40 + 3, 34, 34)
    }
  if (over) {
    ctx.fillStyle = '#000c';
    ctx.fillRect(0, 300, 400, 120);
    ctx.fillStyle = '#fff';
    ctx.font = 'bold 25px system-ui';
    ctx.fillText('GAME OVER', 125, 355);
    ctx.font = '16px system-ui';
    ctx.fillText('按 R 重新开始', 145, 385)
  }
  requestAnimationFrame(draw)
}
function reset() {
  sand = new Int8Array(W * H);
  sand.fill(-1);
  current = null;
  next = null;
  x = 4;
  y = 0;
  rotation = 0;
  score = layers = 0;
  level = 1;
  dropTimer = 0;
  over = false;
  scoreSubmitted = false;
  el('saveStatus').textContent = '';
  el('finishGame').disabled = false;
  spawn();
  el('score').textContent = 0;
  el('layers').textContent = 0;
  el('level').textContent = 1
}
document.addEventListener('keydown', e => {
  if (['ArrowLeft', 'ArrowRight', 'ArrowDown', 'ArrowUp', ' '].includes(e.key)) e.preventDefault();
  if (e.key === 'r' || e.key === 'R') {
    reset();
    return
  }
  if (over) return;
  if (e.key === 'ArrowLeft' && pieceFits(current, x - 1, y)) x--;
  if (e.key === 'ArrowRight' && pieceFits(current, x + 1, y)) x++;
  if (e.key === 'ArrowDown' && pieceFits(current, x, y + 1)) y++;
  if (e.key === 'ArrowUp') {
    const r = (rotation + 1) % 4;
    if (pieceFits(current, x, y, r)) rotation = r
  }
  if (e.code === 'Space') {
    while (pieceFits(current, x, y + 1)) y++;
    settle()
  }
});
el('restart').onclick = reset;
el('finishGame').onclick = finishGame;
reset();
function loop(now) {
  const dt = Math.min((now - last) / 1000, .1);
  last = now;
  update(dt);
  requestAnimationFrame(loop)
}
requestAnimationFrame(loop);
draw();
