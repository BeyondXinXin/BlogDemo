#!/bin/bash

# 添加文件并提交
git add .
mydate=$(date +"%Y-%m-%d")
mytime=$(date +"%H:%M")

# 获取英文星期并映射到中文
weekday=$(date +"%A")
case $weekday in
    "Monday") weekday="星期一" ;;
    "Tuesday") weekday="星期二" ;;
    "Wednesday") weekday="星期三" ;;
    "Thursday") weekday="星期四" ;;
    "Friday") weekday="星期五" ;;
    "Saturday") weekday="星期六" ;;
    "Sunday") weekday="星期天" ;;
esac

# 提交更改
git commit -m "$mydate $weekday $mytime"

# 推送到远程仓库
git push

# 暂停
read -p "Press any key to continue..." -n1 -s
