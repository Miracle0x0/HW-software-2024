#include <bits/stdc++.h>
using namespace std;

// #define LOCAL_DEBUG

#include "include/debug.h"
#include "include/defines.h"
#include "include/good.h"
#include "include/path.h"
#include "include/robot.h"
#include "include/ship.h"

bool que_finish[BERTH_NUM];

// ! Debug
void show_gds() {
#ifdef LOCAL_DEBUG
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            (gds[i][j] == -1) ? Visualize("x") : Visualize("%d", gds[i][j]);
        }
        Visualize("\n");
    }
#endif
}

// ! Debug
void show_dis() {
#ifdef LOCAL_DEBUG
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) Dis("%d ", dis[i][j]);
        Dis("\n");
    }
#endif
}

// ! Debug
void show_global_dis() {
#ifdef LOCAL_DEBUG
    for (int b = 0; b < BERTH_NUM; b++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) GlobalDis(b, "%d ", global_dis[b][i][j]);
            GlobalDis(b, "\n");
        }
    }  // end of berth loop
#endif
}
// 子图划分，采用 BFS
void global_graph_partition() {
    // Debug("[INIT] global graph partition\n");
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
                ques[b].push(pns::pos_encode(nxt_x, nxt_y));
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
                if (ch[nxt_x][nxt_y] == BERTH && query_berth_id(nxt_x, nxt_y) == b)
                    global_dis[b][nxt_x][nxt_y] = global_dis[b][x][y];
                else
                    global_dis[b][nxt_x][nxt_y] = global_dis[b][x][y] + 1;
                ques[b].push(pns::pos_encode(nxt_x, nxt_y));
            }
        }  // end of BFS search
        // ! Debug
        // Assert(ques[b].empty(), "BFS of berth %d error!\n", b);
        if (ques[b].empty()) {
            Debug("BFS of berth %d finished\n", b);
        }
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
    //计算分区的面积和邻接关系
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (gds[i][j] != -1) area_size[gds[i][j]]++;
            for (int k = 1; k < 3; k++) {
                int nxt_x = i + magic_directions[k], nxt_y = j + magic_directions[k + 1];
                if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || ch[i][j] == SEA || ch[i][j] == BORDER) continue;
                if (gds[i][j] != gds[nxt_x][nxt_y]) {
                    neighbor[gds[i][j]][gds[nxt_x][nxt_y]] = 1;
                    neighbor[gds[nxt_x][nxt_y]][gds[i][j]] = 1;
                }
            }
        }
    }

    for (int b = 0; b < BERTH_NUM; b++) {
        all_area_size += area_size[b];
        Debug("area_size of %d is %d\n", b, area_size[b]);
    }
    Debug("all_area_size is %d \n", all_area_size);
    int average_area_size = all_area_size / BERTH_NUM;
    int flag = 1;
    while (flag == 1) {
        //分区的合并，把面积小于平均值的地区合并至面积最小的邻接分区
        flag = 0;
        for (int b = 0; b < BERTH_NUM; b++) {
            if (area_size[b] < average_area_size) {
                int min_size = n * n;
                int min_beath = b;
                for (int bn = 0; bn < BERTH_NUM; bn++) {
                    if (bn != b && neighbor[b][bn] == 1) {
                        if (area_size[bn] < min_size) {
                            min_size = area_size[bn];
                            min_beath = bn;
                        }
                    }
                }
                if (min_beath != b) {
                    flag = 1;
                    // some changes here
                    // int min_beath_new = berth[min_beath].loading_speed >= berth[b].loading_speed ? min_beath : b;
                    // int b_new = berth[min_beath].loading_speed >= berth[b].loading_speed ? b : min_beath;

                    int min_beath_new = sns::get_hole_time(min_beath) >= sns::get_hole_time(b) ? min_beath : b;
                    int b_new = sns::get_hole_time(min_beath) >= sns::get_hole_time(b) ? b : min_beath;
                    area_size[min_beath_new] += area_size[b_new];
                    area_size[b_new] = 0;
                    for (int i = 0; i < n; i++) {
                        for (int j = 0; j < n; j++) {
                            if (gds[i][j] == b_new) {
                                gds[i][j] = min_beath_new;
                            }
                        }
                    }
                    for (int b_neighbor = 0; b_neighbor < BERTH_NUM; b_neighbor++) {
                        if (neighbor[b_new][b_neighbor] == 1 && b_neighbor != b_new) {
                            neighbor[min_beath_new][b_neighbor] = 1;
                            neighbor[b_neighbor][min_beath_new] = 1;
                            neighbor[b_new][b_neighbor] = 0;
                            neighbor[b_neighbor][b_new] = 0;
                        }
                    }
                }
            }
        }
    }

    //重置dis，为重新进行bfs作准备
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dis[i][j] = -1;
        }
    }
    //重新bfs计算dis
    for (int b = 0; b < BERTH_NUM; b++) {
        // global_dis[b][berth[b].x][berth[b].y] = 0;
        // ques[b].push(berth[b].x * n + berth[b].y);
        // 矩形泊位
        if (area_size[b] != 0) {
            for (int dx = 0; dx < BERTH_SIZE; dx++) {
                for (int dy = 0; dy < BERTH_SIZE; dy++) {
                    int nxt_x = berth[b].x + dx, nxt_y = berth[b].y + dy;
                    // if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                    // if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || global_dis[b][nxt_x][nxt_y] != -1) continue;
                    // gds[nxt_x][nxt_y.y] = b; // todo ? goods initial pos in berth
                    dis[nxt_x][nxt_y] = 0;
                    ques[b].push(pns::pos_encode(nxt_x, nxt_y));
                }
            }
            while (!ques[b].empty()) {
                int cur = ques[b].front();
                ques[b].pop();
                int x = cur / n, y = cur % n;
                for (int j = 0; j < 4; j++) {
                    int nxt_x = x + magic_directions[j], nxt_y = y + magic_directions[j + 1];
                    if (nxt_x < 0 || nxt_x >= n || nxt_y < 0 || nxt_y >= n) continue;
                    if (ch[nxt_x][nxt_y] == SEA || ch[nxt_x][nxt_y] == BORDER || dis[nxt_x][nxt_y] != -1 || gds[nxt_x][nxt_y] != b) continue;
                    // 标记为已访问
                    if (ch[nxt_x][nxt_y] == BERTH && query_berth_id(nxt_x, nxt_y) == b)
                        dis[nxt_x][nxt_y] = dis[x][y];
                    else
                        dis[nxt_x][nxt_y] = dis[x][y] + 1;
                    ques[b].push(pns::pos_encode(nxt_x, nxt_y));
                }
            }  // end of BFS search
            // ! Debug
            // Assert(ques[b].empty(), "BFS of berth %d error!\n", b);
            if (ques[b].empty()) {
                Debug("BFS of berth %d finished\n", b);
            }
        }
    }  // end of berth loop
    show_dis();
    show_gds();
    // ! Debug
    Debug("After merge:\n");
    for (int b = 0; b < BERTH_NUM; b++) {
        Debug("area_size of %d is %d\n", b, area_size[b]);
    }
}

