@echo off
echo 🔥 开始清理项目目录...

REM 删除构建目录
rd /s /q build

REM 删除测试和旧数据目录
rd /s /q data
rd /s /q fig
rd /s /q test
rd /s /q legacy

REM 删除所有 .exe 可执行文件
del /q *.exe

REM 删除中间输出文件
del /q *.bin
del /q *.bmp

echo ✅ 清理完成，你的项目现在可以发布了！
pause
