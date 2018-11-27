#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <jpeglib.h>
#include <jerror.h>
#include <math.h>


#define round(x)  	((long)(x + 0.5) > x)? (long)(x + 0.5) : (long)(x + 1.5)

void PrintImageData(XImage * image)
{
    printf("������ (width):   \t\t%d\n"
	   "������ (height):  \t\t%d\n"
	   "�������� �� X (xoffset): \t%d\n"
	   "������ (format):  \t\t%s\n"
	   "������� ���� (byte_order): \t%s\n"
	   "C�������� (bitmap_unit): \t%d\n"
	   "������� ��� (bitmap_bit_order): %s\n"
	   "�������� (bitmap_pad): \t\t%d\n"
	   "������� (depth): \t\t%d\n"
	   "bytes_per_line:  accelarator to next line %d\n"
	   "��� � ������� (bits_per_pixel): %d\n"
	   "red_mask:    %d\n"
	   "green_mask:  %d\n"
	   "blue_mask:   %d\n",
	   image->width, image->height, image->xoffset,
	   (image->format ==
	    XYBitmap) ? "XYBitmap" : ((image->format ==
				       XYPixmap) ? "XYPixmap" : "ZPixmap"),
	   (image->byte_order == LSBFirst) ? "LSBFirst" : "MSBFirst",
	   image->bitmap_unit,
	   (image->bitmap_bit_order == LSBFirst) ? "LSBFirst" : "MSBFirst",
	   image->bitmap_pad, image->depth, image->bytes_per_line,
	   image->bits_per_pixel, image->red_mask, image->green_mask,
	   image->blue_mask);
}


/* ������������ �� 90 �������� */
XImage *RotateImage(Display * dis, XImage * src)
{
    XImage *image;
    int depth, screen, numNewBufBytes;
    unsigned char *data;	/* ����� ��� ������� */
    int numNewBytes, bpp;
    Visual *vis;
    int x, y, i, j;
    unsigned long pixel;

    depth = DefaultDepth(dis, DefaultScreen(dis));
    vis = DefaultVisual(dis, DefaultScreen(dis));

    bpp = (depth >= 24) ? 4 : ((depth >= 15) ? 2 : 1);

    numNewBufBytes = bpp * src->width * src->height;

    printf("malloc: %d x %d bytes need allocate %d\n", src->width,
	   src->height, numNewBufBytes);

    data = malloc(numNewBufBytes);
    if (data == NULL) {
	perror("malloc");
	return NULL;
    }

    image = XCreateImage(dis, vis, depth, ZPixmap, 0,
			 data, src->height,
			 src->width,
			 (depth >= 24) ? 32 : ((depth >= 15) ? 16 : 8),
			 bpp * src->height);
    XInitImage(image);

    /* ����� ���������������� */
    for (i = 0; i < src->width; i++) {
	for (j = 0; j < src->height; j++) {
	    pixel = src->f.get_pixel(src, src->width - i, j);
	    image->f.put_pixel(image, j, i, pixel);
	}
    }
    return image;
}


/* ������� ������ �� ������� �������� �����-����� */
void ColorToBWImage(XImage * image)
{
    unsigned char r, g, b;
    unsigned long pixel;
    unsigned long mid;		/* ����� �������� �������� �������  */
    int x, y, i, j;


    for (i = 0; i < image->width; i++) {
	for (j = 0; j < image->height; j++) {
	    pixel = XGetPixel(image, i, j);
	    /* 
	     * �������� ��� ����� ��� ��������� �����-������ ������� 
	     */

	    if (image->depth == 8) {
		/* ������� ������� */
		r = (pixel >> 5) & 0x3;
		/* ������� ������� */

		g = (pixel >> 2) & 0x7;

		/* ������� ����� */
		b = pixel & 0x03;

		pixel = (r + b + g) / 3;
		pixel = (pixel << 5) | (pixel << 2) | (pixel);

	    } else if (image->depth == 15) {
		/* ������� ������� */
		r = (pixel) >> 10;
		/* ������� ������� */

		g = (pixel >> 5) & 0x01f;

		/* ������� ����� */
		b = pixel & 0x01f;

		pixel = (r + b + g) / 3;
		pixel = (pixel << 10) | (pixel << 5) | (pixel);

	    } else if (image->depth == 16) {
		/* ������� ������� */
		r = (pixel) >> 11;
		/* ������� ������� */

		g = (pixel >> 5) & 0x03f;

		/* ������� ����� */
		b = pixel & 0x01f;

		
		pixel = (r + ((g >> 1) - 1) + b) / 3 ;
		
		pixel = (pixel << 11) | (((pixel << 1) + 2) << 5) | (pixel);
	    }

	    if (image->depth >= 24) {
		/* ������� ������� */
		r = (pixel) >> 16;
		/* ������� ������� */

		g = (pixel & 0x0ff00) >> 8;
		/* ������� ����� */
		b = pixel & 0x0ff;

		pixel = (r + b + g) / 3;

		pixel = (pixel << 16) | (pixel << 8) | (pixel);
	    } else;
	    XPutPixel(image, i, j, pixel);
	}
    }
}


