#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "debug.h"
#include "defines.h"
#include "good.h"
#include "path.h"

// ===== header begin =====

// ***** 机器人状态参数 *****

namespace rns {
    enum ROBOT_DIRECTIONS {
        RIGHT,  // 右
        LEFT,   // 左
        UP,     // 上
        DOWN    // 下
    };

    enum ROBOT_STATUS {
        ROBOT_IDLE,    // 0: 恢复状态
        ROBOT_WORKING  // 1: 正常运行状态
    };

    enum ROBOT_TARTET {
        T_NONE,  // 0: 无目标
        T_GOOD,  // 1: 货物
        T_BERTH  // 2: 泊位
    };

    enum ROBOT_CRASH {
        CRASH_NONE,      // 0: 无冲突
        CRASH_HAPPENED,  // 1: 发生冲突
    };

    // ***** 机器人指令 *****

    // move
    inline int robot_move(int id, int direction) {
        printf("move %d %d\n", id, direction);
        return 0;
    }
    // get
    inline int robot_get(int id) {
        printf("get %d\n", id);
        // ! Debug
        debug_robot("[GET] robot %d get val %d at (%d, %d)\n", id, robot[id].val_of_good, robot[id].x, robot[id].y);
        return 0;
    }
    // pull
    inline int robot_pull(int id) {
        printf("pull %d\n", id);
        // ! Debug
        debug_robot("[PULL] robot %d pull val %d at (%d, %d)\n", id, robot[id].val_of_good, robot[id].x, robot[id].y);
        robot[id].val_of_good = 0;
        return 0;
    }

    /**
    * @brief 机器人寻路算法，设计从机器人当前位置 (x, y) 到目标位置 (mbx, mby) 的可行路径
    * 
    * @param rid 
    * @return int 
    */
    inline int set_move_path(const int rid) {
        // ! Debug
        debug_robot("[ROBOT] set move path for robot %d\n", rid);
        int start_pos = pns::pos_encode(robot[rid].x, robot[rid].y);
        int end_pos = pns::pos_encode(robot[rid].mbx, robot[rid].mby);
        bool planning_res;
        // TODO: 区分首次调度和后续调度
        if (robot[rid].normal_assignment == 0) {
            // 首次调度，使用全局 BFS 算法
            planning_res = pns::path_planning_bfs_strict(rid, start_pos, end_pos, false);
            if (!planning_res) {
                // ! Debug
                debug_robot("[ROBOT] strict path planning failed for robot %d\n", rid);
                planning_res = pns::path_planning_bfs(rid, start_pos, end_pos, false);
            }
            robot[rid].normal_assignment++;
        } else {
            // 后续调度，使用区域内 BFS 算法
            planning_res = pns::path_planning_bfs_strict(rid, start_pos, end_pos);
            if (!planning_res) {
                // ! Debug
                debug_robot("[ROBOT] strict path planning failed for robot %d\n", rid);
                planning_res = pns::path_planning_bfs(rid, start_pos, end_pos);
            }
        }
        if (!planning_res) {
            // ! Debug
            debug_robot("[ROBOT] path planning failed for robot %d\n", rid);
            return -1;
        }
        pns::skip_first(rid);  // 跳过首次移动，因为第一个点是机器人当前位置
        return 0;
    }

    /**
     * @brief 设置机器人取货后返回泊位的路径，按照“下山”的方式返回，保证路径最短且唯一
     * 
     * @param rid 
     * @return int 
     */
    inline int set_berth_path(const int rid) {
        // ! Debug
        debug_robot("[ROBOT] set berth path for robot %d\n", rid);
        // TODO: ...
        bool planning_res = pns::path_planning_downhill(rid);
        if (!planning_res) {
            // ! Debug
            debug_robot("[ROBOT] path planning failed for robot %d\n", rid);
            return -1;
        }
        pns::skip_first(rid);  // 跳过首次移动，因为第一个点是机器人当前位置
        return 0;
    }

