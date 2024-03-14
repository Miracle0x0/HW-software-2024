#include <bits/stdc++.h>
using namespace std;

#include "include/debug.h"
#include "include/defines.h"
#include "include/path.h"
#include "include/robot.h"
#include "include/ship.h"

bool que_finish[BERTH_NUM];

// 子图划分，采用 BFS
void global_graph_partition() {
    Debug("[INIT] global graph partition\n");
    // 从每个 BERTH 开始 BFS，遇到障碍或海洋则停止
    vector<queue<int>> ques(BERTH_NUM);
    // TODO: 考虑用多线程实现

    for (int b = 0; b < BERTH_NUM; b++) {
        // global_dis[b][berth[b].x][berth[b].y] = 0;
        // ques[b].push(berth[b].x * n + berth[b].y);
        // 矩形泊位
        for (int dx = 0; dx < BERTH_SIZE; dx++) {
            for (int dy = 0; dy < BERTH_SIZE; dy++) {
                int nxt_x = berth[b].x + dx, nxt_y = berth[b].y + dy;
                // if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                // if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || global_dis[b][nxt_x][nxt_y] != -1) continue;
                // gds[nxt_x][nxt_y.y] = b; // todo ? goods initial pos in berth
                global_dis[b][nxt_x][nxt_y] = 0;
                ques[b].push((nxt_x) *n + (nxt_y));
            }
        }

        while (!ques[b].empty()) {
            int cur = ques[b].front();
            ques[b].pop();
            int x = cur / n, y = cur % n;
            for (int j = 0; j < 4; j++) {
                int nxt_x = x + magic_directions[j], nxt_y = y + magic_directions[j + 1];
                if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || global_dis[b][nxt_x][nxt_y] != -1) continue;
                // 标记为已访问
                if (ch[nxt_x][nxt_y] == BERTH)
                    global_dis[b][nxt_x][nxt_y] = global_dis[b][x][y];
                else
                    global_dis[b][nxt_x][nxt_y] = global_dis[b][x][y] + 1;
                ques[b].push(nxt_x * n + nxt_y);
            }
        }  // end of BFS search
        // ! Debug
        Assert(ques[b].empty(), "BFS of berth %d error!\n", b);
    }  // end of berth loop

    // 计算每个点到各个泊位的全局最短距离
    for (int b = 0; b < BERTH_NUM; b++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (global_dis[b][i][j] != -1 && (global_dis[b][i][j] < dis[i][j] || dis[i][j] == -1)) {
                    dis[i][j] = global_dis[b][i][j];
                    gds[i][j] = b;  // 修正分区结果
                }
            }
        }
    }  // end of berth loop
}

void init_map() {

    memset(gds, -1, sizeof(gds));
    memset(dis, -1, sizeof(dis));
    memset(global_dis, -1, sizeof(global_dis));

    // * 加载地图 (200 x 200 grids)
    for (int i = 0; i < n; i++) {
        scanf("%s", ch[i]);
    }
    // * 加载泊位 (10 lines, 5 columns)
    for (int i = 0; i < BERTH_NUM; i++) {
        int id;
        scanf("%d", &id);
        scanf("%d %d %d %d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
        // ? 将泊位按照装载速度排序
        order_of_berth.push_back({berth[id].loading_speed, id});
    }
    sort(order_of_berth.begin(), order_of_berth.end(), greater<pair<int, int>>());
    // ! Debug
    // Debug("berth order:\n");
    // for (auto &i: order_of_berth)
    //     Debug("berth_id: %d | loading_speed: %d\n", i.second, i.first);

    // * 轮船容积
    scanf("%d", &ship_capacity);
    scanf("%s", okk);

    // init graph
    global_graph_partition();

    // init robot
    for (int i = 0; i < ROBOT_NUM; i++) {
        robot[i].status = rns::ROBOT_WORKING;
        robot[i].target = rns::T_NONE;
    }
    // end of init
    printf("OK\n");
    fflush(stdout);
}

// ! Debug
void show_gds() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            (gds[i][j] == -1) ? Visualize("x") : Visualize("%d", gds[i][j]);
        }
        Visualize("\n");
    }
}

// ! Debug
void show_dis() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) Dis("%d ", dis[i][j]);
        Dis("\n");
    }
}

// ! Debug
void show_global_dis() {
    for (int b = 0; b < BERTH_NUM; b++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) GlobalDis(b, "%d ", global_dis[b][i][j]);
            GlobalDis(b, "\n");
        }
    }  // end of berth loop
}

int get_inputs(const int frame) {
    int x, y, k, val, id, money;
    scanf("%d %d", &id, &money);  // 帧序号，当前金钱数
    scanf("%d", &k);              // 场上新增货物的数量 K
    // * K 行数据，每一行表示一个新增货物
    // * (横坐标，纵坐标，金额)
    for (int i = 0; i < k; i++) {
        scanf("%d %d %d", &x, &y, &val);
        // TODO: 处理货物数据，抽象为函数
        // ? 处理货物数据
        float cost = cost_of_good(x, y, val);
        if (cost < 0.01) continue;  // TODO: 阈值可调
        if (!rns::is_valid_good(x, y)) continue;
        auto new_good = Good(x, y, val, frame);
        auto area_id = gds[x][y];
        q_goods[area_id].push({cost, new_good});
    }
    // * 10 行数据，每一行表示一个机器人
    // * (是否携带物品，横坐标，纵坐标，状态)
    for (int i = 0; i < ROBOT_NUM; i++) {
        int sts;
        scanf("%d %d %d %d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status);
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

int process_inputs(int frame) {
    // TODO: ...
    Debug("%05d F | process_inputs\n", frame);

    // process robot
    rns::execute_robot_instructions(frame);

    // process ship
    check_ship();

    return 0;
}

// ***** 主程序逻辑 *****
int main() {

    srand(1037);  // * 固定随机种子

    init_map();

    // get information of maps
    // show_gds();
    // show_global_dis();
    // show_dis();

    for (int frame = 1; frame <= FRAME_NUM; frame++) {
        // ! Debug
        Debug("===== Frame %05d start =====\n", frame);
        // start of frame
        int id = get_inputs(frame);

        process_inputs(frame);

        // end of frame
        puts("OK");
        fflush(stdout);

        // ! Debug
        Debug("===== Frame %05d end =====\n", frame);
    }

    return 0;
}
