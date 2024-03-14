#pragma once
#include <bits/stdc++.h>
using namespace std;

// ===== header begin =====

/**
 * @brief 将日志输出到文件 debug_robot.log
 * 
 * @param format 
 * @param ... 
 */
inline void debug_robot(const char *format, ...) {
#ifdef LOCAL_DEBUG
    FILE *file = fopen("debug_robot.log", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

/**
 * @brief 将日志输出到文件 debug_ship.log
 * 
 * @param format 
 * @param ... 
 */
inline void debug_ship(const char *format, ...) {
#ifdef LOCAL_DEBUG
    FILE *file = fopen("debug_ship.log", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

/**
 * @brief 将日志输出到文件 debug.log
 * 
 * @param format 
 * @param ... 
 */
inline void Debug(const char *format, ...) {
#ifdef LOCAL_DEBUG
    FILE *file = fopen("debug.log", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

/**
 * @brief 将划分后的图输出到文件 partition.txt，用于 python 脚本可视化
 * 
 * @param format 
 * @param ... 
 */
inline void Visualize(const char *format, ...) {
#ifdef LOCAL_DEBUG
    FILE *file = fopen("partition.txt", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

/**
 * @brief 将划分后的图的距离矩阵输出到文件 distance.txt，用于 python 脚本可视化
 * 
 * @param format 
 * @param ... 
 */
inline void Dis(const char *format, ...) {
#ifdef LOCAL_DEBUG
    FILE *file = fopen("distance.txt", "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

inline void GlobalDis(int id, const char *format, ...) {
#ifdef LOCAL_DEBUG
    char filename[20];
    sprintf(filename, "global_dis_%d.txt", id);
    FILE *file = fopen(filename, "a");
    if (file != nullptr) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fclose(file);
    }
#endif
}

#define RED_COLOR_START "\033[1;31m"
#define COLOR_END "\033[0m"

/**
 * @brief 断言设置
 * 
 */
#define Assert(condition, message, ...)              \
    do {                                             \
        if (!(condition)) {                          \
            fprintf(stderr, message, ##__VA_ARGS__); \
            exit(EXIT_FAILURE);                      \
        }                                            \
    } while (0)

/**
 * @brief 日志输出
 * 
 */
#define Log(message, ...)                        \
    do {                                         \
        fprintf(stderr, message, ##__VA_ARGS__); \
        Debug(__VA_ARGS__);                      \
    } while (0)

/**
 * @brief 未实现的函数
 * 
 */
#define NOT_IMPL                                                                                                \
    do {                                                                                                        \
        fprintf(stderr, RED_COLOR_START "[TODO] Not implemented yet at %s:%d\n" COLOR_END, __FILE__, __LINE__); \
        Debug("[TODO] Not implemented yet at %s:%d\n", __FILE__, __LINE__);                                     \
    } while (0)

// ===== header end =====