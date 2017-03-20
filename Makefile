
LWIP_LIB_RELEASE ?= $(SDK_PATH)/lib/liblwip_gcc.a
LWIP_LIB ?= liblwip_src.a
SDK_PATH ?= $(abspath ../esp8266/tools/sdk)
ESP_LWIP ?= $(SDK_PATH)/lwip

all clean: prepare
	make -f Makefile.lwip-esp $@
	make -f Makefile.lwip-git $@

prepare:
	test -r $(ESP_LWIP) && { test -L $(ESP_LWIP) || mv $(ESP_LWIP) $(ESP_LWIP).orig; } || true

install release: all
	test -f $(LWIP_LIB_RELEASE).orig || cp $(LWIP_LIB_RELEASE) $(LWIP_LIB_RELEASE).orig
	cp -f $(LWIP_LIB) $(LWIP_LIB_RELEASE)
	rm -f $(ESP_LWIP); ln -sf $(abspath lwip-git-src) $(ESP_LWIP)
	rm -f lwip-git-src/src/include/arch; ln -sf $(abspath lwip-git/arch) lwip-git-src/src/include
	rm -f lwip-git-src/include; ln -sf src/include lwip-git-src/include
	ln -sf $(abspath lwip-git/lwipopts.h) lwip-git-src/src/include

try: all install

revert: prepare
	cp $(LWIP_LIB_RELEASE).orig $(LWIP_LIB_RELEASE)
	rm -f $(ESP_LWIP); ln -sf lwip.orig $(ESP_LWIP)
