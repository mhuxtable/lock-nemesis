KERNEL_OBJ = bin/locktest

obj-m = $(KERNEL_OBJ).o
EXTRA_CFLAGS += -I$(PWD)/src 

$(KERNEL_OBJ)-objs += src/main.o
$(KERNEL_OBJ)-objs += src/rnd.o
$(KERNEL_OBJ)-objs += src/stat.o
$(KERNEL_OBJ)-objs += src/test.o
$(KERNEL_OBJ)-objs += src/sims.o
	
KDIR=/lib/modules/$(shell uname -r)/build
#KDIR=/local/scratch/mjh233/qemu-linux/linux-3.13.1/

all: 
	mkdir -p bin
	make -C $(KDIR) M=$(PWD) modules
	cp bin/locktest.ko .

clean:
	find bin/ -mindepth 1 -delete
	find src/ -name "*.o" -delete
	find src/ -name ".*.o.cmd" -delete
	rm -Rf .tmp_versions/
	rm -f modules.order
	rm -f Module.symvers
	rm -rf ./locktest.ko