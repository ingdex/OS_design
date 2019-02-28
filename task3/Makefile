ifneq ($(KERNELRELEASE),)
#kbuild syntax.
mymodule-objs := mycdev.o
obj-m := mycdev.o
else
	PWD:=$(shell pwd)
	KVER:=$(shell uname -r)
	KDIR:=/lib/modules/$(KVER)/build
all :
	$(MAKE) -C $(KDIR) M=$(PWD)
    
clean:
	#rm -f *.cmd *.o *.mod *.ko
	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions
endif
