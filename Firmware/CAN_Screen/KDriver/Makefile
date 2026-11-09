VERSION         := 1.0.0
TARGET          := $(shell uname -r)
DKMS_ROOT_PATH  := /usr/src/CAN_Screen-$(VERSION)

KERNEL_MODULES	:= /lib/modules/$(TARGET)

ifneq ("","$(wildcard /usr/src/linux-headers-$(TARGET)/*)")
# Configuration pour Ubuntu / Debian
KERNEL_BUILD	:= /usr/src/linux-headers-$(TARGET)
else
ifneq ("","$(wildcard /usr/src/kernels/$(TARGET)/*)")
# Configuration pour Fedora / RHEL
KERNEL_BUILD	:= /usr/src/kernels/$(TARGET)
else
KERNEL_BUILD	:= $(KERNEL_MODULES)/build
endif
endif

# La convention stricte du noyau Linux pour lier CAN_Screen.c -> CAN_Screen.ko
obj-m	:= CAN_Screen.o

.PHONY: all modules clean dkms-install dkms-uninstall

all: modules

modules:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) modules

clean:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) clean

dkms-install:
	@dkms --version > /dev/null 2>&1 || (echo "Erreur: dkms n'est pas installé." && exit 1)
	mkdir -p $(DKMS_ROOT_PATH)
	cp $(CURDIR)/dkms.conf $(DKMS_ROOT_PATH)
	cp $(CURDIR)/Makefile $(DKMS_ROOT_PATH)
	cp $(CURDIR)/CAN_Screen.c $(DKMS_ROOT_PATH)

	# Remplacement propre de la version dans le fichier dkms.conf
	sed -e "s/@VERSION@/$(VERSION)/" -i $(DKMS_ROOT_PATH)/dkms.conf

	dkms add CAN_Screen/$(VERSION)
	dkms build CAN_Screen/$(VERSION)
	dkms install CAN_Screen/$(VERSION)

dkms-uninstall:
	dkms remove CAN_Screen/$(VERSION) --all
	rm -rf $(DKMS_ROOT_PATH)