    inline int set_berth_path_pre(const int rid, const int pos) {
        // ! Debug
        debug_robot("[ROBOT] set berth path for robot %d\n", rid);

        bool planning_res = pns::path_planning_downhill_pre(rid, pos);
        if (!planning_res) {
            // ! Debug
            debug_robot("[ROBOT] path planning pre failed for robot %d\n", rid);
            return -1;
        }
        pns::skip_first(rid);
        return 0;
    }

    /**
     * @brief 机器人 rid 的移动方向
     * 
     * @param rid 
     * @return int 
     */
    inline int moving_dir_of(const int rid) {
        int nxt_pos = pns::next_pos_of(rid);
        int cur_pos = pns::pos_encode(robot[rid].x, robot[rid].y);
        switch (nxt_pos - cur_pos) {
            case n:  // 下移
                return ROBOT_DIRECTIONS::DOWN;
            case -n:  // 上移
                return ROBOT_DIRECTIONS::UP;
            case 1:  // 右移
                return ROBOT_DIRECTIONS::RIGHT;
            case -1:  // 左移
                return ROBOT_DIRECTIONS::LEFT;
            default:
                return -1;
        }
    }

    inline int crash_detection(const int rid) {
        int cur_nxt_pos = pns::next_pos_of(rid);  // 当前机器人的下一步位置
        int cur_pos = pns::pos_encode(robot[rid].x, robot[rid].y);
        int cur_x = robot[rid].x, cur_y = robot[rid].y;

        int nxt_pos;
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (i == rid) continue;
            if (robot[i].status == rns::ROBOT_IDLE || robot[i].wait) {
                nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
            } else {
                nxt_pos = pns::next_pos_of(i);
                if (nxt_pos == -1)
                    nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
            }
            if (cur_nxt_pos == nxt_pos || cur_nxt_pos == pns::pos_encode(robot[i].x, robot[i].y)) {
                // ! Debug
                // debug_robot("[CRASH] Crash detected for robot %d at (%d, %d)\n", rid, robot[rid].x, robot[rid].y);
                return CRASH_HAPPENED;
            }
        }

