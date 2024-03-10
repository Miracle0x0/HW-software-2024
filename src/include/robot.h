#pragma once
#include <bits/stdc++.h>
#include <limits>
using namespace std;

#include "debug.h"
#include "defines.h"

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

    // ***** 机器人指令 *****

    // move
    inline int
    robot_move(int id, int direction) {
        printf("move %d %d\n", id, direction);
        return 0;
    }
    // get
    inline int robot_get(int id) {
        NOT_IMPL;
        return 0;
    }
    // pull
    inline int robot_pull(int id) {
        NOT_IMPL;
        return 0;
    }
}  // namespace rns

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

// TODO: 机器人寻路算法
inline int set_move_path(int rid) {
    Assert(robot[rid].status != rns::ROBOT_IDLE, "Robot %d is in idle status!\n", rid);
    NOT_IMPL;
    return 0;
}

// TODO: 执行机器人指令
inline int execute_robot_instructions(int frame) {
    Debug("%05d F | execute_robot_instructions\n", frame);

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
        // 将当前货物分配给当前区域内空闲的机器人
        int rid = -1;
        for (int i = 0; i < ROBOT_NUM; i++) {
            if (robot[i].status == rns::ROBOT_WORKING && robot[i].target == rns::T_NONE && gds[robot[i].x][robot[i].y] == bid) {
                rid = i;
                break;
            }
        }
        if (rid == -1) continue;  // 当前区域内没有空闲的机器人
        // 将货物分配给机器人
        robot[rid].target = rns::T_GOOD;
        robot[rid].mbx = good.x;
        robot[rid].mby = good.y;
        robot[rid].goods = good.val;
        q_goods[bid].pop();  // 从队列中移除货物
        // TODO: 设置移动路径
    }

    // * 2. 移动执行和碰撞检测
    // TODO: ...

    // * 3. 操作货物（放置或取货）
    for (int rid = 0; rid < ROBOT_NUM; rid++) {
        // 跳过处于恢复状态的机器人
        if (robot[rid].status == rns::ROBOT_IDLE) continue;
        // 如果有机器人到达泊位，则进行放置操作
        if (robot[rid].target == rns::T_BERTH && ch[robot[rid].x][robot[rid].y] == BERTH) {
            rns::robot_pull(rid);
            robot[rid].goods = 0;
            robot[rid].target = rns::T_NONE;  // 放置后，重置目标
        }
        // 如果有机器人到达目标物品处，则进行取货操作
        if (robot[rid].target == rns::T_GOOD && robot[rid].x == robot[rid].mbx && robot[rid].y == robot[rid].mby) {
            rns::robot_get(rid);
            robot[rid].goods = 0;
            robot[rid].target = rns::T_BERTH;  // 取货后，设置目标为泊位
        }
    }

    return 0;
}

// ===== header end =====