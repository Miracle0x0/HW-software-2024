#pragma once
#include <bits/stdc++.h>
#include <bits/types/time_t.h>
#include <random>
using namespace std;

#include "debug.h"
#include "defines.h"
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
    * @brief 判断货物是否合法
    * 
    * @param x 
    * @param y 
    * @return true 
    * @return false 
    */
    inline bool is_valid_good(int x, int y) {
        if (x < 0 || x >= n || y < 0 || y >= n) return false;
        if (ch[x][y] == SEA || ch[x][y] == BORDER) return false;
        return true;
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
            planning_res = pns::path_planning_bfs(rid, start_pos, end_pos, false);
            robot[rid].normal_assignment = 1;
        } else {
            // 后续调度，使用区域内 BFS 算法
            planning_res = pns::path_planning_bfs(rid, start_pos, end_pos);
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

    /**
     * @brief 机器人 rid 的冲突检测
     * 
     * @param rid 
     * @return int 
     */
    inline int crash_detection(const int rid) {
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
        for (int i = 0; i < rid; i++) {
            int nxt_pos;
            if (robot[i].status == rns::ROBOT_IDLE) {
                // 如果机器人处于恢复状态，则下一步位置就是当前位置
                nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
            } else {
                nxt_pos = pns::next_pos_of(i);
                // ! Debug
                // debug_robot("[DETC] next pos of robot %d: (%d, %d)\n", i, pns::pos_decode_x(nxt_pos), pns::pos_decode_y(nxt_pos));
                // TODO: 临时解决方案，需要修正
                if (nxt_pos == -1) {
                    nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
                }
                if (robot[i].wait) {
                    // 如果机器人处于恢复状态，则下一步位置就是当前位置
                    nxt_pos = pns::pos_encode(robot[i].x, robot[i].y);
                }
            }
            invalid_pos_masking.insert(nxt_pos);
        }
        // * 3. id 大于自己的机器人的可能位置
        for (int i = rid + 1; i < ROBOT_NUM; i++) {
            if (robot[i].status == rns::ROBOT_IDLE) {
                // 如果机器人处于恢复状态，则下一步位置就是当前位置
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
    * @brief 机器人执行移动指令
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
            return 0;
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

        return 0;
    }

    /**
    * @brief 执行机器人指令
    * 
    * @param frame 
    * @return int 
    */
    inline int execute_robot_instructions(int frame) {
        debug_robot("%05d F | execute_robot_instructions\n", frame);

        // ! Debug
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (robot[i].status == rns::ROBOT_IDLE) {
                debug_robot("robot %d at (%d, %d) is in idle status\n", i, robot[i].x, robot[i].y);
            } else {
                debug_robot("robot %d at (%d, %d) is in working status\n", i, robot[i].x, robot[i].y);
            }
        }

        // * 1. 分配货物
        for (auto it = order_of_berth.begin(); it != order_of_berth.end(); it++) {
            int bid = it->second;
            if (q_goods[bid].empty()) continue;
            auto good = q_goods[bid].top().second;
            while (good.showup_frame + MAX_EXIST_FRAME < frame) {
                q_goods[bid].pop();
                if (q_goods[bid].empty()) break;
                good = q_goods[bid].top().second;
            }

            int rid = -1;
            // TODO: 区分首次调度和后续调度
            if (first_assignment_tag[bid] == 0) {
                // ! Debug
                debug_robot("first assignment of area %d\n", bid);
                // 首次调度，根据 max_robot_capacity 和 cur_robot_capacity 进行分配
                if (cur_robot_capacity[bid] == max_robot_capacity[bid]) {
                    debug_robot("area %d has reached the maximum robot capacity\n", bid);
                    continue;  // 区域分配到的机器人数量达到上限
                }
                // 优先分配当前区域内的机器人
                for (int i = 0; i < ROBOT_NUM; i++) {
                    if (robot[i].status == rns::ROBOT_WORKING && robot[i].target == rns::T_NONE && gds[robot[i].x][robot[i].y] == bid) {
                        rid = i;
                        break;
                    }
                }
                if (rid == -1) {
                    // 当前区域内没有空闲的机器人，从全局范围内选择一个空闲的机器人
                    for (int i = 0; i < ROBOT_NUM; i++) {
                        if (robot[i].status == rns::ROBOT_WORKING && robot[i].target == rns::T_NONE && gds[robot[i].x][robot[i].y] != -1) {
                            rid = i;
                            break;
                        }
                    }
                }
                cur_robot_capacity[bid]++;
                first_assignment_tag[bid] = 1;
            } else {
                // ! Debug
                debug_robot("normal assignment of area %d\n", bid);
                // 将当前货物分配给当前区域内空闲的机器人
                for (int i = 0; i < ROBOT_NUM; i++) {
                    if (robot[i].status == rns::ROBOT_WORKING && robot[i].target == rns::T_NONE && gds[robot[i].x][robot[i].y] == bid) {
                        rid = i;
                        break;
                    }
                }
                if (rid == -1) continue;  // 当前区域内没有空闲的机器人
            }
            // ! Debug
            debug_robot("Assign robot %d to area %d\n", rid, bid);

            // 将货物分配给机器人
            // ! Debug
            // Assert(robot[rid].status != rns::ROBOT_IDLE, "Robot %d is in idle status!\n", rid);
            if (robot[rid].status == rns::ROBOT_IDLE) {
                debug_robot("Robot %d is in idle status!\n", rid);
            }
            // ! Debug
            debug_robot("assign good (%d, %d) of val %d to robot %d\n", good.val, good.x, good.y, rid);
            robot[rid].target = rns::T_GOOD;
            robot[rid].mbx = good.x;
            robot[rid].mby = good.y;
            robot[rid].val_of_good = good.val;
            q_goods[bid].pop();  // 从队列中移除货物

            set_move_path(rid);  // * 设置移动路径
            // ! Debug
            debug_robot("finish set move path for robot %d\n", rid);
            // ! Debug
            pns::backtrace_path(rid);
            debug_robot("finish backtrace path for robot %d\n", rid);
        }

        // * 2. 碰撞检测和移动执行
        // TODO: 冲突检测
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            if (robot[rid].status == rns::ROBOT_IDLE) continue;  // 跳过处于恢复状态的机器人
            // ? 进行冲突检测
            int crash_res = crash_detection(rid);
            // ! Debug
            if (crash_res == rns::CRASH_HAPPENED) {
                debug_robot("[CRASH] Crash happened for robot %d at (%d, %d)\n", rid, robot[rid].x, robot[rid].y);
            }
        }
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            if (robot[rid].status == rns::ROBOT_IDLE) continue;  // 跳过处于恢复状态的机器人
            // ? 执行移动指令
            if (!pns::empty_path(rid)) { exec_move(rid); }
        }

        // * 3. 操作货物（放置或取货）
        // TODO: 修正放置和取货的延迟，在到达指定位置的同一帧就进行操作
        for (int rid = 0; rid < ROBOT_NUM; rid++) {
            // 跳过处于恢复状态的机器人
            if (robot[rid].status == rns::ROBOT_IDLE) continue;
            // 如果有机器人到达泊位，则进行放置操作
            if (robot[rid].target == rns::T_BERTH && ch[robot[rid].x][robot[rid].y] == BERTH) {
                // add goods to berth
                int bid = query_berth_id(robot[rid].x, robot[rid].y);
                berth[bid].value += robot[rid].val_of_good;
                berth[bid].goods_value.push(robot[rid].val_of_good);

                rns::robot_pull(rid);
                robot[rid].target = rns::T_NONE;  // 放置后，重置目标
            }
            // 如果有机器人到达目标物品处，则进行取货操作
            else if (robot[rid].target == rns::T_GOOD && robot[rid].x == robot[rid].mbx && robot[rid].y == robot[rid].mby) {
                rns::robot_get(rid);
                robot[rid].target = rns::T_BERTH;  // 取货后，设置目标为泊位

                set_berth_path(rid);  // * 设置运送货物到泊位的路径
                // ! Debug
                pns::backtrace_path(rid);
                debug_robot("finish backtrace path for robot %d\n", rid);
            }
        }

        return 0;
    }

}  // namespace rns

// ===== header end =====