make clean
make
sudo insmod mycdev.ko
dmesg | tail -8
echo  
echo "enter < mknod /dev/mycdev c %d 0 > to create a dev device"
