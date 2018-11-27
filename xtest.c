#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <X11/xpm.h>

#include <stdio.h>
#include <errno.h>

#define ZOOM_X	100
#define ZOOM_Y  100

int zoom_factor = 2;
extern int errno;
extern long TimerCount;
extern unsigned long colors[256];

main(int argc, char *argv[])
{
    Display *dis;
    Window win;			/* Наше окно */
    XEvent event;		/* События */
    GC gc;			/* Графический контекст */
    XSetWindowAttributes SetWinAttr;
    XImage *image, *newimage, *shape;
    XpmAttributes xpm;
    int screen, depth, err, width, height;
    unsigned char *data;
    Visual *vis;
    unsigned long ValueMask;
    char file_name[128], buffer[256];
    int x = ZOOM_X;
    int y = ZOOM_Y;
    int minor, major, ispixmap;
    long now1, now2;

#if 1
    if (argc == 1) {
	printf("%s file\n", argv[0]);
	exit(1);
    } else {
	strcpy(file_name, argv[1]);
    }
#endif

    if ((dis = XOpenDisplay(getenv("DISPLAY"))) == NULL) {
	printf("Can't connect X server: %s\n", strerror(errno));
	exit(1);
    }

#if 1
    data = decode_jpeg(file_name, &width, &height);
    if (data == NULL) {
	fprintf(stderr, "unable to decode JPEG");
	exit(EXIT_FAILURE);
    }


    image = CreateImageFromBuffer(dis, data, width, height);
    if (image == NULL) {
	printf("Can't create image\n");
	exit(EXIT_FAILURE);
    }
    free(data);
#endif

    win = XCreateSimpleWindow(dis, RootWindow(dis, DefaultScreen(dis)),
			      0, 0, 1024, 768,
			      5, BlackPixel(dis, DefaultScreen(dis)),
			      WhitePixel(dis, DefaultScreen(dis)));

    sprintf(buffer, "%s: %s", argv[0],
	    (strlen(file_name) > 0) ? file_name : "None");
    XSetStandardProperties(dis, win, buffer, argv[0], None, argv, argc,
			   NULL);

    gc = DefaultGC(dis, DefaultScreen(dis));

#if 1
    data = decode_jpeg(file_name, &width, &height);
    if (data == NULL) {
	fprintf(stderr, "unable to decode JPEG");
	exit(EXIT_FAILURE);
    }


    image = CreateImageFromBuffer(dis, data, width, height);
    if (image == NULL) {
	printf("Can't create image\n");
	exit(EXIT_FAILURE);
    }
    free(data);
#else
    xpm.closeness = 40000;
    xpm.valuemask = XpmSize | XpmCloseness;
    err = XpmReadFileToImage(dis, file_name, &image, &shape, &xpm);
    if (err == XpmPixmapOpenFailed) {
	printf("Pixmap open failed\n");
	return 1;
    } else if (err == XpmPixmapFileInvalid) {
	printf("Pixmap file invalid\n");
	return 1;
    } else if (err == XpmPixmapNoMemory) {
	printf("No memory for pixmap\n");
	return 1;
    } else
	printf("Pixmap open success\n");
#endif

    init_timer(0, 10000);	/* 10000 микросекунд */
    newimage = ScaleImage(dis, image, x, y);


    XMapWindow(dis, win);
    XSelectInput(dis, win, ExposureMask | KeyPressMask | KeyReleaseMask |
		 ButtonPressMask);
    while (1) {
	XNextEvent(dis, &event);
	if (event.xany.window == win) {
	    switch (event.type) {
	    case Expose:
		XPutImage(dis, win, gc, newimage, 0, 0, 20,
			  20, newimage->width, newimage->height);
              printf("Resampling for %ld milliseconds\n", TimerCount - now1);
/*
		for (x = 0; x < 256; x++) {
		    XSetForeground(dis, gc, colors[x]);
    		    XFillRectangle(dis, win, gc, 0, x * 10, 1024,10);
		}
*/
		break;

	    case KeyPress:
		if (XLookupKeysym(&event.xkey, 0) == XK_q) {
		    XDestroyImage(image);
		    XDestroyImage(newimage);
		    XCloseDisplay(dis);
		    exit(EXIT_SUCCESS);
		}
		break;

	    case KeyRelease:
		break;

	    case ButtonPress:
#if 1
		if (event.xbutton.button == 0x4) {	/* Меньше */
		    if ((x <= 1) || (y <= 1))
			break;
		    XClearArea(dis, win, 20, 20, newimage->width,
			       newimage->height, True);
		    now1 = TimerCount;
    		    x--, y--;			   
		    newimage = ScaleImage(dis, image, x, y);
		    printf("Resampling for %ld milliseconds\n",
			   TimerCount - now1);
		    printf("Меньше: x = %d y = %d\n", x, y);
		}

		if (event.xbutton.button == 0x5) {	/* Больше */
		    if ((x >= 250) || (y >= 250))
			break;
		    XClearArea(dis, win, 20, 20, newimage->width,
			       newimage->height, True);
		    now1 = TimerCount;
		    x++, y++;
		    newimage = ScaleImage(dis, image, x, y);
		    printf("Resampling for %ld milliseconds\n",
			   TimerCount - now1);
		    printf("Больше: x = %d y = %d\n", x, y);
		}
#endif
		break;

	    default:
		break;
	    }
	}
    }
}
