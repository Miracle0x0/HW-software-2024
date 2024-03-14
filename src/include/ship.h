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

    inline int ship_move(int id, int berth_id, int frame) {
        // 船 id 去往 泊位 berth_id
        debug_ship("frame %5d| ship %d to %d\n", frame, id, berth_id);
        ship[id].status = sns::SHIP_TRANSFERRING;
        ship[id].pos = berth_id;
        printf("ship %d %d\n", id, berth_id);
        return 0;
    }

    inline int ship_go(int id, int frame) {
        debug_ship("frame %5d| go %d\n", frame, id);
        printf("go %d\n", id);
        ship[id].status = 1;
        ship[id].pos = -1;
        return 0;
    }
}  // namespace sns

inline int check_ship(int frame) {
    for (int i = 0; i < SHIP_NUM; i++) {
        debug_ship("frame %5d| ship %d status %d pos %d\n", frame, i, ship[i].status, ship[i].pos);
        // status: 0 移动中
        // status: 2 等待中
        if (ship[i].status == sns::SHIP_TRANSFERRING || ship[i].status == sns::SHIP_WAITING) {
            // do nothing
            continue;
        } else if (ship[i].status == sns::SHIP_WORKING) {  // status: 1 working
            if (ship[i].pos == -1) {
                // pos:-1 status:1 表示船在虚拟点
                //        发送 ship 指令
                // v1.0: 选择价值高且没有船的泊位
                int ind = 0, max_value = -1;
                for (int ind_berth = 0; ind_berth < BERTH_NUM; ind_berth++) {
                    // 价值高 且无船
                    if (berth[ind_berth].value >= max_value && query_berth(ind_berth) == 0) {
                        ind = ind_berth;
                        max_value = berth[ind_berth].value;
                    }
                }
                // 发送 ship 指令 前往泊位
                // 重设 船的价值和剩余容量
                ship[i].num = 0;
                ship[i].value = 0;
                sns::ship_move(i, ind, frame);

            } else {
                // pos:0-9 status:1 表示船在泊位 装货,此时可发出 ship_go 指令
                // 船先离港后装卸货物

                // v1.0: 装满，或者岸边没有货物 运送至虚拟点
                debug_ship("frame %5d| ship %d ship_num %d berth %d good_val %d\n", frame, i, ship[i].num, ship[i].pos, berth[ship[i].pos].goods_value.size());
                if (ship_capacity == ship[i].num || berth[ship[i].pos].goods_value.empty())
                    sns::ship_go(i, frame);

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
        }

    }  // end of ship loop
    return 0;
}

// ===== header end =====