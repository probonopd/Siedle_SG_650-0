#include <common.h>

#include "parseopt.h"

void parseopt_hu(const char *options, const char *opt, unsigned short *val)
{
	const char *start;
	size_t optlen = strlen(opt);
	ulong v;
	char *endp;

again:
	start = strstr(options, opt);

	if (!start)
		return;

	if (start > options && start[-1] != ',') {
		options = start;
		goto again;
	}

	if (start[optlen] != '=') {
		options = start;
		goto again;
	}

	v = simple_strtoul(start + optlen + 1, &endp, 0);
	if (v > USHORT_MAX)
		return;

	if (*endp == ',' || *endp == '\0')
		*val = v;
}
