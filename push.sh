#!/bin/bash

# 添加所有文件
git add .

# 获取当前日期和时间
mydate=$(date +"%Y-%m-%d")
mytime=$(date +"%H:%M")

# 提交更改
git commit -m "$mydate $mytime"

# 推送到远程仓库
git push

# 保持终端窗口打开
read -p "Press any key to continue..." -n1 -s

