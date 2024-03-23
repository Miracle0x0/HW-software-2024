#pragma once
#include <bits/stdc++.h>
using namespace std;

// ===== header begin =====

// * 常量定义

const int n = 200;                 // 地图边界
const int ROBOT_NUM = 10;          // 机器人数量
const int BERTH_NUM = 10;          // 泊位数量
const int BERTH_SIZE = 4;          // 泊位大小
const int SHIP_NUM = 5;            // 轮船数量
const int FRAME_NUM = 15000;       // 总帧数（初赛为 15000）
const int N = n + 10;              // 地图边界（防越界）
const int MAX_K = 10;              // 每帧最多新增货物数量
const int MAX_EXIST_FRAME = 1000;  // 货物最长存在时间
const int pre_frame = 10000;       // 预热帧数 
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
    int target_area_id;     // 目标区域 ID
    int normal_assignment;  // 是否为正常调度（区分首次调度）
    int val_of_good;        // 携带货物的价值
    int moved_steps;        // 当前已经走过的路径长度
    int wait;               // 是否处于等待状态（即暂停一帧）
    int target;             // 0: 无目标 1: 货物 2: 泊位
    int mbx, mby;           // 目标坐标
    int mb_showup_frame;    // 目标货物出现的帧数
    int berth_id = -1;      // 该机器人当前的归属泊位 -1 表示无归属
    int change_able = 1;    // 是否可以改变目标
    Robot() {}
    Robot(int startX, int startY) : x(startX), y(startY) {}
} robot[ROBOT_NUM * 2];


struct Berth {
    // * shape: 4x4
    int x, y;
    int transport_time;
    int loading_speed;

    int tag = 0;             // 标记泊位是否可用 0: 不可用 1: 可用
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
int all_area_size = 0;
char ch[N][N];
int gds[N][N];
int dis[N][N];                       // BFS 得到的距离矩阵，代表当前位置到达各个泊位的距离
int global_dis[BERTH_NUM][N][N];     // 距离矩阵，保存每个点到达各个泊位的距离
int min_dis[N][N];                   // 最优距离矩阵，保存每个点到达各个泊位的最短距离
int area_size[BERTH_NUM];            // 每个泊位的分区面积
int neighbor[BERTH_NUM][BERTH_NUM];  // 泊位是否近邻
queue<int> next_berth;               // 下一个要去的泊位
int avaliable_berth = 0;             // 可用泊位数
char okk[100];

// * 辅助函数定义

/**
 * @brief 查询港口是否有船，返回 1 表示有船，0 表示无船
 * 
 * @param id 
 */
inline int query_berth(int id) {
    for (int i = 0; i < SHIP_NUM; i++)
        if (ship[i].pos == id) return 1;
    return 0;
}

/**
 * @brief 查询位置x,y处于几号泊位 -1表示不在泊位
 * 
 * @param x y
 */
inline int query_berth_id(int x, int y) {
    for (int i = 0; i < BERTH_NUM; i++) {
        if (berth[i].x <= x && x < berth[i].x + BERTH_SIZE &&
            berth[i].y <= y && y < berth[i].y + BERTH_SIZE) {
            return i;
        }
    }
    return -1;
}

// 货物的队列（每个泊位或分区维护一个优先队列）
priority_queue<pair<float, Good>> q_goods[BERTH_NUM];
// 备选货物队列
priority_queue<pair<float, Good>> q_goods_bak[BERTH_NUM];
// 按照装卸速度对泊位队列排序
vector<pair<int, int>> order_of_berth;

// TODO: 每个泊位对应区域能够分配到的最大机器人数量（和泊位大小和装载速度有关）
int max_robot_capacity[BERTH_NUM];
// TODO: 当前泊位对应区域分配到的机器人数量
int cur_robot_capacity[BERTH_NUM];
// TODO: 首次调度标记
int first_assignment_tag[BERTH_NUM];
int sum_value = 0;
int send_value = 0;
// ===== header end =====