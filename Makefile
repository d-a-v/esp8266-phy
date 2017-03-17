
LWIP_LIB_RELEASE ?= $(SDK_PATH)/lib/liblwip_gcc.a
LWIP_LIB ?= liblwip_src.a
SDK_PATH ?= $(abspath ../esp8266/tools/sdk)
ESP_LWIP ?= $(SDK_PATH)/lwip

prepare:
	test -L $(ESP_LWIP) || mv $(ESP_LWIP) $(ESP_LWIP).orig

all clean: prepare
	make -f Makefile.lwip-stub $@
	make -f Makefile.lwip-git $@

install release: all
	test -f $(LWIP_LIB_RELEASE).orig || cp $(LWIP_LIB_RELEASE) $(LWIP_LIB_RELEASE).orig
	cp -f $(LWIP_LIB) $(LWIP_LIB_RELEASE)
	rm -f $(ESP_LWIP); ln -sf $(abspath lwip-git) $(ESP_LWIP)
	rm -f lwip-git/src/include/arch; ln -sf $(abspath lwip-new/arch) lwip-git/src/include
	rm -f lwip-git/include; ln -sf src/include lwip-git/include
	ln -sf $(abspath lwip-new/lwipopts.h) lwip-git/src/include
	ln -sf $(abspath lwip-glue/bufprint.h) lwip-git/src/include
	

try: all install

revert: prepare
	cp $(LWIP_LIB_RELEASE).orig $(LWIP_LIB_RELEASE)
	rm -f $(ESP_LWIP); ln -sf lwip.orig $(ESP_LWIP)
