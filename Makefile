
LWIP_LIB ?= liblwip_src.a
SDK_PATH ?= $(abspath ../esp8266/tools/sdk)

all clean:
	make -f Makefile.lwip-stub $@
	make -f Makefile.lwip-git $@

install: all
	cp -f $(LWIP_LIB) $(SDK_PATH)/lib/$(LWIP_LIB)

release: all
	cp -f $(LWIP_LIB) $(SDK_PATH)/lib/liblwip_gcc.a