/* ������� ������ ������� */
void InvertImage(XImage * image)
{
    unsigned long r, g, b;
    unsigned long pixel;
    unsigned long mid;		/* ����� �������� �������� �������  */
    int x, y, i, j;

    for (i = 0; i < image->width; i++) {
	for (j = 0; j < image->height; j++) {
	    pixel = XGetPixel(image, i, j);

	    /* 
	     * ������� ����������
	     */
	    if (image->depth == 8) {
		/* ������� ������� */
		r = (pixel >> 5) & 0x3;

		/* ������� ������� */
		g = (pixel >> 2) & 0x7;

		/* ������� ����� */
		b = pixel & 0x03;
		pixel = (r << 5) | (g << 2) | (b);
	    } else if (image->depth == 15) {
		/* ������� ������� */
		r = (pixel) >> 10;
		r = 31 - r;

		/* ������� ������� */
		g = (pixel >> 5) & 0x01f;
		g = 31 - g;

		/* ������� ����� */
		b = pixel & 0x01f;
		b = 31 - b;

		pixel = (r << 10) | (g << 5) | (b);

	    } else if (image->depth == 16) {
		/* ������� ������� */
		r = (pixel) >> 11;
		r = 31 - r;

		/* ������� ������� */
		g = (pixel >> 5) & 0x03f;
		g = 63 - g;

		/* ������� ����� */
		b = pixel & 0x01f;
		b = 32 - b;

		pixel = (r << 11) | (g << 5) | (b);
	    } else if (image->depth >= 24) {
		/* ������� ������� */
		r = (pixel) >> 16;
		r = 255 - r;

		/* ������� ������� */
		g = (pixel & 0x0ff00) >> 8;
		g = 255 - g;

		/* ������� ����� */
		b = pixel & 0x0ff;
		b = 255 - b;

		pixel = (r << 16) | (g << 8) | (b);
	    } else;

	    XPutPixel(image, i, j, pixel);
	}
    }
}

/* ���������������� ����������� �� x_percent � y_percent */
XImage *ScaleImage(Display * dis, XImage * src, int x_percent, int y_percent)
{
    XImage *image;
    int depth, num_bytes, bpp;
    Visual *vis;
    double x_scale, y_scale;
    int i = 0, j = 0, n, m;
    double x = 0, y = 0;
    unsigned char *buf;		/* ����� ��� ������� */
    int new_width, new_height;
    unsigned long pix_0, pix_1, pixel, pix_r, pix_g, pix_b;	/* ���������� ������� */
    unsigned char r0, r1, g0, g1, b0, b1;	/* ���������� ������� */


    x_scale = x_percent / 100.0;
    y_scale = y_percent / 100.0;
    depth = DefaultDepth(dis, DefaultScreen(dis));
    vis = DefaultVisual(dis, DefaultScreen(dis));
    bpp = (depth >= 24) ? 4 : (depth >= 15) ? 2 : 1;

    new_width = (src->width - 1) * x_scale + 1;
    new_height = (src->height - 1) * y_scale + 1;
    num_bytes = bpp * new_width * new_height;

    buf = malloc(num_bytes);
    if (buf == NULL) {
	perror("malloc");
	return NULL;
    }

    bilinear_resample(depth, src->width, src->height, new_width, new_height, src->data, buf); 
    image = XCreateImage(dis, vis, depth, ZPixmap, 0,
			 buf, new_width,
			 new_height,
			 (depth >= 24) ? 32 : ((depth >= 15) ? 16 : 8),
			 bpp * new_width);

    XInitImage(image);
    return image;
}


void jpeg_error_exit(j_common_ptr cinfo)
{
    cinfo->err->output_message(cinfo);
    exit(EXIT_FAILURE);
}

