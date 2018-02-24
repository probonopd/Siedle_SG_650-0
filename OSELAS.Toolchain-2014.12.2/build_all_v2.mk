#!/usr/bin/make -f

#
# Makefile to build all ptxconfigs
# Copyright (C) 2007 Carsten Schlote <c.schlote@konzeptpark.de>
#               2008, 2011 Marc Kleine-Budde <mkl@pengutronix.de>
#

SHELL		:= /bin/bash

#
# config
#
PTX_AUTOBUILD_DESTDIR	:= ${PWD}/inst
export PTX_AUTOBUILD_DESTDIR

BENICE			:= true

PTXDIST			:= ./p --force
ARG			:= images

export PTXDIST_ENV_WHITELIST	:= CROSS_GDB_WITHOUT_PYTHON
export CROSS_GDB_WITHOUT_PYTHON	?= y

ifdef BENICE
NICE			+= nice -20
endif

CONFIGDIR	:= ptxconfigs
CONFIGFILES	:= $(wildcard $(CONFIGDIR)/*.ptxconfig) $(wildcard $(CONFIGDIR)/*/*.ptxconfig)
CONFIGS		:= $(notdir $(basename $(CONFIGFILES)))
CONFIGS_	:= $(subst _,-,$(CONFIGS))

define gen_2configfile
$(eval 2CONFIGFILE_$(subst _,-,$(notdir $(basename $(1)))) := $(1))
endef
$(foreach cfgfile,$(CONFIGFILES),$(call gen_2configfile,$(cfgfile)))

STATEDIR	:= gstate
DISTDIR		:= dist

BUILDS		:= $(foreach config,$(CONFIGS_),$(addprefix $(STATEDIR)/,$(addsuffix .build,$(config))))
OLDCONFIGS	:= $(foreach config,$(CONFIGS_),$(addsuffix .oldconfig,$(config)))

all: $(BUILDS)

$(foreach config,$(CONFIGS_),$(eval $(STATEDIR)/$(config).build: $(2CONFIGFILE_$(config))))
$(STATEDIR)/%.build: | mkdirs
	@echo "building ${*}"
	$(NICE) $(PTXDIST) $(ARG) --ptxconfig=$(2CONFIGFILE_$(*))
	@if [ "$(strip $(filter images,$(ARG)))" = "images" ]; then touch "$@"; fi

oldconfig: $(OLDCONFIGS)

%.oldconfig:
	$(PTXDIST) oldconfig --ptxconfig=$(2CONFIGFILE_$(*))

mkdirs:
	@mkdir -p $(STATEDIR) $(DISTDIR)

print-%:
	@echo "$* is \"$($(*))\""

help:
	@echo "Available build targets:"
	@for i in $(sort $(BUILDS)); do echo $$i; done;
