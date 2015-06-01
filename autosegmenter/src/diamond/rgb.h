#ifndef __RGB_H__
#define __RGB_H__

struct RGBImage {
	uint32_t type;
	uint32_t nbytes;		/* size of this struct */
	int32_t height;
	int32_t width;
	uint32_t data[0];		/* variable length */
};

#endif
