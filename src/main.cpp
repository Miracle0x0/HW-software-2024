#include <bits/stdc++.h>
using namespace std;

const int n = 200;         // 地图边界
const int ROBOT_NUM = 10;  // 机器人数量
const int BERTH_NUM = 10;  // 泊位数量
const int SHIP_NUM = 5;    // 轮船数量
const int N = 210;

// 使用 stderr 将日志输出到控制台
#define DEBUG_LOG(x) fprintf(stderr, "%s\n", x);

struct Robot {
    // * shape: 1x1
    int x, y, goods;
    int status;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
} robot[ROBOT_NUM + 10];

struct Berth {
    // * shape: 4x4
    int x;
    int y;
    int transport_time;
    int loading_speed;
    Berth() {}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this->x = x;
        this->y = y;
        this->transport_time = transport_time;
        this->loading_speed = loading_speed;
    }
} berth[BERTH_NUM + 10];

struct Boat {
    // * shape: 2*4
    int num, pos, status;
} boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];

void Init() {
    for (int i = 1; i <= n; i++)
        scanf("%s", ch[i] + 1);
    for (int i = 0; i < BERTH_NUM; i++) {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int Input() {
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    for (int i = 1; i <= num; i++) {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
    }
    for (int i = 0; i < ROBOT_NUM; i++) {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
    }
    for (int i = 0; i < 5; i++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    char okk[100];
    scanf("%s", okk);
    return id;
}

int main() {
    Init();
    for (int zhen = 1; zhen <= 15000; zhen++) {
        int id = Input();
        for (int i = 0; i < ROBOT_NUM; i++) {
            printf("move %d %d\n", i, rand() % 4);
            // DEBUG_LOG("测试");
        }
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
