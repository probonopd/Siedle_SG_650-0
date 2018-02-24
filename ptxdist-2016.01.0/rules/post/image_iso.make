# -*-makefile-*-
#
# Copyright (C) 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

SEL_ROOTFS-$(PTXCONF_IMAGE_ISO) += $(IMAGEDIR)/bootcd.iso

image_iso/workdir := $(IMAGEDIR)/image_iso-workdir
image_iso/isolinux_bin := $(PTXDIST_SYSROOT_TARGET)/usr/share/syslinux/isolinux.bin

ifdef PTXCONF_IMAGE_ISO
$(IMAGEDIR)/bootcd.iso: $(IMAGEDIR)/root.cpio.gz $(IMAGEDIR)/linuximage $(image_iso/isolinux_bin)
	@echo -n "Creating '$(notdir $(@))' from '$(notdir $(<))'..."
	@rm -rf "$(image_iso/workdir)"
	@mkdir -p "$(image_iso/workdir)"
	@cp $(^) "$(image_iso/workdir)"
	@mv "$(image_iso/workdir)/root.cpio.gz" "$(image_iso/workdir)/initrd.gz"
	@mv "$(image_iso/workdir)/linuximage" "$(image_iso/workdir)/kernel"
	@tar -C $(PTXCONF_IMAGE_ISO_ADDON_DIR) -cf - \
		--exclude .git \
		--exclude .pc \
		--exclude .svn \
		--exclude *~ \
		. | \
		tar -o -C "$(image_iso/workdir)" -xf -
	@if [ \! -e "$(image_iso/workdir)/isolinux.cfg" ]; then \
		ptxd_bailout "no isolinux.cfg found - please ensure you have one in your $(PTXCONF_IMAGE_ISO_ADDON_DIR) directory."; \
	fi
	@genisoimage \
		-R \
		-V "$(call remove_quotes, $(PTXCONF_PROJECT)$(PTXCONF_PROJECT_VERSION))" \
		-o $(@) \
		-b "$(notdir $(image_iso/isolinux_bin))" \
		-c boot.cat \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		"$(image_iso/workdir)" >/dev/null 2>&1
	@rm -rf "$(image_iso/workdir)"
	@echo "done."
endif

# vim: syntax=make