/* ���������� ������ � 24 ������ image.*/
unsigned char *decode_jpeg(char *filename, int *widthPtr, int *heightPtr)
{
    register JSAMPARRAY lineBuf;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr err_mgr;
    int bpp;
    FILE *inFile;
    int fd;
    unsigned char *retBuf;

    inFile = fopen(filename, "rb");
    if (NULL == inFile) {
	perror("open");
	return NULL;
    }

    cinfo.err = jpeg_std_error(&err_mgr);
    err_mgr.error_exit = jpeg_error_exit;

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inFile);
    jpeg_read_header(&cinfo, 1);
    cinfo.do_fancy_upsampling = 0;
    cinfo.do_block_smoothing = 0;
    jpeg_start_decompress(&cinfo);

    *widthPtr = cinfo.output_width;
    *heightPtr = cinfo.output_height;
    bpp = cinfo.output_components;

    lineBuf =
	cinfo.mem->alloc_sarray((j_common_ptr) & cinfo, JPOOL_IMAGE,
				(*widthPtr * bpp), 1);
    retBuf = malloc(3 * (*widthPtr * *heightPtr));

    if (NULL == retBuf) {
	perror(NULL);
	return NULL;
    }

    if (3 == bpp) {
	int lineOffset = (*widthPtr * 3);
	int x;
	int y;

	for (y = 0; y < cinfo.output_height; ++y) {
	    jpeg_read_scanlines(&cinfo, lineBuf, 1);

	    for (x = 0; x < lineOffset; ++x) {
		retBuf[(lineOffset * y) + x] = lineBuf[0][x];
		++x;
		retBuf[(lineOffset * y) + x] = lineBuf[0][x];
		++x;
		retBuf[(lineOffset * y) + x] = lineBuf[0][x];
	    }
	}
    } else if (1 == bpp) {
	unsigned int col;
	int lineOffset = (*widthPtr * 3);
	int lineBufIndex;
	int x;
	int y;

	for (y = 0; y < cinfo.output_height; ++y) {
	    jpeg_read_scanlines(&cinfo, lineBuf, 1);

	    lineBufIndex = 0;
	    for (x = 0; x < lineOffset; ++x) {
		col = lineBuf[0][lineBufIndex];
		retBuf[(lineOffset * y) + x] = col;
		++x;
		retBuf[(lineOffset * y) + x] = col;
		++x;
		retBuf[(lineOffset * y) + x] = col;
		++lineBufIndex;
	    }
	}
    } else {
	fprintf(stderr,
		"Error: the number of color channels is %d.  This program only handles 1 or 3\n",
		bpp);
	return NULL;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(inFile);

    printf("bytes per pixel = %d\n", bpp);
    return retBuf;
}

XImage *CreateImageFromBuffer(Display * dis,
			      unsigned char *buf, int width, int height)
{
    int depth, screen;
    XImage *img = NULL;
    Visual *vis;
    double rRatio;
    double gRatio;
    double bRatio;
    int outIndex = 0;
    int i;
    int numBufBytes = (3 * (width * height));

    screen = DefaultScreen(dis);
    depth = DefaultDepth(dis, screen);
    vis = DefaultVisual(dis, screen);

    rRatio = vis->red_mask / 255.0;
    gRatio = vis->green_mask / 255.0;
    bRatio = vis->blue_mask / 255.0;

    if (depth >= 24) {
	size_t numNewBufBytes = (4 * (width * height));
	int32_t *newBuf = malloc(numNewBufBytes);

	for (i = 0; i < numBufBytes; ++i) {
	    unsigned int r = 0, g = 0, b = 0;

	    r = (buf[i] * rRatio);
	    ++i;
	    g = (buf[i] * gRatio);
	    ++i;
	    b = (buf[i] * bRatio);

	    r &= vis->red_mask;
	    g &= vis->green_mask;
	    b &= vis->blue_mask;

	    newBuf[outIndex] = r | g | b;
	    ++outIndex;
	}
	img = XCreateImage(dis,
			   CopyFromParent, depth,
			   ZPixmap, 0, (char *) newBuf, width, height, 32,
			   0);

    } else if (depth >= 15) {
	size_t numNewBufBytes = (2 * (width * height));
	int16_t *newBuf = malloc(numNewBufBytes);

	for (i = 0; i < numBufBytes; ++i) {
	    unsigned int r, g, b;

	    r = (buf[i] * rRatio);
	    ++i;
	    g = (buf[i] * gRatio);
	    ++i;
	    b = (buf[i] * bRatio);

	    r &= vis->red_mask;
	    g &= vis->green_mask;
	    b &= vis->blue_mask;

	    newBuf[outIndex] = r | g | b;
	    ++outIndex;
	}
	img = XCreateImage(dis,
			   CopyFromParent, depth,
			   ZPixmap, 0, (char *) newBuf, width, height, 32,
			   0);

    } else {
	size_t numNewBufBytes = width * height;
	unsigned char *newBuf = malloc(numNewBufBytes);

	for (i = 0; i < numBufBytes; ++i) {
	    unsigned int r = 0, g = 0, b = 0;


	    r = (buf[i] * rRatio);
	    ++i;
	    g = (buf[i] * gRatio);
	    ++i;
	    b = (buf[i] * bRatio);

	    r &= vis->red_mask;
	    g &= vis->green_mask;
	    b &= vis->blue_mask;

	    newBuf[outIndex] = r | g | b;
	    ++outIndex;
	}
	printf("OK\n");
	img = XCreateImage(dis,
			   CopyFromParent, depth,
			   ZPixmap, 0, (char *) newBuf, width, height, 32,
			   0);


//	fprintf(stderr,
//		"This program does not support displays with a depth less than 15.");
    }

    XInitImage(img);
    /* Set the client's byte order, so that XPutImage knows what to do with the data. 
       The default in a new X image is the server's format, which may not be what we want. */

    img->byte_order = get_endian();

    /* The bitmap_bit_order doesn't matter with ZPixmap images. */
    img->bitmap_bit_order = MSBFirst;
    return img;
}


int get_endian(void)
{
    unsigned long a = 0x0001;

    if ((int) *((char *) &a) == 1)
	return (1 - LSBFirst);
    else
	return(1 - MSBFirst);
}