        return CRASH_NONE;
    }

    /**
     * @brief 机器人 rid 的冲突修正
     * 
     * @param rid 
     * @return int 
     */
    inline int crash_fix(const int rid, const vector<int> &crash_fix_order, const int cur_i) {
        int cur_nxt_pos = pns::next_pos_of(rid);  // 当前机器人的下一步位置
        int cur_pos = pns::pos_encode(robot[rid].x, robot[rid].y);
        int cur_x = robot[rid].x, cur_y = robot[rid].y;

        // * 不能走的位置，包括障碍物位置、id 小于自己的机器人的下一步位置和 id 大于自己的机器人的可能位置
        set<int> invalid_pos_masking;
        // * 1. 障碍物位置
        for (int i = 0; i < 4; i++) {
            int x = cur_x + magic_directions[i], y = cur_y + magic_directions[i + 1];
            if (!pns::valid_pos(x, y)) invalid_pos_masking.insert(pns::pos_encode(x, y));
        }
        // * 2. id 小于自己的机器人的下一步位置
        // for (int i = 0; i < rid; i++) {
        for (int j = 0; j < cur_i; j++) {
            int i = crash_fix_order[j];
            int nxt_pos;
            if (robot[i].status == rns::ROBOT_IDLE || robot[i].wait) {
                // 如果机器人处于恢复状态或等待状态，则下一步位置就是当前位置
                nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
            } else {
                nxt_pos = pns::next_pos_of(i);
                // ! Debug
                // debug_robot("[DETC] next pos of robot %d: (%d, %d)\n", i, pns::pos_decode_x(nxt_pos), pns::pos_decode_y(nxt_pos));
                // TODO: 临时解决方案，需要修正
                if (nxt_pos == -1) {
                    nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
                }
            }
            invalid_pos_masking.insert(nxt_pos);
        }
        // * 3. id 大于自己的机器人的可能位置
        // for (int i = rid + 1; i < ROBOT_NUM; i++) {
        for (int j = cur_i + 1; j < ROBOT_NUM; j++) {
            int i = crash_fix_order[j];
            if (robot[i].status == rns::ROBOT_IDLE || robot[i].wait) {
                // 如果机器人处于恢复状态或等待状态，则下一步位置就是当前位置
                invalid_pos_masking.insert(pns::pos_encode(robot[i].x, robot[i].y));
            } else {
                // 否则，所有可能的位置都是不可走的
                int x = robot[i].x, y = robot[i].y;
                invalid_pos_masking.insert(pns::pos_encode(x, y));  // 原本位置
                for (int j = 0; j < 4; j++) {
                    int x = robot[i].x + magic_directions[j], y = robot[i].y + magic_directions[j + 1];
                    if (pns::valid_pos(x, y)) invalid_pos_masking.insert(pns::pos_encode(x, y));  // 上下左右
                }
            }
        }

        if (invalid_pos_masking.count(cur_nxt_pos) == 0) {
            // 无冲突
            return CRASH_NONE;
        }

        // ? 存在冲突，需要从能够走的位置中选择一个
        // * 1. 首先考虑除了当前下一步位置之外的其他上下左右位置
        // TODO: 考虑加入一定的随机性
        int idx;
        for (int i = 0; i < 4; i++) {
            idx = i;
            int x = cur_x + magic_directions[idx], y = cur_y + magic_directions[idx + 1];
            int pos = pns::pos_encode(x, y);
            if (invalid_pos_masking.count(pos) == 0) {
                pns::insert_avoid_path(rid, cur_pos);
                // 找到一个可以走的位置即可
                pns::insert_avoid_path(rid, pos);
                return 0;
            }
        }
        // * 2. 如果上下左右位置都不行，则考虑当前位置，即原地不动
        robot[rid].wait = 1;

        return CRASH_HAPPENED;
    }

    /**
    * @brief 机器人执行移动指令，返回移动后的位置编码
    * 
    * @param rid I
    * @return int 
    */
    inline int exec_move(const int rid) {
        // ! Debug
        debug_robot("[ROBOT] Executing move command for robot %d\n", rid);

        // * 如果需要暂停则跳过当前帧
        if (robot[rid].wait) {
            robot[rid].wait = 0;
            // ! Debug
            debug_robot("[ROBOT] Robot %d is waiting at (%d, %d)\n", rid, robot[rid].x, robot[rid].y);
            // return 0;
            return pns::pos_encode(robot[rid].x, robot[rid].y);
        }

        int nxt_pos = pns::step_once(rid);
        int cur_pos = pns::pos_encode(robot[rid].x, robot[rid].y);
        int nxt_x = pns::pos_decode_x(nxt_pos), nxt_y = pns::pos_decode_y(nxt_pos);
        // ! Debug
        Assert(pns::valid_pos(nxt_x, nxt_y), "[ROBOT] Invalid position (%d, %d)!\n", nxt_x, nxt_y);
        Assert(abs(nxt_x - robot[rid].x) + abs(nxt_y - robot[rid].y) == 1, "[ROBOT] Invalid move command at (%d, %d) -> (%d, %d)!\n", robot[rid].x, robot[rid].y, nxt_x, nxt_y);
        // ! Debug
        debug_robot("[ROBOT] Robot %d move from (%d, %d) to (%d, %d)\n", rid, robot[rid].x, robot[rid].y, nxt_x, nxt_y);

        // TODO: 添加碰撞检测模块
        if (nxt_x == robot[rid].x) {
            // * 执行 y 方向上的移动
            robot_move(rid, (nxt_y > robot[rid].y) ? ROBOT_DIRECTIONS::RIGHT : ROBOT_DIRECTIONS::LEFT);
        } else {
            // * 执行 x 方向上的移动
            robot_move(rid, (nxt_x > robot[rid].x) ? ROBOT_DIRECTIONS::DOWN : ROBOT_DIRECTIONS::UP);
        }

        // return 0;
        return nxt_pos;
    }

    /**
     * @brief 是否变更目标货物
     * 
     * @return true 
     * @return false 
     */
    inline bool change_target_good(const int frame, const int rid, const Good &new_good) {
        int v1 = robot[rid].val_of_good, v2 = new_good.val;
        int d = robot[rid].moved_steps;
        int d1_d = pns::length_of_path(rid);
        int d2 = dis[new_good.x][new_good.y];

        // if (v2 * (d1_d + d) * 2 <= v1 * (d + d2)) return false;  // * 剪枝
        // if (v2 * (2 * d1_d + d) <= v1 * (d + d2)) return false;  // * 剪枝
        int man_dis = pns::distance(robot[rid].x, robot[rid].y, new_good.x, new_good.y);
        // if (v2 * (d1_d + d) * 2 <= v1 * (d + man_dis + d2)) return false;  // * 剪枝
        // if (v2 * (2 * d1_d + d) <= v1 * (d + man_dis + d2)) return false;  // * 剪枝

        int dd = pns::path_length(rid, pns::pos_encode(robot[rid].x, robot[rid].y), pns::pos_encode(new_good.x, new_good.y));

        if (frame + dd >= new_good.showup_frame + MAX_EXIST_FRAME) return false;  // * 剪枝

        // if (v2 * (d1_d + d) * 2 > v1 * (d + dd + d2)) return true;
        // if (v2 * (2 * d1_d + d) > v1 * (d + dd + d2)) {
        if (v2 >= v1 && dd + d2 <= 2 * d1_d + d) {
            robot[rid].change_able = 0;  // * 只允许更换一次目标
            return true;
        }
        return false;
    }

    /**
     * @brief 将货物分配给机器人
     * 
     * @param rid 
     * @param good 
     * @param bid 
     */
    inline void assign_good_to_robot(const int rid, const Good &good, const int bid) {
        robot[rid].target = rns::T_GOOD;
        robot[rid].mbx = good.x;
        robot[rid].mby = good.y;
        robot[rid].val_of_good = good.val;
        robot[rid].mb_showup_frame = good.showup_frame;
    }

    /**
     * @brief 决定机器人和货物的分配关系
     * 
     * @param frame 
     */
    inline void goods_assignment(const int frame) {
        // ? 首先从每个分区的货物队列中取出货物，然后分配给机器人
        for (const auto &it: order_of_berth) {
            int bid = it.second;
            // if (area_size[bid] == 0 || q_goods[bid].empty()) continue;
            if (area_size[bid] == 0 || !gns::remaining_good(bid)) continue;
            // ! Debug
            debug_robot("consider area %d\n", bid);
            // auto good = q_goods[bid].top().second;
            auto good = gns::fetch_good(bid);
            while (good.showup_frame + MAX_EXIST_FRAME < frame) {
                gns::pop_good(bid);
                debug_robot("[LOSS] good of val %d loss\n", good.val);
                // if (q_goods[bid].empty()) break;
                if (!gns::remaining_good(bid)) break;
                // good = q_goods[bid].top().second;
                good = gns::fetch_good(bid);
            }

            // if (q_goods[bid].empty()) {
            if (!gns::remaining_good(bid)) {
                // ! debug
                debug_robot("no goods in area %d\n", bid);
                continue;
            }

            int rid = -1;
            // * 通用调度，机器人只负责区域内刷新的货物
            // ! Debug
            debug_robot("normal assignment of area %d\n", bid);
            // 将当前货物分配给当前区域内空闲的机器人
            for (int i = 0; i < ROBOT_NUM; i++) {
                if (robot[i].status == rns::ROBOT_WORKING && robot[i].target == rns::T_NONE && robot[i].berth_id == bid) {
                    rid = i;
                    break;
                }
            }
            // ! Debug
            if (rid == -1) {
                debug_robot("No robot available for area %d\n", bid);  // 当前区域内没有空闲的机器人
                continue;
            }

            // ! Debug
            debug_robot("Assign robot %d to area %d\n", rid, bid);

            // 将货物分配给机器人
            // ! Debug
            Assert(robot[rid].status != rns::ROBOT_IDLE, "Robot %d is in idle status!\n", rid);
            // ! Debug
            debug_robot("try to assign good (%d, %d) of val %d to robot %d\n", good.x, good.y, good.val, rid);
            assign_good_to_robot(rid, good, bid);

            set_move_path(rid);  // * 设置移动路径

            // TODO: 如果机器人到达货物位置的时刻货物已经消失，放弃这次分配
            if (frame + pns::length_of_path(rid) >= good.showup_frame + MAX_EXIST_FRAME) {
                int try_count = 0;
                do {
                    // ! Debug
                    debug_robot("give up to assign good (%d, %d) of val %d to robot %d\n", good.x, good.y, good.val, rid);
                    try_count++;
                    debug_robot("try count: %d\n", try_count);

                    pns::clear_robot(rid);
                    robot[rid].target = T_NONE;
                    robot[rid].val_of_good = 0;

                    if (try_count >= 10) break;

                    do {
                        // q_goods[bid].pop();
                        gns::pop_good(bid);
                        debug_robot("[LOSS] good of val %d loss\n", good.val);
                        // if (q_goods[bid].empty()) break;
                        if (!gns::remaining_good(bid)) break;
                        // good = q_goods[bid].top().second;
                        good = gns::fetch_good(bid);
                    } while (good.showup_frame + MAX_EXIST_FRAME < frame && !q_goods[bid].empty());

                    // if (q_goods[bid].empty()) break;
                    if (!gns::remaining_good(bid)) break;

                    assign_good_to_robot(rid, good, bid);
                    set_move_path(rid);  // * 设置移动路径
                } while (frame + pns::length_of_path(rid) >= good.showup_frame + MAX_EXIST_FRAME);

                if (try_count >= 10) {
                    debug_robot("give up to assign good of val %d to robot %d\n", good.val, rid);
                    pns::clear_robot(rid);
                    robot[rid].target = T_NONE;
                    robot[rid].val_of_good = 0;
                    // if (!q_goods[bid].empty()) q_goods[bid].pop();
                    if (gns::remaining_good(bid)) gns::pop_good(bid);
                }

            } else {
                // q_goods[bid].pop();  // * 分配成功，从队列中移除货物
                gns::pop_good(bid);
                // ! Debug
                debug_robot("finish set move path for robot %d\n", rid);
                // ! Debug
                pns::backtrace_path(rid);
                debug_robot("finish backtrace path for robot %d\n", rid);
            }
        }

        // TODO: 优化目标更换算法，使得更换目标后价值能够提升……
        // TODO: 默认关闭
        bool change_target = false;
        if (change_target) {
            for (int rid = 0; rid < ROBOT_NUM; rid++) {
                if (robot[rid].status == rns::ROBOT_WORKING && robot[rid].target == rns::T_GOOD && robot[rid].normal_assignment != 0) {
                    // * 对正在赶往货物所在位置的机器人，如果所在区域刷新了更高价值的货物，则变更目标
                    int bid = robot[rid].berth_id;
                    // if (area_size[bid] == 0 || q_goods[bid].empty()) continue;
                    if (area_size[bid] == 0 || !gns::remaining_good(bid)) continue;
                    auto good = gns::fetch_good(bid);
                    while (good.showup_frame + MAX_EXIST_FRAME < frame) {
                        gns::pop_good(bid);
                        debug_robot("[LOSS] good of val %d loss\n", good.val);
                        if (!gns::remaining_good(bid)) break;
                        good = gns::fetch_good(bid);
                    }
                    if (!gns::remaining_good(bid)) continue;

                    if (change_target_good(frame, rid, good)) {  // * 更高价值的货物出现，变更目标
                        // * 将原目标货物重新加入队列
                        gns::append_good(robot[rid].mb_showup_frame, robot[rid].mbx, robot[rid].mby, robot[rid].val_of_good);

                        pns::clear_robot(rid);
                        robot[rid].moved_steps = 0;

                        // ! Debug
                        debug_robot("change target of robot %d from (%d, %d) to (%d, %d)\n", rid, robot[rid].mbx, robot[rid].mby, good.x, good.y);
                        debug_robot("origin val: %d | new val: %d\n", robot[rid].val_of_good, good.val);
                        assign_good_to_robot(rid, good, bid);

                        set_move_path(rid);  // * 设置移动路径
                        gns::pop_good(bid);  // * 分配成功，从队列中移除货物
                    }
                }
            }
        }
    }

    /**
    * @brief 分配具体机器人给泊位
    * 
    * @param  
    * @return
    */
    inline void assign_robot() {
        // 按照时间顺序优先给泊位分配机器人
        for (auto it: order_of_berth) {
            // 从可用的泊位中分配
            // 循环分 - 找最近的机器人
            int bid = it.second;
            // 每个泊位分配机器人个数
            for (int j = max_robot_capacity[bid]; j > 0; j--) {
                int min_dis = 100000;
                int min_robot_ind = -1;
                for (int i = 0; i < ROBOT_NUM; i++) {
                    if (robot[i].berth_id == -1) {  // 机器人未分配
                        // 机器人到泊位的距离更小
                        int dis = global_dis[bid][robot[i].x][robot[i].y];
                        if (dis != -1 && dis < min_dis) {  // 可达 且 更近
                            min_dis = dis;
                            min_robot_ind = i;
                        }
                    }
                }
                if (min_robot_ind != -1) {
                    robot[min_robot_ind].berth_id = bid;  // 设置机器人归属泊位
                    cur_robot_capacity[bid]++;            // 当前泊位分配机器人数加一
                }
            }
        }
        // 有机器人没有分配到的情况
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (robot[i].berth_id == -1) {
                // 遍历所有可用的泊位
                // 找可达的最近泊位
                int min_dis = 100000;
                int min_ber_ind = -1;
                for (auto it: order_of_berth) {  // 遍历可用泊位
                    int bid = it.second;
                    int dis = global_dis[bid][robot[i].x][robot[i].y];
                    if (dis != -1 && dis < min_dis && cur_robot_capacity[bid] < max_robot_capacity[bid]) {  // 更近的泊位
                        min_dis = dis;
                        min_ber_ind = bid;
                    }
                }
                if (min_ber_ind != -1) {  // 找到可用泊位
                    robot[i].berth_id = min_ber_ind;
                    cur_robot_capacity[min_ber_ind]++;
                }
            }
        }
        // !Debug
        // show assign information
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (robot[i].berth_id != -1) {
                debug_robot("robot %d is assigned to berth %d\n", i, robot[i].berth_id);
            }
        }
    }
    /**
    * @brief 时间不足时派机器人去其余泊位
    * 
    * @param  bid 该泊位id的泊位机器人可以借给其余泊位
    * @return
    */
    inline void refesh_robot(int bid, int frame) {
        // todo 必须是有船可到达的泊位
        // 往可用的货物数量少的泊位发送机器人
        // 注意机器人必须可达该泊位
        vector<pair<float, int>> ber;  // pair: time, ind
        for (int ind = 0; ind < BERTH_NUM; ind++) {
            // 发送至可达的泊位
            if (berth[ind].tag == 1 && global_dis[ind][berth[bid].x][berth[bid].y] != -1 && ind != bid) {
                ber.emplace_back(make_pair(berth[ind].goods_value.size(), ind));
            }
        }
        // 按照泊位货物数量排序
        sort(ber.begin(), ber.end());


        // todo: 船已装货物接近船容量的泊位不发机器人
        for (auto it: ber) {
            int new_bid = it.second;  // 机器人新泊位
            for (int i = 0; i < ROBOT_NUM; i++) {
                if (robot[i].berth_id == bid) {
                    debug_robot("frame %5d| robot %d frin berth %d change to berth %d\n", frame, i, bid, new_bid);
                    // 找到需要新分配的机器人
                    robot[i].berth_id = new_bid;
                    if (robot[i].target == rns::T_BERTH) {
                        // 机器人正在前往泊位
                        // 更改目标泊位
                        robot[i].mbx = berth[new_bid].x;
                        robot[i].mby = berth[new_bid].y;
                    } else if (robot[i].target == rns::T_GOOD) {
                        // 在往货物走
                        // 直接摆烂等待下一次分配
                        robot[i].target = rns::T_NONE;
                        robot[i].mbx = berth[new_bid].x;
                        robot[i].mby = berth[new_bid].y;
                        robot[i].normal_assignment = 0;
                    }
                }
            }
        }
    }
    /**
    * @brief 执行机器人指令
    * 
    * @param frame 
    * @return int 
    */
    inline int execute_robot_instructions(int frame) {
        debug_robot("========== %05d F | execute_robot_instructions | begin ==========\n", frame);

        // ! Debug
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (robot[i].status == rns::ROBOT_IDLE) {
                debug_robot("robot %d at (%d, %d) is in idle status\n", i, robot[i].x, robot[i].y);
            } else {
                debug_robot("robot %d at (%d, %d) is in working status\n", i, robot[i].x, robot[i].y);
            }
        }
        // 0-0
        // 第一帧的时候分配一下机器人
        // 避免机器人不会开不到对应泊位的情况
        if (frame == 1) {
            assign_robot();
        }
        // * 1. 分配货物
        goods_assignment(frame);

        // ? 如果存在卸货后因未被分配货物而停留在泊位的机器人，令其离开泊位，为其它机器人腾出泊位
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            if (robot[rid].target == T_NONE && robot[rid].status == rns::ROBOT_WORKING && dis[robot[rid].x][robot[rid].y] == 0 && pns::empty_path(rid) && pns::empty_avoid_path(rid)) {
                // ! Debug
                debug_robot("robot %d is leaving berth %d\n", rid, robot[rid].berth_id);
                for (int k = 0; k < 4; k++) {
                    int x = robot[rid].x + magic_directions[k], y = robot[rid].y + magic_directions[k + 1];
                    if (pns::valid_pos(x, y) && dis[x][y] == 0) {
                        int opposite_x = robot[rid].x - magic_directions[k], opposite_y = robot[rid].y - magic_directions[k + 1];
                        if (dis[opposite_x][opposite_y] == 1) {
                            pns::append_path(rid, pns::pos_encode(x, y));
                            // pns::append_path(rid, pns::pos_encode(robot[rid].x, robot[rid].y));
                            break;
                        }
                    }
                }
            }
        }

        // * 2. 碰撞检测和移动执行
        set<int> crashed_robots;
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            if (robot[rid].status == rns::ROBOT_IDLE) continue;
            if (crash_detection(rid) == CRASH_HAPPENED)
                crashed_robots.insert(rid);
        }

        vector<int> crash_fix_order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        random_shuffle(crash_fix_order.begin(), crash_fix_order.end());

        // 冲突检测
        // for (int rid = 0; rid < ROBOT_NUM; rid++) {
        for (int i = 0; i < ROBOT_NUM; i++) {
            int rid = crash_fix_order[i];
            if (robot[rid].status == rns::ROBOT_IDLE) continue;  // 跳过处于恢复状态的机器人
            // ! Debug
            if (!crashed_robots.count(rid)) continue;
            // ? 进行冲突检测
            int crash_res = crash_fix(rid, crash_fix_order, i);
            // ! Debug
            if (crash_res == rns::CRASH_HAPPENED) {
                debug_robot("[CRASH] Crash happened for robot %d at (%d, %d)\n", rid, robot[rid].x, robot[rid].y);
            }
        }
        // TODO: 消除放置和取货的延迟
        vector<int> nxt_pos_of_robots(ROBOT_NUM, -1);
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            if (robot[rid].status == rns::ROBOT_IDLE) continue;  // 跳过处于恢复状态的机器人
            // ? 执行移动指令
            if (!pns::empty_path(rid)) {
                nxt_pos_of_robots[rid] = exec_move(rid);
            } else {
                nxt_pos_of_robots[rid] = pns::pos_encode(robot[rid].x, robot[rid].y);
            }
        }

        // * 3. 操作货物（放置或取货）
        // TODO: 修正放置和取货的延迟，在到达指定位置的同一帧就进行操作
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            // 跳过处于恢复状态的机器人
            if (robot[rid].status == rns::ROBOT_IDLE) continue;
            // ? 放置
            // 如果有机器人到达泊位，则进行放置操作
            if (robot[rid].target == rns::T_BERTH && ch[robot[rid].x][robot[rid].y] == BERTH && gds[robot[rid].x][robot[rid].y] == robot[rid].target_area_id) {
                // add goods to berth
                int bid = query_berth_id(robot[rid].x, robot[rid].y);
                berth[bid].value += robot[rid].val_of_good;
                berth[bid].goods_value.push(robot[rid].val_of_good);

                rns::robot_pull(rid);
                robot[rid].target = rns::T_NONE;  // 放置后，重置目标
                robot[rid].change_able = 1;
            } else if (nxt_pos_of_robots[rid] != -1 && robot[rid].target == rns::T_BERTH && ch[pns::pos_decode_x(nxt_pos_of_robots[rid])][pns::pos_decode_y(nxt_pos_of_robots[rid])] == BERTH && gds[pns::pos_decode_x(nxt_pos_of_robots[rid])][pns::pos_decode_y(nxt_pos_of_robots[rid])] == robot[rid].target_area_id) {
                // add goods to berth
                int bid = query_berth_id(pns::pos_decode_x(nxt_pos_of_robots[rid]), pns::pos_decode_y(nxt_pos_of_robots[rid]));
                berth[bid].value += robot[rid].val_of_good;
                sum_value += robot[rid].val_of_good;
                berth[bid].goods_value.push(robot[rid].val_of_good);

                rns::robot_pull(rid);
                robot[rid].target = rns::T_NONE;  // 放置后，重置目标
                robot[rid].change_able = 1;
            }

            // ? 取货
            // 如果有机器人到达目标物品处，则进行取货操作
            else if (robot[rid].target == rns::T_GOOD && robot[rid].x == robot[rid].mbx && robot[rid].y == robot[rid].mby) {
                rns::robot_get(rid);
                robot[rid].target = rns::T_BERTH;  // 取货后，设置目标为泊位

                set_berth_path(rid);  // * 设置运送货物到泊位的路径
                // ! Debug
                pns::backtrace_path(rid);
                debug_robot("finish backtrace path for robot %d\n", rid);
            } else if (nxt_pos_of_robots[rid] != -1 && robot[rid].target == rns::T_GOOD && robot[rid].mbx == pns::pos_decode_x(nxt_pos_of_robots[rid]) && robot[rid].mby == pns::pos_decode_y(nxt_pos_of_robots[rid])) {
                rns::robot_get(rid);
                robot[rid].target = rns::T_BERTH;  // 取货后，设置目标为泊位

                set_berth_path_pre(rid, nxt_pos_of_robots[rid]);  // * 设置运送货物到泊位的路径
                // ! Debug
                pns::backtrace_path(rid);
                debug_robot("finish backtrace path for robot %d\n", rid);
            }
        }

        // ! Debug
        debug_robot("========== %05d F | execute_robot_instructions | end ==========\n", frame);

        return 0;
    }

}  // namespace rns

// ===== header end =====