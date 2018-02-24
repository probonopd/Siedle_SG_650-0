# -*-makefile-*-
#
# Copyright (C) 2003-2009 by the ptxdist project <ptxdist@pengutronix.de>
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

SEL_ROOTFS-$(PTXCONF_IMAGE_IPKG_PUSH_TO_REPOSITORY) += $(STATEDIR)/ipkg-push

ipkg-push : $(STATEDIR)/ipkg-push

$(STATEDIR)/ipkg-push: $(STATEDIR)/host-ipkg-utils.install.post $(STATEDIR)/world.targetinstall
	@$(call targetinfo)
ifdef PTXCONF_IMAGE_IPKG_FORCED_PUSH
	rm  -rf "$(IMAGE_REPO_DIST_DIR)"
endif
	@echo "pushing ipkg packages to ipkg-repository..."
	@$(HOST_ENV) $(PTXDIST_TOPDIR)/scripts/ipkg-push \
		--ipkgdir  $(call remove_quotes,$(PKGDIR)) \
		--repodir  $(call remove_quotes,$(PTXCONF_SETUP_IPKG_REPOSITORY)) \
		--revision $(call remove_quotes,$(PTXDIST_VERSION_FULL)) \
		--project  $(call remove_quotes,$(PTXCONF_PROJECT)) \
		--dist     $(call remove_quotes,$(PTXCONF_PROJECT)$(PTXCONF_PROJECT_VERSION)) \
		--type     $(PTXCONF_HOST_PACKAGE_MANAGEMENT)
	@echo "ipkg-repository updated"
ifdef PTXCONF_IMAGE_IPKG_SIGN_OPENSSL
	@echo "signing Packages..."
	openssl smime -sign \
		-in "$(IMAGE_REPO_DIST_DIR)/Packages" \
		-text -binary \
		-outform PEM \
		-signer $(PTXCONF_IMAGE_IPKG_SIGN_OPENSSL_SIGNER) \
		-inkey $(PTXCONF_IMAGE_IPKG_SIGN_OPENSSL_KEY) \
		-out "$(IMAGE_REPO_DIST_DIR)/Packages.sig"
	@echo "Packages.sig created"
endif
	@touch $@


SEL_ROOTFS-$(PTXCONF_IMAGE_IPKG_INDEX) += $(PKGDIR)/Packages

ipkg-index: $(PKGDIR)/Packages

PHONY += $(PKGDIR)/Packages
$(PKGDIR)/Packages: $(STATEDIR)/host-ipkg-utils.install.post $(STATEDIR)/world.targetinstall
	@echo "Creating ipkg index '$@'..."
	@rm -f $(PKGDIR)/Packages*
	@$(HOST_ENV) $(PTXCONF_HOST_PACKAGE_MANAGEMENT)-make-index \
		-l "$(PKGDIR)/Packages.filelist" -p "$(@)" "$(PKGDIR)"
	@echo "done."

# vim: syntax=make
