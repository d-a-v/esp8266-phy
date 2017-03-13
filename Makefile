
LWIP_LIB_RELEASE ?= $(SDK_PATH)/lib/liblwip_gcc.a
LWIP_LIB ?= liblwip_src.a
SDK_PATH ?= $(abspath ../esp8266/tools/sdk)

all clean:
	make -f Makefile.lwip-stub $@
	make -f Makefile.lwip-git $@

install release: all
	test -f $(LWIP_LIB_RELEASE).orig || cp $(LWIP_LIB_RELEASE) $(LWIP_LIB_RELEASE).orig
	cp -f $(LWIP_LIB) $(LWIP_LIB_RELEASE)

try: all install

revert:
	cp $(LWIP_LIB_RELEASE).orig $(LWIP_LIB_RELEASE)
