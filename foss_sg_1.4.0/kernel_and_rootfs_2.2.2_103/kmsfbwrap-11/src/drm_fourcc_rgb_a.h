#ifndef __DRM_FOURCC_RGB_A_H__
#define __DRM_FOURCC_RGB_A_H__

#include <drm_fourcc.h>

/*
 * If the kernel headers don't include 2 plane RGB + A formats yet,
 * add them manually:
 */

#ifndef DRM_FORMAT_RGB888_A8
/*
 * 2 plane RGB + A
 * index 0 = RGB plane
 * index 1 = A plane
 */
#define DRM_FORMAT_RGBX8888_A8	fourcc_code('R', 'X', 'A', '8')
#define DRM_FORMAT_BGRX8888_A8	fourcc_code('B', 'X', 'A', '8')
#define DRM_FORMAT_RGB888_A8    fourcc_code('R', '8', 'A', '8')
#define DRM_FORMAT_BGR888_A8    fourcc_code('B', '8', 'A', '8')
#define DRM_FORMAT_RGB565_A8    fourcc_code('R', '5', 'A', '8')
#define DRM_FORMAT_BGR565_A8    fourcc_code('B', '5', 'A', '8')
#endif /* DRM_FORMAT_RGB888_A8 */

#endif /* __DRM_FOURCC_RGB_A_H__ */
