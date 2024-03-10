#include <bits/stdc++.h>
using namespace std;

#include "include/debug.h"
#include "include/defines.h"
#include "include/robot.h"
#include "include/ship.h"


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
    Debug("berth order:\n");
    for (auto &i: order_of_berth)
        Debug("berth_id: %d | loading_speed: %d\n", i.second, i.first);
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
}

void global_graph_partition() {
    Debug("[INIT] global graph partition\n");
    // 从每个 BERTH 开始 BFS，遇到障碍或海洋则停止
    vector<queue<int>> ques(BERTH_NUM);
    // TODO: 考虑用多线程实现
    for (int b = 0; b < BERTH_NUM; b++) {

        global_dis[b][berth[b].x][berth[b].y] = 0;
        ques[b].push(berth[b].x * n + berth[b].y);

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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            min_dis[i][j] = dis[i][j];
    }
    for (int b = 0; b < BERTH_NUM; b++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (global_dis[b][i][j] != -1 && global_dis[b][i][j] < min_dis[i][j]) {
                    min_dis[i][j] = global_dis[b][i][j];
                    gds[i][j] = b;  // 修正分区结果
                }
            }
        }
    }  // end of berth loop
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
        if (!is_valid_good(x, y)) continue;
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

    execute_robot_instructions(frame);

    return 0;
}

// ***** 主程序逻辑 *****
int main() {

    srand(1037);  // * 固定随机种子

    init_map();

    graph_partition();
    // ! 暂时禁用，耗时太长
    // global_graph_partition();

    show_gds();
    // ! 暂时禁用，耗时太长
    // show_global_dis();

    for (int frame = 1; frame <= FRAME_NUM; frame++) {
        // start of frame
        int id = get_inputs(frame);

        process_inputs(frame);

        // end of frame
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
