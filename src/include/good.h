#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "defines.h"

// ===== header begin =====

namespace gns {

    /**
     * @brief 物品价值函数，含义为单位时间（即距离）能够获取的价值
     * 
     * @param x 
     * @param y 
     * @param val 
     * @return float 
     */
    inline float cost_of_good(int x, int y, int val) {
        return ((float) val) / dis[x][y];
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
        if (gds[x][y] == -1) return false;
        return true;
    }

    /**
     * @brief 是否还有剩余货物
     * 
     * @param bid 
     * @return true 
     * @return false 
     */
    inline bool remaining_good(const int bid) {
        // return !q_goods[bid].empty();
        return (!q_goods[bid].empty() || !q_goods_bak[bid].empty());
    }

    /**
     * @brief 添加货物
     * 
     * @param x 
     * @param y 
     * @param val 
     * @return true 
     * @return false 
     */
    inline bool append_good(const int frame, const int x, const int y, const int val) {
        if (!is_valid_good(x, y)) return false;
        float cost = cost_of_good(x, y, val);
        auto new_good = Good(x, y, val, frame);
        int area_id = gds[x][y];
        if (cost >= 0.01) {  // TODO: 阈值可调
            q_goods[area_id].push({cost, new_good});
        } else {  // * 加入备选队列
            q_goods_bak[area_id].push({cost, new_good});
        }
        return true;
    }

    /**
     * @brief 获取货物
     * 
     * @param bid 
     * @return Good 
     */
    inline Good fetch_good(const int bid) {
        // return q_goods[bid].top().second;
#ifdef LOCAL_DEBUG
        Assert(remaining_good(bid), "[GOOD] No remaining goods in area %d!\n", bid);
#endif
        if (!q_goods[bid].empty()) return q_goods[bid].top().second;
        return q_goods_bak[bid].top().second;
    }

    /**
     * @brief 取出货物
     * 
     * @param bid 
     */
    inline void pop_good(const int bid) {
#ifdef LODAL_DEBUG
        Assert(remaining_good(bid), "[GOOD] No remaining goods in area %d!\n", bid);
#endif
        // q_goods[bid].pop();
        if (!q_goods[bid].empty()) {
            q_goods[bid].pop();
        } else {
            q_goods_bak[bid].pop();
        }
    }
}  // namespace gns

// ===== header end =====