
$(STATEDIR)/world.cleanup: $(STATEDIR)/world.targetinstall
	@$(call targetinfo)
	@find "$(PTXCONF_SYSROOT_CROSS)/"* -depth -type d -print0 | \
		xargs -r -0 -- rmdir -v --ignore-fail-on-non-empty --
	find \
		"$(PTXCONF_SYSROOT_CROSS)/libexec/" \
		"$(PTXCONF_SYSROOT_CROSS)/bin/" \
		"$(PTXCONF_SYSROOT_CROSS)/"*/bin/ \
		-type f \( -executable -o -name "*.so*" \) \
		| xargs -n1 --verbose strip || true
	find \
		"$(PTXCONF_SYSROOT_CROSS)/" \
		-type f \( -executable -o -name "*.so*" \) \
		| xargs chrpath -k -d 2>/dev/null || true
	@$(call touch)

world: $(STATEDIR)/world.cleanup

# vim: syntax=make
