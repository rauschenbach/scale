#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>


#define BLACK       colors[0]
#define WHITE       colors[1]
#define RED         colors[2]
#define GREEN       colors[3]
#define BLUE        colors[4]
#define GREY   	    colors[5]
#define YELLOW      colors[6]
#define DARK_GREY   colors[7]
#define LIGHT_GREY  colors[8]
#define ORANGE      colors[9]
#define GREY_2      colors[10]

unsigned long colors[256];
static int base_colors[] = {
    0, 0, 0,			/* black */
    0xFFFF, 0xFFFF, 0xFFFF,	/* white */
    0xFFFF, 0x38E3, 0x820,	/* red */
    0x28A2, 0xCF3C, 0x820,	/* green */

    0, 0, 0xFFFF,		/* blue */
    0x8617, 0x9A69, 0xA699,	/* grey */
    0xFFFF, 0xBEFB, 0x4103,	/* yellow */
    0x30C2, 0x4514, 0x5144,	/* dark_grey */

    0x6185, 0x8A28, 0x8617,	/* light_grey */
    0xF7F7, 0x5151, 0x808,	/* orange */
    0x38E3, 0x5555, 0x6185	/* grey_2 */
};

/* Загрузить палитру */
void LoadColormap(Display * dis, Window win)
{
    int visnum, depth, i, j;
    Colormap colormap;
    Visual *visual;
    VisualID visid;
    XVisualInfo vinfo;
    XVisualInfo *result;
    XColor color;

    visual = DefaultVisual(dis, DefaultScreen(dis));
    visid = XVisualIDFromVisual(visual);
    vinfo.visualid = visid;
    result = XGetVisualInfo(dis, VisualIDMask, &vinfo, &visnum);

    depth = result->class;

    if (depth == TrueColor) {
	printf("load truecolor\n");
	colormap = XCreateColormap(dis, win, visual, AllocNone);

	for (i = 0, j = 0; j < 256; i += 3, j++) {
	    if (i < sizeof(base_colors)) {
		color.red = base_colors[i];
		color.green = base_colors[i + 1];
		color.blue = base_colors[i + 2];
	    } else {
		if (j < sizeof(base_colors) / 3 + 100)
		    color.red =
			30000 + (j -
				 sizeof(base_colors) / 3) * (35535 / 100);
		else {
		    color.red = 0;
		    color.green = 0;
		    color.blue = 0;
		}
	    }
	    color.flags = DoRed | DoGreen | DoBlue;
	    XAllocColor(dis, colormap, &color);
	    colors[j] = color.pixel;
	}
    } else if (depth == PseudoColor) {
	printf("load pseudocolor\n");
	colormap = XCreateColormap(dis, win, visual, AllocAll);

	for (i = 0, j = 0; j < 256; i += 3, j++) {
	    if (i < sizeof(base_colors)) {
		color.red = base_colors[i];
		color.green = base_colors[i + 1];
		color.blue = base_colors[i + 2];
	    } else {
		if (j < sizeof(base_colors) / 3 + 10)
		    color.red =
			30000 + (j -
				 sizeof(base_colors) / 3) * (35535 / 10);
		else {
		    color.red = 0;
		    color.green = 0;
		    color.blue = 0;
		}
	    }
	    color.flags = DoRed | DoGreen | DoBlue;
	    color.pixel = j;
	    XStoreColor(dis, colormap, &color);
	    colors[j] = color.pixel;
	}
    } else
	printf("load ??? color\n");

    XSetWindowColormap(dis, win, colormap);
    XInstallColormap(dis, colormap);
}
