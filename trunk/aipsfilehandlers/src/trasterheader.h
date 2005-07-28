/*
 * $Id: trasterheader.h,v 1.1.1.1 2004/10/08 07:31:55 belitz Exp $
 */

#ifndef _TRasterHeader_h_
#define _TRasterHeader_h_

/* definitions of types (machine dependent declarations) */
#include <X11/Xmd.h>

#define RASTER_FILE_VERSION		1
#define sz_RasterHeader			(9*sizeof(CARD32)+256)
#define RASTER_FILE_MAGIC		0xDECC3004

typedef struct _raster_file_header {
	CARD32	magic B32;
	CARD32	file_version B32;
	CARD32	data_type B32;		/* 0=uint, 1=int, 2=float */
	CARD32	byte_order B32;		/* MSBFirst, LSBFirst */
	CARD32	data_type_size B32;	/* size of data type (e.g. 4 bytes for a float on i386) */
	CARD32	width B32;			/* width of each slice */
	CARD32	height B32;			/* height of each slice */
	CARD32	depth B32;			/* number of slices in file */
	CARD32	crc32;				/* crc32 checksum of data*/
	char	description[256];		/* short discription of data */
} TRasterFileHeader;

/* sizeof data = width*height*depth*data_type_size */

#endif /* _TRasterHeader_h_ */

