#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "debug.h"
#include "defines.h"

// ===== header begin =====

namespace pns {

    // * 常量定义

    const int NN = N * N;  // 位置编码数组长度

    // * 变量定义

    int pos_ptr[ROBOT_NUM][NN];                        // 坐标指针数组，代替真实指针
    bool vis[ROBOT_NUM][NN];                           // 访问标记数组
    int move_path[ROBOT_NUM][NN];                      // 机器人的移动路径
    int path_head[ROBOT_NUM], path_tail[ROBOT_NUM];    // 移动路径头尾指针
    int avoid_path[ROBOT_NUM][NN];                     // 冲突规避路径
    int avoid_head[ROBOT_NUM], avoid_tail[ROBOT_NUM];  // 冲突规避路径头尾指针
    int avoid_path_tag[ROBOT_NUM];                     // 冲突规避路径标记 (append or insert)
    int que_arr[ROBOT_NUM][NN];                        // BFS 队列
    int que_head[ROBOT_NUM], que_tail[ROBOT_NUM];      // BFS 队列头尾指针

    // * 工具定义

    inline int pos_encode(const int x, const int y) { return x * n + y; }
    inline int pos_decode_x(const int pos) { return pos / n; }
    inline int pos_decode_y(const int pos) { return pos % n; }

#define parent_of(rid, pos) (pos_ptr[(rid)][(pos)])
#define qh(rid) (que_head[(rid)])
#define qt(rid) (que_tail[(rid)])
#define ph(rid) (path_head[(rid)])
#define pt(rid) (path_tail[(rid)])
#define ah(rid) (avoid_head[(rid)])
#define at(rid) (avoid_tail[(rid)])
#define mod_inc(x) ((x) = ((x) + 1) % NN)
#define mod_dec(x) (x) = ((x) + NN - 1) % NN

    // * 函数定义

    // ? 距离函数

    inline int distance(const int x1, const int y1, const int x2, const int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }
    inline int distance(const int pos1, const int pos2) {
        return abs(pos_decode_x(pos1) - pos_decode_x(pos2)) + abs(pos_decode_y(pos1) - pos_decode_y(pos2));
    }

    // ? 位置合法性判断

    inline bool valid_pos(const int x, const int y) {
        return x >= 0 && x < n && y >= 0 && y < n && ch[x][y] != BORDER && ch[x][y] != SEA;
    }
    inline bool valid_pos(const int pos) {
        return valid_pos(pos_decode_x(pos), pos_decode_y(pos));
    }

    // ? 访问标记

    inline void mark_vis(const int rid, const int pos) { vis[rid][pos] = true; }
    inline void clear_vis(const int rid) { memset(vis[rid], 0, sizeof(vis[rid])); }

    // ? 队列操作

    inline void clear_que(const int rid) { qh(rid) = qt(rid); }
    inline bool empty_que(const int rid) { return qh(rid) == qt(rid); }
    inline void push_que(const int rid, const int pos) {
        que_arr[rid][qt(rid)] = pos;
        mod_inc(qt(rid));
    }
    inline void pop_que(const int rid) { mod_inc(qh(rid)); }
    inline int front_que(const int rid) { return que_arr[rid][qh(rid)]; }

    // ? 正常移动路径操作

    inline void clear_path(const int rid) { ph(rid) = pt(rid); }
    inline bool empty_path(const int rid) { return ph(rid) == pt(rid); }
    inline void append_path(const int rid, const int pos) {
        move_path[rid][pt(rid)] = pos;
        mod_inc(pt(rid));
    }
    inline void insert_path(const int rid, const int pos) {
        mod_dec(ph(rid));
        move_path[rid][ph(rid)] = pos;
    }

    // ? 冲突规避路径操作

    inline void clear_avoid_path(const int rid) { ah(rid) = at(rid); }
    inline bool empty_avoid_path(const int rid) { return ah(rid) == at(rid); }
    inline void append_avoid_path(const int rid, const int pos) {
        avoid_path[rid][at(rid)] = pos;
        mod_inc(at(rid));
    }
    inline void insert_avoid_path(const int rid, const int pos) {
        mod_dec(ah(rid));
        avoid_path[rid][ah(rid)] = pos;
    }

    inline void skip_first_avoid(const int rid) {
        if (empty_avoid_path(rid)) return;
        mod_inc(ah(rid));
    }

    inline void clear_pos(const int rid) { memset(pos_ptr[rid], -1, sizeof(pos_ptr[rid])); }


    /**
     * @brief 机器人 rid 执行一次移动
     * 
     * @param rid 
     * @return int 
     */
    inline int step_once(const int rid) {
        if (empty_path(rid)) return -1;
        int nxt_pos;
        // * 优先执行冲突规避路径
        if (!empty_avoid_path(rid)) {
            nxt_pos = avoid_path[rid][ah(rid)];
            mod_inc(ah(rid));
            return nxt_pos;
        }
        // * 无需规避冲突，按照正常路径行进
        nxt_pos = move_path[rid][ph(rid)];
        mod_inc(ph(rid));
        return nxt_pos;
    }

