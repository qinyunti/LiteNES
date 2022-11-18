- 编译:
```
aarch64-linux-gnu-gcc -I include src/key.c src/lcd.c src/main.c src/hal.c src/fce/*.c -o litenes -lpthread
```
- 运行:
```
chmod +x litenes
./litenes ./tank.nes /dev/fb0 /dev/input/event8
```
