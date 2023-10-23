#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 input.s"
    exit 1
fi

input_file="$1"
output_file="${input_file%.s}.x"

# 使用MPI汇编器将.s文件转换为机器码
mpicc -S -o "${output_file%.x}.o" "$input_file"

# 使用hexdump将机器码转换为hexdump格式
hexdump -v -e '"%08x\n"' "${output_file%.x}.o" > "$output_file"

echo "Conversion complete. Output saved to $output_file"
