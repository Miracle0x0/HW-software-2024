#pragma once
#include <bits/stdc++.h>
using namespace std;

// ===== header begin =====

// * 常量定义

const int n = 200;                 // 地图边界
const int ROBOT_NUM = 10;          // 机器人数量
const int BERTH_NUM = 10;          // 泊位数量
const int SHIP_NUM = 5;            // 轮船数量
const int FRAME_NUM = 15000;       // 总帧数（初赛为 15000）
const int N = n + 10;              // 地图边界（防越界）
const int MAX_K = 10;              // 每帧最多新增货物数量
const int MAX_EXIST_FRAME = 1000;  // 货物最长存在时间

const int magic_directions[] = {-1, 0, 1, 0, -1};

// ***** 地图说明符号 *****
const char SPACE = '.';   // 空地
const char SEA = '*';     // 海洋
const char BORDER = '#';  // 障碍
const char ROBOT = 'A';   // 机器人
const char BERTH = 'B';   // 泊位

// * 结构体定义

struct Robot {
    // * shape: 1x1
    int x, y, goods;
    int status;
    int target;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) : x(startX), y(startY) {}
} robot[ROBOT_NUM * 2];

//--------change 增加参数 value 和 队列goods_value
//--------todo: 机器人在泊位pull时候：1.增加泊位价值  2.货物价值加入队列
struct Berth {
    // * shape: 4x4
    int x, y;
    int transport_time;
    int loading_speed;

    int value = 0;           // 泊位价值
    queue<int> goods_value;  // 泊位上的货物价值（取货按照时间排序）

    Berth() {}
    Berth(int x, int y, int transport_time, int loading_speed) : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}
} berth[BERTH_NUM * 2];

//--------change 增加参数  value
// pos:-1 status:0 表示驶出泊位
// pos:-1 status:1 表示在虚拟点
struct Ship {
    // * shape: 2*4
    int num;     // 已经装有的容量
    int pos;     // 泊位 ID，虚拟泊位为 -1
    int status;  // 状态：0 表示移动（运输）中，1 表示正常运行状态（即装货状态或运输完成状态），2 表示泊位外等待状态

    int value = 0;  // 当前货物价值
} ship[SHIP_NUM * 2];

// TODO: 使用更高效的数据结构实现，需要支持增、删、改、查等功能
struct Good {
    int x, y, val;
    int showup_frame;
    Good() {}
    Good(int x, int y, int val, int showup_frame) : x(x), y(y), val(val), showup_frame(showup_frame) {}

    bool operator<(const Good &g) const {
        if (val == g.val) return showup_frame > g.showup_frame;
        return val < g.val;
    }
} good[MAX_K * MAX_EXIST_FRAME];

// * 变量定义

int ship_capacity;
char ch[N][N];
int gds[N][N];
int dis[N][N];                    // BFS 得到的距离矩阵，代表当前位置到达各个泊位的距离
int global_dis[BERTH_NUM][N][N];  // 距离矩阵，保存每个点到达各个泊位的距离
int min_dis[N][N];                // 最优距离矩阵，保存每个点到达各个泊位的最短距离

char okk[100];

// * 辅助函数定义

inline float cost_of_good(int x, int y, int val) {
    return ((float) val) / dis[x][y];
}

// 货物的队列（每个泊位或分区维护一个优先队列）
priority_queue<pair<float, Good>> q_goods[BERTH_NUM];
// 按照装卸速度对泊位队列排序
vector<pair<int, int>> order_of_berth;

// ===== header end =====