#!/bin/bash

# 读取标准输入的数据到变量中
input_data=$(cat)

# 提取所有的分数到数组
scores=($(echo "$input_data" | grep -oP '(?<="score":)\d+'))

# 初始化最大和最小值变量，设定初值为第一个分数
max_score=${scores[0]}
min_score=${scores[0]}
total_score=0

# 遍历分数数组
for i in "${!scores[@]}"; do
  score=${scores[$i]}
  total_score=$((total_score + score))

  # 更新最大值
  if [ "$score" -gt "$max_score" ]; then
    max_score=$score
  fi

  # 更新最小值
  if [ "$score" -lt "$min_score" ]; then
    min_score=$score
  fi
done

# 计算平均值
average_score=$(echo "$total_score / ${#scores[@]}" | bc)

# 打印结果
echo "最高分数: $max_score"
echo "最低分数: $min_score"
echo "平均分数: $average_score"
