#!/bin/bash

# 设置中文环境获取星期
LC_TIME=zh_CN.UTF-8

# 添加文件并提交
git add .
mydate=$(date +"%Y-%m-%d")
weekday=$(date +"%A")
mytime=$(date +"%H:%M")
git commit -m "$mydate $weekday $mytime"

# 推送到远程仓库
git push

# 暂停
read -p "Press any key to continue..." -n1 -s
