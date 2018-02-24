# -*-makefile-*-
#
# Copyright (C) 2011-2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

PTX_PACKAGES_TOOLS := \
	$(CROSS_PACKAGES) \
	$(HOST_PACKAGES) \
	$(LAZY_PACKAGES)

PHONY += license-report

license-report: \
	$(REPORTDIR)/license-report.pdf \
	$(REPORTDIR)/license-report-tools.pdf

$(REPORTDIR)/license-report.pdf: $(addprefix $(STATEDIR)/,$(addsuffix .report,$(PTX_PACKAGES_INSTALL)))
	@$(call targetinfo)
	@mkdir -p $(REPORTDIR)
	@$(image/env) \
	ptx_license_target="$@" \
	ptxd_make_license_report $(sort $(PTX_PACKAGES_INSTALL))
	@$(call finish)

$(REPORTDIR)/license-report-tools.pdf: $(addprefix $(STATEDIR)/,$(addsuffix .report,$(PTX_PACKAGES_TOOLS)))
	@$(call targetinfo)
	@mkdir -p $(REPORTDIR)
	@$(image/env) \
	ptx_license_target="$@" \
	ptxd_make_license_report $(sort $(PTX_PACKAGES_TOOLS))
	@$(call finish)

PTXDIST_LICENSE_COMPLIANCE_OSS_ARCHIVE := $(RELEASEDIR)/OSS-$(call remove_quotes,$(PTXCONF_PROJECT_VENDOR)-$(PTXCONF_PROJECT)$(PTXCONF_PROJECT_VERSION)).tar.gz

PHONY += license-compliance-distribution

license-compliance-distribution: \
		$(RELEASEDIR)/license-compliance.pdf \
		$(PTXDIST_LICENSE_COMPLIANCE_OSS_ARCHIVE)

$(PTXDIST_LICENSE_COMPLIANCE_OSS_ARCHIVE): $(addprefix $(STATEDIR)/,$(addsuffix .release,$(PTX_PACKAGES_INSTALL) $(PTX_PACKAGES_TOOLS)))
	@$(call targetinfo)
	@tar -C "$(RELEASEDIR)" \
		--exclude=license-compliance.pdf --exclude $(notdir $@) \
		 --transform 's;^./;$(notdir $(basename $(basename $@)))/;' -cf "$@" .
	@$(call finish)


$(RELEASEDIR)/license-compliance.pdf: $(addprefix $(STATEDIR)/,$(addsuffix .report,$(PTX_PACKAGES_INSTALL)))
	@$(call targetinfo)
	@mkdir -p $(RELEASEDIR)
	@$(image/env) \
	ptx_license_target="$@" \
	ptxd_make_license_compliance $(sort $(PTX_PACKAGES_INSTALL))
	@$(call finish)

# vim: syntax=make
