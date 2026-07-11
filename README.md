# Neon Tetris

一个使用 C++17 + SFML 制作的霓虹风俄罗斯方块。

## 构建运行

需要 CMake 3.16+ 和可用的 C++ 编译器。SFML 由 CMake 自动下载：

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/neon_tetris
```

## 操作

- `←` / `→`：左右移动
- `↓`：加速下落
- `↑`：旋转
- `Space`：直接落到底部
- `R`：重新开始

## 目录结构

```text
include/       公共头文件
src/           游戏入口、逻辑、棋盘与渲染实现
CMakeLists.txt 构建配置
```
