diff srcfile desfile
gcc -ggdb -o main main.c
cp  ~/os_design/task1/task1-2_2.0/build-get-Desktop_Qt_5_12_0_GCC_64bit-Debug/get ~/os_design/task1/task1-2_2.0/run/
cp  ~/os_design/task1/task1-2_2.0/build-copy-Desktop_Qt_5_12_0_GCC_64bit-Debug/copy ~/os_design/task1/task1-2_2.0/run/
cp  ~/os_design/task1/task1-2_2.0/build-put-Desktop_Qt_5_12_0_GCC_64bit-Debug/put ~/os_design/task1/task1-2_2.0/run/
./main srcfile desfile
diff srcfile desfile
