import os
import re

# 文件路径和注释标志定义
start_comment = r'// ===== header begin ====='
end_comment = r'// ===== header end ====='

# 当前目录，项目根目录，编译使用的临时目录
cur_dir = os.path.dirname(os.path.abspath(__file__))
base_dir = os.path.dirname(cur_dir)
tmp_dir = os.path.join(base_dir, 'tmp')
include_dir = os.path.join(tmp_dir, 'include')

# 目标 main.cpp 文件的路径
target_cpp_file = os.path.join(tmp_dir, 'main.cpp')

# 读取 main.cpp 文件的内容
with open(target_cpp_file, 'r') as file:
    main_cpp_content = file.read()

# 获取文件夹下的所有头文件
for dirpath, dirnames, files in os.walk(tmp_dir):
    for filename in [f for f in files if f.endswith('.h')]:
        header_file_path = os.path.join(dirpath, filename)
        # 读取头文件内容
        with open(header_file_path, 'r') as file:
            header_content = file.read()

        # 提取两个注释标志之间的部分
        extraction_pattern = re.compile(
            r'{}(.*?){}'.format(re.escape(start_comment),
                                re.escape(end_comment)),
            re.DOTALL
        )
        match = extraction_pattern.search(header_content)
        if match:
            # 待替换的头文件内容，移除注释行
            replace_content = re.sub(r'//.*?\n', '', match.group(1))

            # 构建一个标准的 include 行，用于在 main_cpp_content 里面查找
            standard_include_line = f'#include "include/{filename}"'

            # 如果找到这个 include 行，替换它
            if standard_include_line in main_cpp_content:
                main_cpp_content = main_cpp_content.replace(
                    standard_include_line, replace_content)

# 写回 main.cpp 文件
with open(target_cpp_file, 'w') as file:
    file.write(main_cpp_content)

print("Completed inserting header file content into 'main.cpp'.")
