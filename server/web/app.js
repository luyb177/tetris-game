/* 首页交互入口：负责登录态展示、验证码发送、用户注册/登录和排行榜刷新。 */
const API_BASE = window.TETRIS_API_BASE || 'http://127.0.0.1:8080';
const $ = id => document.getElementById(id);
const message = t => $('formMessage').textContent = t;
const getUser = () => {
  try {
    return JSON.parse(localStorage.getItem('tetris_user') || 'null')
  } catch (e) {
    return null
  }
};
const isLoggedIn = () => Boolean(localStorage.getItem('tetris_token'));
function openModal(tab = 'login') {
  $('authModal').classList.remove('hidden');
  switchTab(tab)
}
function closeModal() {
  $('authModal').classList.add('hidden')
}
function switchTab(tab) {
  document.querySelectorAll('.tab').forEach(
      x => x.classList.toggle('active', x.dataset.tab === tab));
  $('loginForm').classList.toggle('hidden', tab !== 'login');
  $('registerForm').classList.toggle('hidden', tab !== 'register');
  message('')
}
function updateAuthUI() {
  const user = getUser(), logged = Boolean(user && isLoggedIn()), account = $('navLogin'),
        hero = $('heroPlay'), play = $('playNow');
  if (logged) {
    account.textContent = user.nickname || '我的账号';
    account.classList.add('logged-in');
    account.onclick = logout;
    hero.textContent = '继续游戏';
    $('heroHint').textContent = `你好，${user.nickname || '玩家'}，本次成绩会自动保存。`;
    $('playDescription').textContent = '登录状态已生效，游戏结束后成绩会自动上传到排行榜。';
    play.textContent = '开始并保存成绩'
  } else {
    account.textContent = '登录';
    account.classList.remove('logged-in');
    account.onclick = () => openModal();
    hero.textContent = '游客开始游戏';
    $('heroHint').textContent = '无需注册也可以游玩，登录后成绩将自动保存。';
    $('playDescription').textContent = '游客可以完整游玩；登录后成绩会自动上传到排行榜。';
    play.textContent = '进入游戏'
  }
}
function logout() {
  localStorage.removeItem('tetris_token');
  localStorage.removeItem('tetris_user');
  updateAuthUI();
  message('已退出登录')
}
async function api(path, options = {}) {
  const token = localStorage.getItem('tetris_token');
  options.headers = {
    ...(options.headers || {}),
    'Content-Type': 'application/json',
    ...(token ? {Authorization: `Bearer ${token}`} : {})
  };
  const r = await fetch(`${API_BASE}${path}`, options);
  let data;
  try {
    data = await r.json()
  } catch (e) {
    throw new Error('服务器返回了无效响应')
  }
  if (!r.ok) {
    throw new Error(data.error || `请求失败（${r.status}）`)
  }
  return data
}
async function loadRanking() {
  const box = $('rankingList'), status = $('rankingStatus'), button = $('refreshRanking');
  button.disabled = true;
  status.textContent = '正在刷新成绩…';
  try {
    const d = await api('/api/scores/ranking');
    const items = Array.isArray(d.items) ? d.items : [];
    box.innerHTML = items.length ?
        items
            .map(
                (x, i) => `<div class="rank"><strong>#${i + 1}</strong><span>${
                    x.nickname || '玩家'}</span><b>${Number(x.score) || 0} 分</b></div>`)
            .join('') :
        '<span class="muted">暂时还没有成绩，完成一局游戏后即可上榜。</span>';
    status.textContent = `共 ${items.length} 条成绩`
  } catch (e) {
    box.innerHTML = '<span class="muted">排行榜加载失败，请点击刷新重试。</span>';
    status.textContent = e.message
  } finally {
    button.disabled = false
  }
}
document.querySelectorAll('.tab').forEach(x => x.onclick = () => switchTab(x.dataset.tab));
$('registerBtn').onclick = () => openModal('register');
$('closeModal').onclick = closeModal;
$('authModal').onclick = e => {
  if (e.target.id === 'authModal') closeModal()
};
$('refreshRanking').onclick = loadRanking;
let sendCodeTimer = null, sendCodeBusy = false;
function startSendCodeCooldown(seconds = 60) {
  const button = $('sendCode');
  let left = seconds;
  sendCodeBusy = true;
  button.disabled = true;
  button.textContent = `重新发送(${left}s)`;
  clearInterval(sendCodeTimer);
  sendCodeTimer = setInterval(() => {
    left -= 1;
    if (left <= 0) {
      clearInterval(sendCodeTimer);
      sendCodeBusy = false;
      button.disabled = false;
      button.textContent = '发送验证码'
    } else
      button.textContent = `重新发送(${left}s)`
  }, 1000)
}
$('sendCode').onclick = async () => {
  const email = $('regEmail').value.trim();
  if (!email) return message('请先填写邮箱');
  if (sendCodeBusy) return;
  sendCodeBusy = true;
  $('sendCode').disabled = true;
  $('sendCode').textContent = '发送中…';
  try {
    const d = await api('/api/auth/send-code', {method: 'POST', body: JSON.stringify({email})});
    message(d.message || '验证码已发送');
    startSendCodeCooldown(Number(d.retryAfter) || 60)
  } catch (e) {
    sendCodeBusy = false;
    $('sendCode').disabled = false;
    $('sendCode').textContent = '发送验证码';
    message(`发送失败：${e.message}`)
  }
};
async function completeLogin(d, successText) {
  localStorage.setItem('tetris_token', d.token);
  localStorage.setItem(
      'tetris_user',
      JSON.stringify({nickname: d.nickname || d.name || '玩家', email: d.email || ''}));
  updateAuthUI();
  message(successText);
  setTimeout(() => {
    closeModal();
    window.location.href = '/game.html'
  }, 500)
}
$('loginForm').onsubmit = async e => {
  e.preventDefault();
  const button = e.submitter;
  button.disabled = true;
  try {
    const d = await api('/api/auth/login', {
      method: 'POST',
      body:
          JSON.stringify({email: $('loginEmail').value.trim(), password: $('loginPassword').value})
    });
    if (d.token)
      await completeLogin(d, '登录成功，正在进入游戏');
    else
      message(d.error || '登录失败')
  } catch (e) {
    message(`登录失败：${e.message}`)
  } finally {
    button.disabled = false
  }
};
$('registerForm').onsubmit = async e => {
  e.preventDefault();
  const button = e.submitter;
  button.disabled = true;
  try {
    const d = await api('/api/auth/register', {
      method: 'POST',
      body: JSON.stringify({
        email: $('regEmail').value.trim(),
        nickname: $('regName').value.trim(),
        password: $('regPassword').value,
        code: $('regCode').value.trim()
      })
    });
    if (d.token)
      await completeLogin(d, '注册成功，正在进入游戏');
    else
      message(d.error || '注册失败')
  } catch (e) {
    message(`注册失败：${e.message}`)
  } finally {
    button.disabled = false
  }
};
updateAuthUI();
loadRanking();
