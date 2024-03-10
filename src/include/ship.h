#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "debug.h"
#include "defines.h"

// ===== header begin =====

namespace sns {
    // ***** 船状态参数 *****

    enum SHIP_STATUS {
        SHIP_TRANSFERRING,  // 0: 移动（运输中）
        SHIP_WORKING,       // 1: 正常运行状态（即装货状态或运输完成状态）
        SHIP_WAITING        // 2: 泊位外等待状态
    };

    // ***** 船指令 *****

    inline int ship_move(int id, int berth_id) {
        // 船 id 去往 泊位 berth_id
        printf("ship %d %d\n", id, berth_id);
        return 0;
    }

    inline int ship_go(int id) {
        printf("go %d\n", id);
        return 0;
    }
}  // namespace sns

/**
 * @brief 查询港口是否有船，返回 1 表示有船，0 表示无船
 * 
 * @param id 
 */
inline int query_berth(int id) {
    for (int i = 0; i < SHIP_NUM; i++)
        if (ship[i].pos == id) return 1;
    return 0;
}

inline int check_ship() {
    for (int i = 0; i < SHIP_NUM; i++) {
        if (ship[i].pos == -1) {  // ? pos:-1
            if (ship[i].status == sns::SHIP_TRANSFERRING) {
                // pos:-1 status:0 表示船在运输中
                // do nothing
            } else if (ship[i].status == sns::SHIP_WORKING) {
                // pos:-1 status:1 表示船在虚拟点
                //        发送 ship 指令
                // v1.0: 选择价值高且没有船的泊位
                int ind = 0, max_value = 0;
                for (int ind_berth = 0; ind_berth < BERTH_NUM; ind_berth++) {
                    // 价值高 且无船
                    if (berth[ind_berth].value > max_value && query_berth(ind_berth) == 0) {
                        ind = ind_berth;
                        max_value = berth[ind_berth].value;
                    }
                }
                // 发送 ship 指令 前往泊位
                // 重设 船的价值和剩余容量
                ship[i].num = ship_capacity;
                ship[i].value = 0;
                sns::ship_move(i, ind);
            }
        } else {  // ? pos:0-9
            // pos:0-9 status:1 表示船在泊位 装货,此时可发出 ship_go 指令
            // 船先离港后装卸货物

            // v1.0: 装满，或者岸边没有货物 运送至虚拟点
            if (ship_capacity == ship[i].num || berth[ship[i].pos].goods_value.empty())
                sns::ship_go(i);
            // 正常情况下装货
            if (ship_capacity - ship[i].num > 0) {
                // 装货
                // 装载 loading_speed 次
                for (int j = 0; j < berth[ship[i].pos].loading_speed; j++) {
                    // 无法继续装
                    if (ship_capacity == ship[i].num || berth[ship[i].pos].goods_value.empty()) break;
                    // 装货（队列中的货物价值）
                    int val = berth[ship[i].pos].goods_value.front();
                    berth[ship[i].pos].goods_value.pop();
                    berth[ship[i].pos].value -= val;  // 泊位价值改变
                    // 容量, 价值改变
                    ship[i].num++;
                    ship[i].value += val;
                }
            }
        }
    }  // end of ship loop
    return 0;
}

// ===== header end =====