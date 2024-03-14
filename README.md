# 华为软件精英挑战赛 2024 —— 武长赛区

- `maps` 存放地图（每日更新）
- `pdfs` 存放说明文档（定期同步官网论坛）
- `replay` 存放回放文件（不同步）
- `sdk` 存放初始代码
- `src` 存放主代码
  - `include` 存放头文件
  - `CMakeLists.txt` 用于项目构建

## 初赛

### 0. 环境配置

- 一键脚本: [setup_env.sh](./setup_env.sh)

```shell
sudo apt install gcc g++ cmake make clang-format
```

### 1. 项目构建

参考 [Makefile](./Makefile)。

- `make`: 编译 `main.cpp`。
- `make test`: 调用判题器，日志存放在 `debug.log` 中。
- `make clean`: 清除文件。

### 2. 分区可视化

- `partition_visualize.py`: 可视化分区情况，每种颜色代表一个泊位划分出的区域。
- `distance_visualize.py`: 可视化分区内各点到泊位的距离，灰度图，颜色越深代表距离越远。
- `merge_header.py`: 头文件合并脚本，由 [Makefile](./Makefile) 调用。合并得到的 `main.cpp` 位于 `tmp` 目录下。

### 3. 调试说明

`include/debug.h` 中提供了多种调试日志输出，可按需使用。如果需要启用日志，在 `src/main.cpp` 中 `using namespace std;` 行之后，第一个 `"#include/xxx.h"` 行之前加入

```cpp
#define LOCAL_DEBUG
```

即可。提交代码前，记得删除这行宏定义。

- `Debug` 为通用调试，将调试信息输出到 `debug.log` 文件。
- `debug_robot` 为调试机器人相关代码专用，将调试信息输出到 `debug_robot.log` 文件。
- `debug_ship` 为调试轮船相关代码专用，将调试信息输出到 `debug_ship.log` 文件。
