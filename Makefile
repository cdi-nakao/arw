obj-m += cdiarw.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all default: modules
inrtall: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@
