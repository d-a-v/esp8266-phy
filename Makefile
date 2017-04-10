
ROOT	= .
include $(ROOT)/Makefile.common

all clean: prepare
	make -f Makefile.lwip-esp ROOT=$(ROOT) $@
	make -f Makefile.lwip-git ROOT=$(ROOT) TCP_MSS=$(TCP_MSS) $@

prepare:
	@# rename original lwip directory -> lwip.orig
	test -r $(ESP_LWIP) && { test -L $(ESP_LWIP) || mv $(ESP_LWIP) $(ESP_LWIP_ORIG); } || true
	@# copy original library file
	test -f $(LWIP_LIB_RELEASE_ORIG) || cp $(LWIP_LIB_RELEASE) $(LWIP_LIB_RELEASE_ORIG)

install release: all
	@# (re)symlink library file
	rm -f $(LWIP_LIB_RELEASE)
	ln -sf $(abspath $(LWIP_LIB)) $(LWIP_LIB_RELEASE)
	@# (re)symlink lwip directory to the new one
	rm -f $(ESP_LWIP)
	ln -sf $(abspath lwip-git-src) $(ESP_LWIP)
	@# (re)symlink some include files to the right place from esp point of view
	rm -f lwip-git-src/src/include/arch; ln -sf $(abspath lwip-git/arch) lwip-git-src/src/include
	rm -f lwip-git-src/include; ln -sf src/include lwip-git-src/include
	ln -sf $(abspath lwip-git/lwipopts.h) $(abspath lwip-glue/esp-missing.h) lwip-git-src/src/include

disappear revert:
	@# restore original library file
	test -r $(LWIP_LIB_RELEASE_ORIG) && rm -f $(LWIP_LIB_RELEASE) && cp $(LWIP_LIB_RELEASE_ORIG) $(LWIP_LIB_RELEASE)
	@# restore original lwip directory
	test -r $(ESP_LWIP_ORIG) && rm -f $(ESP_LWIP) && mv $(ESP_LWIP_ORIG) $(ESP_LWIP) || true
