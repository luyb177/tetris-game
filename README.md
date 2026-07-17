# NEON SAND TETRIS

一个基于浏览器的沙粒俄罗斯方块游戏平台，包含 HTML/CSS/JavaScript 前端、Drogon C++ HTTP 后端、MySQL 数据库和 SMTP 邮件验证码服务。

## 项目结构

```text
.
├── assets/                  # 项目架构图等静态资源
├── server/
│   ├── config/              # 配置模板；本地 app.yaml 不提交
│   ├── sql/schema.sql       # MySQL 建库建表脚本
│   ├── src/app/main.cpp     # 服务启动与连接循环
│   ├── src/controllers/      # HTTP 路由与请求分发
│   ├── src/data/             # MySQL 数据访问层
│   ├── src/services/         # SMTP 邮件服务层
│   ├── src/auth/             # Token 会话层
│   ├── src/config/           # 配置和通用工具
│   └── web/                  # 独立网页前端
├── CMakeLists.txt           # 后端构建入口
└── .gitignore
```

前后端分离：`server/web/` 是独立的 HTML/CSS/JavaScript 前端，Drogon 后端只提供 JSON API，负责请求分发、MySQL 数据访问和 SMTP 邮件发送。

```text
浏览器前端 ── JSON / HTTP ──> Drogon Controller ──> Service ──> Repository ──> MySQL
                                  └──────> SMTP 邮件服务
```

## 编译并运行后端

```bash
cp server/config/app.example.yaml server/config/app.yaml
# 编辑 server/config/app.yaml，填写 MySQL 和 SMTP 配置
mysql -u root -p < server/sql/schema.sql
cmake -S . -B build
cmake --build build --target tetris_server -j
./build/bin/tetris_server server/config/app.yaml
```

## 独立运行前端

后端启动后，另开一个终端运行静态文件服务：

```bash
python3 -m http.server 5173 --directory server/web
```

然后访问 `http://127.0.0.1:5173/`。前端默认请求
`http://127.0.0.1:8080` 的后端 API；如需修改地址，可在页面脚本加载前设置
`window.TETRIS_API_BASE`。

macOS/Homebrew 如果之前配置过失败的 CMake 缓存，先执行 `rm -rf build` 再重新配置。

真实配置文件已被 `.gitignore` 忽略，不要提交邮箱密码、SMTP 授权码或数据库密码。`email.enabled: false` 时验证码会输出到服务端控制台，便于本地开发。

后端不再托管网页文件，前端和后端可以分别部署在不同端口或不同服务器上。

后端分层说明：

- `controllers/`：Drogon 请求入口、JSON 解析和响应转换。
- `services/`：注册、登录、验证码、成绩和邮件等业务逻辑。
- `repositories/`：用户、验证码、成绩数据的 SQL 访问。
- `data/`：MySQL 连接、查询和更新。
- `auth/`：Token 会话管理。

## API

```text
POST /api/auth/send-code   发送邮箱验证码
POST /api/auth/register    邮箱验证码注册
POST /api/auth/login       登录并返回 Token
POST /api/scores           登录用户提交分数
GET  /api/scores/ranking   获取排行榜
```

提交分数需要 `Authorization: Bearer <token>`。没有 Token 的游客可以正常游玩，但提交分数会返回 `401`。

## 安全说明

当前后端定位为课程项目和本地开发版本；正式部署前应增加 HTTPS、请求体校验、限流、安全密码哈希和持久化 Token。

## 提交前检查

请勿提交 `server/config/app.yaml`、`.env`、数据库导出文件、日志或任何包含真实凭据的文件。提交前可执行：

```bash
git status --short
git diff --cached --check
git ls-files | rg -i '(\.env|\.pem|\.key|credential|secret|dump|backup)'
```

其中最后一条命令应只显示你明确确认可以公开的文件。
