import os
from PIL import Image

work_dir = os.path.dirname(os.path.abspath(__file__))
base_dir = os.path.dirname(work_dir)

# Map each character to an RGB color
color_map = {
    '0': (255, 0, 0),      # Red
    '1': (0, 255, 0),      # Green
    '2': (0, 0, 255),      # Blue
    '3': (255, 255, 0),    # Yellow
    '4': (255, 0, 255),    # Magenta
    '5': (0, 255, 255),    # Cyan
    '6': (192, 192, 192),  # Silver
    '7': (128, 0, 0),      # Maroon
    '8': (128, 128, 0),    # Olive
    '9': (0, 128, 0),      # Dark Green
    'x': (0, 0, 0),        # Black
}

# Your provided text
text = ""
# Read text from file
partition_file = os.path.join(base_dir, 'partition.txt')
try:
    with open(partition_file, 'r') as file:
        text = file.read()
except FileNotFoundError:
    raise FileNotFoundError("The file partition.txt does not exist.")

# 删除文本中的换行符
text = text.replace('\n', '')

# 确保文本足够长
assert len(text) >= 40000, "文本长度需要至少40000字符。"

# 创建一个200x200的白色背景图像
img = Image.new('RGB', (200, 200), color=(255, 255, 255))
pixels = img.load()

# 按照文本设置像素
for y in range(200):
    for x in range(200):
        char = text[x + y * 200]  # 200个字符为一行
        pixels[x, y] = color_map.get(char, (0, 0, 0))  # 如果映射中没有字符，则为黑色

# 保存图像
image_dir = os.path.join(base_dir, "image")
if os.path.exists(image_dir) is False:
    os.makedirs(image_dir)
img.save(os.path.join(image_dir, 'partition_200x200.png'))
