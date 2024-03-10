import os
import numpy as np
from PIL import Image

work_dir = os.path.dirname(os.path.abspath(__file__))
base_dir = os.path.dirname(work_dir)

# 文件路径
file_path = os.path.join(base_dir, 'distance.txt')
# file_path = os.path.join(base_dir, 'global_dis_4.txt')

# 读取 txt 文件并转化为 numpy 数组
with open(file_path, 'r') as file:
    lines = file.readlines()
    data = [[float(num) for num in line.split()] for line in lines]
    data = [[255 if num == -1 else num for num in line] for line in data]
    array = np.array(data)

# 找到最大值和最小值
min_val = array.min()
max_val = array.max()

# 归一化到 0 - 255 范围
array_normalized = ((array - min_val) / (max_val - min_val)
                    * 255).astype(np.uint8)
# 反色
array_normalized = 255 - array_normalized

# 将 numpy 数组转换为图片
img = Image.fromarray(array_normalized, 'L')

# 保存图片
image_dir = os.path.join(base_dir, "image")
if os.path.exists(image_dir) is False:
    os.makedirs(image_dir)
img.save(os.path.join(image_dir, 'distance_200x200.png'))
