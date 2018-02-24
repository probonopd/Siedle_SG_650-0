#include <common.h>
#include <image-metadata.h>
#include <generated/compile.h>
#include <generated/utsrelease.h>
#include <generated/sss_version.h>

/*
 * Mark a imd entry as used so that the linker cannot
 * throw it away.
 */
void imd_used(const void *used)
{
}

struct imd_header imd_start_header
__BAREBOX_IMD_SECTION(.barebox_imd_start) = {
	.type = cpu_to_le32(IMD_TYPE_START),
};

struct imd_header imd_end_header
__BAREBOX_IMD_SECTION(.barebox_imd_end) = {
	.type = cpu_to_le32(IMD_TYPE_END),
};

BAREBOX_IMD_TAG_STRING(imd_build_tag, IMD_TYPE_BUILD, UTS_VERSION, 1);
BAREBOX_IMD_TAG_STRING(imd_release_tag, IMD_TYPE_RELEASE, UTS_RELEASE, 1);
/* Siedle proprietary version tag */
BAREBOX_IMD_TAG_STRING(sss_version_tag, IMD_TYPE_PARAMETER, SSS_VERSION_TAG "=" SSS_VERSION, 1);
