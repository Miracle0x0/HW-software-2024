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
        int bid = ship[id].pos;
        ship[id].status = sns::SHIP_WORKING;
        ship[id].pos = -1;
        debug_ship("frame %5d| avaliable %d\n", frame, avaliable_berth);
        // go 最后5000帧只运输5个泊位
        if (frame > pre_frame && avaliable_berth > 5) {
            // 遍历所有机器人查看当前可用泊位数目
            // 不再用这个泊位
            avaliable_berth--;
            berth[bid].tag = 0;
            rns::refesh_robot(bid, frame);
            debug_ship("frame %5d| avaliavle %d\n", frame, avaliable_berth);
        }

        // 打印所有泊位的货物价值
        int tmp = 0;
        for (int i = 0; i < BERTH_NUM; i++) {
            tmp += berth[i].value;
            debug_ship("frame %5d| berth %d goods_val %d goods_num %d avaliable %d\n",
                       frame, i, berth[i].value, berth[i].goods_value.size(), berth[i].tag);
        }
        debug_ship("frame %5d| sum val %d\n", frame, tmp);

        return 0;
    }
    // 查找最小可被选中的泊位时间
    inline int find_min_time() {
        int min_ind = -1, min_time = -1;
        // 查找可被选中泊位
        for (int i = 0; i < BERTH_NUM; i++) {
            if (berth[i].tag == 1) {
                min_ind = i;
                min_time = berth[i].transport_time;
                break;
            }
        }
        // 查找最小可被选中的泊位时间
        for (int i = min_ind; i < BERTH_NUM; i++) {
            if (berth[i].tag == 1 && min_time < berth[i].transport_time) {
                min_ind = i;
                min_time = berth[i].transport_time;
                break;
            }
        }
        return min_ind;
    }
    // 根据时间 更新可被选中的berth的tag
    // 时间不足时不再选中往返耗时长的泊位
    inline void check_tag(int frame) {
        for (int i = 0; i < BERTH_NUM; i++) {
            // 不再选中往返耗时长的泊位
            if (berth[i].tag == 1 && ((FRAME_NUM - frame) < 2 * berth[i].transport_time)) {
                berth[i].tag = 0;
            }
        }
    }
    // 计算总时间
    // 船到泊位加上卸货时间
    inline float get_hole_time(int id) {
        // * 削弱 transport_time 量级的影响
        return 2.0 * berth[id].transport_time / 2000.0 + ship_capacity / (1.0 * berth[id].loading_speed);
    }
    // 执行船操作
    inline int check_ship(int frame) {
        if (frame == 1) {
            // 初始安排next_berth
            for (auto it: order_of_berth) {
                avaliable_berth++;
            }
        }

        for (int i = 0; i < SHIP_NUM; i++) {
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
                    /*
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
                    */
                    // v1.1: 选择时间短的泊位且没有船的泊位
                    //       时间不足时选择时选择时间合适的泊位
                    // 根据时间 更新可选中泊位
                    // check_tag(frame);
                    vector<pair<float, int>> ber;  // pair: time, ind
                    for (int ind = 0; ind < BERTH_NUM; ind++) {
                        if (berth[ind].tag == 1 && query_berth(ind) == 0) {
                            // 往返时间 + 卸货时间
                            //ber.emplace_back(make_pair(2.0 * berth[ind].transport_time + ship_capacity/(1.0 * berth[ind].loading_speed), ind) );
                            ber.emplace_back(make_pair(-berth[ind].value, ind));
                        }
                        if (berth[ind].tag == 0 && query_berth(ind) == 0 && berth[ind].value > 500 && berth[ind].goods_value.size() > 5) {
                            ber.emplace_back(make_pair(-berth[ind].value, ind));
                        }
                    }
                    sort(ber.begin(), ber.end());
                    // 输出一遍各个泊位价值
                    // for (auto it:ber) {
                    //     debug_ship("frame %5d| berth %lf value %d\n", frame, it.second, it.first);
                    // }
                    if (ber.size() >= 1) {  // 有泊位可去
                        // 发送 ship 指令 前往泊位
                        // 重设 船的价值和剩余容量
                        ship[i].num = 0;
                        ship[i].value = 0;
                        sns::ship_move(i, ber[0].second, frame);
                    }

                } else {
                    // pos:0-9 status:1 表示船在泊位 装货,此时可发出 ship_go 指令
                    // 船先离港后装卸货物

                    // v1.0: 装满，或者岸边没有货物 运送至虚拟点

                    // v1.1: 时间不足往返 继续装货
                    //       剩余时间等于送到泊位 发送go指令
                    int bid = ship[i].pos;
                    int min_bid = find_min_time();

                    // debug_ship("frmae %5d| ship %d ship_num %d berth %d good_val %d\n", frame, i, ship[i].num, ship[i].pos, berth[ship[i].pos].goods_value);
                    if (ship_capacity == ship[i].num) {  // 装满 运送至虚拟点
                        debug_ship("frmae %5d| ship %d left by no capacity\n", frame, i);
                        sns::ship_go(i, frame);
                    }
                    if (frame > pre_frame) {
                        if (avaliable_berth > 5) {
                            if (berth[bid].goods_value.empty()) {
                                debug_ship("frmae %5d| ship %d left by no goods\n", frame, i);
                                sns::ship_go(i, frame);
                            }
                        } else {
                            // 把剩余的位置
                            if (get_hole_time(bid) + berth[bid].transport_time + 20 == FRAME_NUM - frame) {
                                // todo: 衡量装货时间
                                //        (ship_capacity-ship[i].num)/berth[i].loading_speed
                                debug_ship("frmae %5d| ship %d left by at end plan\n", frame, i);
                                sns::ship_go(i, frame);
                            }
                            // 剩余时间等于送到泊位时间 运送至虚拟点
                            if (FRAME_NUM - frame <= berth[bid].transport_time) {
                                debug_ship("frmae %5d| ship %d left by no time\n", frame, i);
                                sns::ship_go(i, frame);
                            }
                        }
                    } else {
                        if (berth[bid].goods_value.empty()) {
                            debug_ship("frmae %5d| ship %d left by no goods\n", frame, i);
                            sns::ship_go(i, frame);
                        }
                    }


                    // 正常情况下装货
                    if (ship_capacity - ship[i].num > 0) {
                        // 装货
                        // 装载 loading_speed 次
                        for (int j = 0; j < berth[bid].loading_speed; j++) {
                            // 无法继续装
                            if (ship_capacity == ship[i].num || berth[bid].goods_value.empty()) break;
                            // 装货（队列中的货物价值）
                            int val = berth[bid].goods_value.front();
                            send_value += val;
                            berth[bid].goods_value.pop();
                            berth[bid].value -= val;  // 泊位价值改变
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
}  // namespace sns


// ===== header end =====