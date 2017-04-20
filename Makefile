
ROOT	= .
include $(ROOT)/Makefile.common

# esp
IRAM	= glue2esp_err esp2glue_err pbuf_wrapper_get pbuf_wrapper_release glue2esp_linkoutput ethernet_input pbuf_alloc pbuf_free pbuf_ref
# git
IRAM	+= glue2git_err git2glue_err new_linkoutput esp2glue_pbuf_freed esp2glue_alloc_for_recv esp2glue_ethernet_input
# lwip2
IRAM	+= sys_timeout_RENAMED

all clean: prepare
	make -f Makefile.lwip-esp ROOT=$(ROOT) $@
	make -f Makefile.lwip-git ROOT=$(ROOT) $@

prepare:
	@# rename original lwip directory -> lwip.orig
	test -r $(ESP_LWIP) && { test -L $(ESP_LWIP) || mv $(ESP_LWIP) $(ESP_LWIP_ORIG); } || true
	@# copy original library file
	test -f $(LWIP_LIB_RELEASE_ORIG) || cp $(LWIP_LIB_RELEASE) $(LWIP_LIB_RELEASE_ORIG)

rename:
	@# according to eagle.app.v6.common.ld:
	@# putting symbols into .gnu.linkonce.literal.* instead of .text.*
	@# will eventually move them in iram: section .text.* instead of .irom0.text.*
	@# check this with xtensa-lx106-elf-objdump -t <elfresult> | grep <symbol>
	@for i in $(IRAM); do \
		echo "---- moving $$i in IRAM"; \
		$(OC) --rename-section .text.$$i=.gnu.linkonce.literal.$$i $(LWIP_LIB_RELEASE); \
	done

install release: all rename symlink

rominstall romrelease: all symlink

symlink:
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

