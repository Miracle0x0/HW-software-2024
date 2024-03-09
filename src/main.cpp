#include <bits/stdc++.h>
using namespace std;

const int n = 200;            // 地图边界
const int ROBOT_NUM = 10;     // 机器人数量
const int BERTH_NUM = 10;     // 泊位数量
const int SHIP_NUM = 5;       // 轮船数量
const int FRAME_NUM = 15000;  // 总帧数（初赛为 15000）
const int N = 210;
const int MAX_K = 10;
const int MAX_EXIST_FRAME = 1000;

enum DIRECTIONS { RIGHT,
                  LEFT,
                  UP,
                  DOWN };

const int magic_directions[] = {-1, 0, 1, 0, -1};

// ***** 地图说明符号 *****
const char SPACE = '.';   // 空地
const char SEA = '*';     // 海洋
const char BORDER = '#';  // 障碍
const char ROBOT = 'A';   // 机器人
const char BERTH = 'B';   // 泊位

/**
 * @brief 将日志输出到文件 Debug.Log
 * 
 * @param format 
 * @param ... 
 */
inline void Log(const char *format, ...) {
    FILE *file = fopen("debug.log", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
}

/**
 * @brief 将划分后的图输出到文件 partition.txt，用于 python 脚本可视化
 * 
 * @param format 
 * @param ... 
 */
inline void Visualize(const char *format, ...) {
    FILE *file = fopen("partition.txt", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
}

#define RED_COLOR_START "\033[1;31m"
#define COLOR_END "\033[0m"

#define Assert(condition, message, ...)              \
    do {                                             \
        if (!(condition)) {                          \
            fprintf(stderr, message, ##__VA_ARGS__); \
            exit(EXIT_FAILURE);                      \
        }                                            \
    } while (0)
#define Debug(...)                    \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        Log(__VA_ARGS__);             \
    } while (0)
#define NOT_IMPL \
    fprintf(stderr, RED_COLOR_START "[TODO] please implement me!\n" COLOR_END)


struct Robot {
    // * shape: 1x1
    int x, y, goods;
    int status;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) : x(startX), y(startY) {}
} robot[ROBOT_NUM + 10];

//--------change 增加参数 value 和 队列goods_value
//--------todo: 机器人在泊位pull时候：1.增加泊位价值  2.货物价值加入队列
struct Berth {
    // * shape: 4x4
    int x, y;
    int transport_time;
    int loading_speed;
    
    int value = 0; // 泊位价值
    queue<int> goods_value; // 泊位上的货物价值（取货按照时间排序）

    Berth() {}
    Berth(int x, int y, int transport_time, int loading_speed) : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}
} berth[BERTH_NUM + 10];

//--------change 增加参数  value
// pos:-1 status:0 表示驶出泊位
// pos:-1 status:1 表示在虚拟点
struct Ship {
    // * shape: 2*4
    int num; // 已经装有的容量
    int pos;     // 泊位 ID，虚拟泊位为 -1
    int status;  // 状态：0 表示移动（运输）中，1 表示正常运行状态（即装货状态或运输完成状态），2 表示泊位外等待状态

    int value = 0; // 当前货物价值
} ship[10];

int money, ship_capacity, id;
char ch[N][N];
int gds[N][N];

char okk[100];
// ! Debug
char tmp[1000];

// TODO: 使用更高效的数据结构实现，需要支持增、删、改、查等功能
struct Good {
    int x, y, val;
    bool is_picked = false;
    Good() {}
    Good(int x, int y, int val) : x(x), y(y), val(val) {}

    void pick() { this->is_picked = true; }
} global_goods[MAX_K * MAX_EXIST_FRAME];

// * 机器人指令
int robot_move(int id, int direction) {
    printf("move %d %d\n", id, direction);
    return 0;
}
int robot_get(int id) {
    NOT_IMPL;
    return 0;
}
int robot_pull(int id) {
    NOT_IMPL;
    return 0;
}

// * 船指令
//--------change begin：补全
int ship_move(int id, int berth_id) {
    // 船id 去往 berth_id
    printf("ship %d %d\n", id, berth_id);
    return 0;
}
// change end

//--------change begin：补全
int ship_go(int id) {
    printf("go %d\n", id);
    return 0;
}
// change end

//--------change begin：函数新增
// 查询港口是否有船
// 返回1表示有船，0表示无船
int query_berth(int id) {
    for(int i=0; i<BERTH_NUM; i++){
        if(ship[i].pos == id){
            return 1;
        }
    }
    return 0;
}
//change end