// 标记可用的泊位
void check_berth_tag() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            berth[gds[i][j]].tag = 1;
        }
    }
    for (int i = 0; i < BERTH_NUM; i++) {
        Debug("berth %d tag %d\n", i, berth[i].tag);
    }
}

// 分配泊位和机器人
void assign_berth_and_robot() {
    // * 泊位的总时间排序
    for (int i = 0; i < BERTH_NUM; i++) {
        if (berth[i].tag)
            order_of_berth.push_back({sns::get_hole_time(i), i});
    }
    // sort(order_of_berth.begin(), order_of_berth.end(), greater<pair<int, int>>());
    sort(order_of_berth.begin(), order_of_berth.end());

    // ! Debug
    Debug("berth order of loading_speed:\n");
    for (auto &i: order_of_berth)
        Debug("berth_id: %d | loading_speed: %d\n", i.second, i.first);


    // * 设置能够调度的机器人数量
    int assignable_robot_num = ROBOT_NUM;
    // ! Debug
    Debug("assignable_robot_num = %d\n", assignable_robot_num);

    // TODO: 设置每个泊位对应区域的最多分配到的机器人数量
    // TODO: 目前仅考虑区域的面积大小
    int remain_assignable_robot_num = assignable_robot_num;
    for (int i = 0; i < BERTH_NUM; i++) {
        if (area_size[i] == 0) continue;
        max_robot_capacity[i] = int((float(area_size[i] * assignable_robot_num) / float(all_area_size)));
        remain_assignable_robot_num -= max_robot_capacity[i];
    }
    // ! Debug
    Debug("After first assignment:\n");
    for (int i = 0; i < BERTH_NUM; i++) {
        Debug("max_robot_capacity[%d] = %d\n", i, max_robot_capacity[i]);
    }
    // TODO: 如果还有没有分配的机器人，按照装载速度分配
    while (remain_assignable_robot_num != 0) {
        for (auto &it: order_of_berth) {
            int bid = it.second;
            if (remain_assignable_robot_num == 0) break;
            if (area_size[bid] == 0) continue;
            max_robot_capacity[bid]++;
            remain_assignable_robot_num--;
        }
    }
    // ! Debug
    Assert(remain_assignable_robot_num == 0, "Error: remain_assignable_robot_num = %d\n", remain_assignable_robot_num);
    // ! Debug
    Debug("Final assignment:\n");
    for (int i = 0; i < BERTH_NUM; i++) {
        Debug("max_robot_capacity[%d] = %d\n", i, max_robot_capacity[i]);
    }
}
void init_map() {

    memset(gds, -1, sizeof(gds));
    memset(dis, -1, sizeof(dis));
    memset(global_dis, -1, sizeof(global_dis));
    memset(area_size, 0, sizeof(area_size));
    memset(neighbor, 0, sizeof(neighbor));

    // * 加载地图 (200 x 200 grids)
    for (int i = 0; i < n; i++) {
        scanf("%s", ch[i]);
    }
    // * 加载泊位 (10 lines, 5 columns)
    for (int i = 0; i < BERTH_NUM; i++) {
        int id;
        scanf("%d", &id);
        scanf("%d %d %d %d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }

    // * 轮船容积
    scanf("%d", &ship_capacity);
    scanf("%s", okk);

    // init graph
    global_graph_partition();
    // init berth tag
    check_berth_tag();
    // 分配机器人给泊位
    assign_berth_and_robot();

    // init robot
    for (int i = 0; i < ROBOT_NUM; i++) {
        robot[i].status = rns::ROBOT_WORKING;
        robot[i].target = rns::T_NONE;
    }

    // show gds or dis
    // show_gds();

    // end of init
    printf("OK\n");
    fflush(stdout);
}

int get_inputs(const int frame) {
    int x, y, k, val, id, money;
    scanf("%d %d", &id, &money);  // 帧序号，当前金钱数
    // ! Debug
    Debug("frame %05d | id %d | money %d\n", frame, id, money);
    scanf("%d", &k);  // 场上新增货物的数量 K
    // * K 行数据，每一行表示一个新增货物
    // * (横坐标，纵坐标，金额)
    for (int i = 0; i < k; i++) {
        scanf("%d %d %d", &x, &y, &val);
        // ? 处理货物数据
        gns::append_good(frame, x, y, val);
    }
    // * 10 行数据，每一行表示一个机器人
    // * (是否携带物品，横坐标，纵坐标，状态)
    for (int i = 0; i < ROBOT_NUM; i++) {
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
    // Debug("%05d F | process_inputs\n", frame);

    // process robot
    rns::execute_robot_instructions(frame);

    // process ship
    sns::check_ship(frame);

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

        // * 修正实际帧序号
        if (frame != id) {
            // ! Debug
            Debug("[WARNING] frameskip | frame %d to %d\n", frame, id);
            frame = id;
        }

        process_inputs(frame);

        // end of frame
        puts("OK");
        fflush(stdout);

        // ! Debug
        Debug("===== Frame %05d end =====\n", frame);
    }
    Debug("sum val %d send val %d\n", sum_value, send_value);
    return 0;
}
