#!/bin/bash

# 检查命令行参数
if [ $# -ne 1 ]; then
    echo "用法: $0 input.s"
    exit 1
fi

# 提取输入文件名和输出文件名
input_file="$1"
output_file="${input_file%.s}.o"

# 使用 MIPS GNU 工具链中的汇编器将.s文件汇编为.o文件
mips-linux-gnu-as -march=mips32 "$input_file" -o "$output_file"

# 检查汇编是否成功
if [ $? -eq 0 ]; then
    echo "汇编成功，目标文件保存为 $output_file"
else
    echo "汇编失败"
fi