//--------change begin：函数新增
int check_ship() {
    for(int i=0; i<SHIP_NUM; i++){
        if(ship[i].pos == -1 && ship[i].status == 0){
            // pos:-1 status:0 表示船在运输中
            // do nothing
        }
        else if(ship[i].pos == -1 && ship[i].status == 1){
            // pos:-1 status:1 表示船在虚拟点
            //        发送 ship 指令 
            // v1.0: 选择价值高且没有船的泊位 
            int ind=0, max_value=0;
            for(int ind_berth=0; ind_berth<BERTH_NUM; ind_berth++){
                // 价值高 且无船
                if(berth[ind_berth].value > max_value && query_berth(ind_berth) == 0){
                    ind = ind_berth;
                    max_value = berth[ind_berth].value;
                }
            }
            // 发送 ship 指令 前往泊位
            // 重设 船的价值和剩余容量
            ship[i].num = ship_capacity;
            ship[i].value = 0;
            ship_move(i, ind);
        }
        else if(ship[i].pos != -1){
            // pos:0-9 status:1 表示船在泊位 装货,此时可发出ship_go指令
            // 船先离港后装卸货物
            
            // v1.0: 装满，或者岸边没有货物 运送至虚拟点
            if(ship_capacity == ship[i].num  || berth[ship[i].pos].goods_value.empty()){
                ship_go(i);
            }
            // 正常情况下装货
            if(ship_capacity - ship[i].num > 0 ){
                // 装货
                // 装载loading_speed次
                for(int j=0; j<berth[ship[i].pos].loading_speed; j++){
                    // 无法继续装
                    if(ship_capacity == ship[i].num || berth[ship[i].pos].goods_value.empty()) break;
                    // 装货（队列中的货物价值）
                    int val = berth[ship[i].pos].goods_value.front();
                    berth[ship[i].pos].goods_value.pop();
                    berth[ship[i].pos].value -= val; // 泊位价值改变
                    // 容量, 价值改变
                    ship[i].num++;
                    ship[i].value += val;
                }
            }
        }
    }
    return 0;
}
// change end

// ! Debug
void show_ch() {
    Log("show ch\n");
    for (int i = 0; i < n; i++)
        Log("%s\n", ch[i]);
    Log("end ch\n");
}

void init_map() {

    memset(gds, -1, sizeof(gds));

    // * 加载地图
    for (int i = 0; i < n; i++) {
        scanf("%s", ch[i]);
    }
    // ! Debug
    // show_ch();
    // * 加载泊位 (10 lines, 5 columns)
    for (int i = 0; i < BERTH_NUM; i++) {
        int id;
        scanf("%d", &id);
        scanf("%d %d %d %d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    // * 轮船容积
    scanf("%d", &ship_capacity);

    // end of init
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

bool que_finish[BERTH_NUM];

// TODO: 子图划分，采用 BFS
void graph_partition() {
    Debug("[INIT] graph partition\n");
    // 从每个 BERTH 开始 BFS，遇到障碍或海洋则停止
    vector<queue<int>> ques(BERTH_NUM);
    for (int i = 0; i < BERTH_NUM; i++) {
        gds[berth[i].x][berth[i].y] = i;
        ques[i].push(berth[i].x * n + berth[i].y);
    }

    int finish_count = 0;
    while (finish_count < BERTH_NUM) {
        for (int i = 0; i < BERTH_NUM; i++) {
            if (ques[i].empty()) {
                if (!que_finish[i]) {
                    que_finish[i] = true;
                    finish_count++;
                }
                continue;
            }
            int cur = ques[i].front();
            ques[i].pop();
            int x = cur / n, y = cur % n;
            for (int j = 0; j < 4; j++) {
                int nxt_x = x + magic_directions[j], nxt_y = y + magic_directions[j + 1];
                if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || gds[nxt_x][nxt_y] != -1) continue;
                // 标记为已访问
                gds[nxt_x][nxt_y] = i;
                ques[i].push(nxt_x * n + nxt_y);
            }
        }
    }
    // ! Debug
    for (int i = 0; i < BERTH_NUM; i++) {
        Assert(ques[i].empty(), "ques[%d] should be empty", i);
    }
}

// ! Debug
void show_gds() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (gds[i][j] == -1)
                Visualize("x");
            else
                Visualize("%d", gds[i][j]);
        }
        Visualize("\n");
    }
}

int get_inputs() {
    int x, y, k, val;
    scanf("%d %d", &id, &money);  // 帧序号，当前金钱数
    scanf("%d", &k);              // 场上新增货物的数量 K
    // * K 行数据，每一行表示一个新增货物
    // * (横坐标，纵坐标，金额)
    for (int i = 0; i < k; i++) {
        scanf("%d %d %d", &x, &y, &val);
    }
    // * 10 行数据，每一行表示一个机器人
    // * (是否携带物品，横坐标，纵坐标，状态)
    for (int i = 0; i < ROBOT_NUM; i++) {
        int sts;
        scanf("%d %d %d %d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
    }
    // * 5 行数据，每一行表示一艘船
    // * (状态，泊位 ID)
    for (int i = 0; i < SHIP_NUM; i++) {
        scanf("%d %d\n", &ship[i].status, &ship[i].pos);
    }
    // end of inputs
    scanf("%s", okk);
    return id;
}

inline void random_move() {
    for (int i = 0; i < ROBOT_NUM; i++) robot_move(i, rand() % 4);
}

int process_inputs() {
    // TODO: ...
    // Debug("[TODO] process inputs\n");
    random_move();

    return 0;
}

// ***** 主程序逻辑 *****
int main() {

    init_map();

    graph_partition();

    show_gds();

    for (int frame = 1; frame <= FRAME_NUM; frame++) {
        // start of frame
        int id = get_inputs();

        // process_inputs();

        // end of frame
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
