# -*-makefile-*-
#
# Copyright (C) 2006-2008 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UCLIBC_HEADERS) += uclibc-headers

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(UCLIBC_CONFIG):
	@echo
	@echo "**************************************************************************"
	@echo "**** Please generate a uclibc config with 'ptxdist menuconfig uclibc' ****"
	@echo "**************************************************************************"
	@echo
	@echo
	@exit 1

$(STATEDIR)/uclibc-headers.prepare: $(STATEDIR)/uclibc.extract $(UCLIBC_CONFIG)
	@$(call targetinfo)

	@if [ -f $(UCLIBC_CONFIG) ]; then				\
		echo "Using uclibc config file: $(UCLIBC_CONFIG)";	\
		install -m 644 $(UCLIBC_CONFIG) $(UCLIBC_DIR)/.config; 	\
	else								\
		echo "ERROR: No such uclibc config: $(UCLIBC_CONFIG)";	\
		exit 1;							\
	fi

	cd $(UCLIBC_DIR) && yes "" | \
		$(UCLIBC_PATH) $(UCLIBC_ENV) $(MAKE)  \
		$(UCLIBC_MAKE_OPT) oldconfig

	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/uclibc-headers.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/uclibc-headers.install:
	@$(call targetinfo)
	cd $(UCLIBC_DIR) && \
		$(UCLIBC_PATH) $(UCLIBC_ENV) \
		$(MAKE) $(UCLIBC_MAKE_OPT) \
		install_headers
	@$(call touch)

# vim: syntax=make
