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

struct Berth {
    // * shape: 4x4
    int x, y;
    int transport_time;
    int loading_speed;
    Berth() {}
    Berth(int x, int y, int transport_time, int loading_speed) : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}
} berth[BERTH_NUM + 10];

struct Ship {
    // * shape: 2*4
    int num;
    int pos;     // 泊位 ID，虚拟泊位为 -1
    int status;  // 状态：0 表示移动（运输）中，1 表示正常运行状态（即装货状态或运输完成状态），2 表示泊位外等待状态
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
int ship_move(int id, int berth_id) {
    NOT_IMPL;
    return 0;
}
int ship_go(int id) {
    NOT_IMPL;
    return 0;
}

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