    inline void skip_first(const int rid) {
        if (empty_path(rid)) return;
        mod_inc(ph(rid));
    }

    inline int next_pos_of(const int rid) {
        // * 优先查看冲突规避路径
        if (!empty_avoid_path(rid)) {
            return avoid_path[rid][ah(rid)];
        }
        // * 无需规避冲突，则查看正常路径
        if (empty_path(rid)) return -1;
        return move_path[rid][ph(rid)];
    }


    inline void clear_robot(const int rid) {
        clear_que(rid);
        clear_vis(rid);
        clear_path(rid);
        clear_avoid_path(rid);
        clear_pos(rid);
    }


    /**
     * @brief 使用 BFS 的路径规划
     * 
     * @param start_pos 
     * @param end_pos 
     * @return true 
     * @return false 
     */
    inline bool path_planning_bfs(const int rid, const int start_pos, const int end_pos) {
        clear_robot(rid);

        push_que(rid, start_pos);
        mark_vis(rid, start_pos);

        while (!empty_que(rid)) {
            int cur = front_que(rid);
            pop_que(rid);
            int x = pos_decode_x(cur), y = pos_decode_y(cur);
            for (int k = 0; k < 4; k++) {
                int nxt_x = x + magic_directions[k], nxt_y = y + magic_directions[k + 1];
                int nxt_pos = pos_encode(nxt_x, nxt_y);
                if (!valid_pos(nxt_x, nxt_y) || vis[rid][nxt_pos]) continue;

                mark_vis(rid, nxt_pos);
                parent_of(rid, nxt_pos) = cur;

                if (nxt_pos == end_pos) {
                    while (nxt_pos != -1) {
                        insert_path(rid, nxt_pos);
                        nxt_pos = parent_of(rid, nxt_pos);
                    }
                    return true;
                }
                push_que(rid, nxt_pos);
            }
        }
        return false;
    }

    /**
     * @brief “下山”法规划返回泊位的路径
     * 
     * @param rid 
     * @return true 
     * @return false 
     */
    inline bool path_planning_downhill(const int rid) {
        clear_robot(rid);

        // TODO: 修 bug ...
        // TODO: 检查 parent_of 是否必须
        int nxt_x = -1, nxt_y = -1, nxt_pos = -1;
        int x = robot[rid].x, y = robot[rid].y;
        int cur_pos = pos_encode(robot[rid].x, robot[rid].y);
        int cur_dis = dis[robot[rid].x][robot[rid].y];
        int area_id = gds[robot[rid].x][robot[rid].y];

        // ! Debug
        // Debug("[DOWNHILL] start downhill planning for robot %d\n", rid);
        // Debug("start at (%d, %d)\n", x, y);

        append_path(rid, cur_pos);

        // * 寻找周围与 (x, y) 位于同一分区且 dis 数值比当前位置少 1 的点
        while (cur_dis != 0) {
            for (int k = 0; k < 4; k++) {
                nxt_x = x + magic_directions[k], nxt_y = y + magic_directions[k + 1];
                nxt_pos = pos_encode(nxt_x, nxt_y);
                if (!valid_pos(nxt_x, nxt_y) || gds[nxt_x][nxt_y] != area_id || dis[nxt_x][nxt_y] != cur_dis - 1) continue;
                break;
            }
            append_path(rid, nxt_pos);
            parent_of(rid, nxt_pos) = cur_pos;

            x = nxt_x, y = nxt_y;
            cur_dis--;
        }
        return true;
    }

    /**
     * @brief 回溯路径
     * 
     * @param rid 
     */
    inline void backtrace_path(const int rid) {
        // ! Debug
        Assert(ph(rid) != pt(rid), "backtrace_path: path is empty!\n");
        for (int i = ph(rid); i != pt(rid); mod_inc(i)) {
            int pos_x = pos_decode_x(move_path[rid][i]), pos_y = pos_decode_y(move_path[rid][i]);
            // if ((i + 1) % NN != pt(rid))
                // Debug("(%d, %d) -> ", pos_x, pos_y);
            // else
                // Debug("(%d, %d)\n", pos_x, pos_y);
        }
    }

    inline void backtrace_avoid_path(const int rid) {
        // ! Debug
        Assert(ah(rid) != at(rid), "backtrace_avoid_path: path is empty!\n");
        for (int i = ah(rid); i != at(rid); mod_inc(i)) {
            int pos_x = pos_decode_x(avoid_path[rid][i]), pos_y = pos_decode_y(avoid_path[rid][i]);
            // if ((i + 1) % NN != at(rid))
                // Debug("(%d, %d) -> ", pos_x, pos_y);
            // else
                // Debug("(%d, %d)\n", pos_x, pos_y);
        }
    }

}  // namespace pns

// ===== header end =====