#include <bits/stdc++.h>
using namespace std;

const int n = 200;            // 地图边界
const int ROBOT_NUM = 10;     // 机器人数量
const int BERTH_NUM = 10;     // 泊位数量
const int SHIP_NUM = 5;       // 轮船数量
const int FRAME_NUM = 15000;  // 总帧数（初赛为 15000）
const int N = 210;

enum { RIGHT,
       LEFT,
       UP,
       DOWN };

#define DEBUG_LOG(x) fprintf(stderr, "%s\n", x)

// * 机器人指令
#define robot_move(id, direction) printf("move %d %d\n", id, direction)
// TODO: finish get instruction for robot
#define robot_get(id) \
    printf("")
// TODO: finish pull instruction for robot
#define robot_pull(id) \
    printf("")

// * 船指令
// TODO: finish ship instruction for ship
#define ship_move(id, berth_id) \
    printf("ship %d %d\n", id, berth_id)
// TODO: finish go instruction for ship
#define ship_go(id) \
    printf("")

// TODO: 判断机器人可否使用，即能否离开障碍区域到达泊位
// not implemented yet

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

struct Boat {
    // * shape: 2*4
    int num;
    int pos;     // 泊位 ID，虚拟泊位为 -1
    int status;  // 状态：0 表示移动（运输）中，1 表示正常运行状态（即装货状态或运输完成状态），2 表示泊位外等待状态
} boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];

char okk[100];

void init_map() {
    for (int i = 1; i <= n; i++)
        scanf("%s", ch[i] + 1);
    for (int i = 0; i < BERTH_NUM; i++) {
        int id;
        scanf("%d", &id);
        scanf("%d %d %d %d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    // * OK
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int get_inputs() {
    scanf("%d %d", &id, &money);  // 帧序号，当前金钱数
    int num;                      // 场上新增货物的数量 K
    scanf("%d", &num);
    // * K 行数据，每一行表示一个新增货物
    // * (横坐标，纵坐标，金额)
    for (int i = 1; i <= num; i++) {
        int x, y, val;
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
    for (int i = 0; i < SHIP_NUM; i++)
        scanf("%d %d\n", &boat[i].status, &boat[i].pos);
    // * OK
    scanf("%s", okk);
    return id;
}

int main() {
    init_map();
    for (int zhen = 1; zhen <= FRAME_NUM; zhen++) {
        int id = get_inputs();
        for (int i = 0; i < ROBOT_NUM; i++) {
            robot_move(i, rand() % 4);
        }
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
