CC = gcc
CFLAGS= -DMITSHM
INCLUDES =  -I/usr/X11R6/include -I/usr/X11R6/include/X11 -I./h
LIBS= -L/usr/X11R6/lib -lX11 -lXpm -ljpeg -lm

xtest:	 xtest.o utils.o sampling.o timer.o xfunc.o
	$(CC) -O3 -o  xtest xtest.o utils.o sampling.o timer.o xfunc.o $(CFLAGS) $(INCLUDES) $(LIBS) 
	@rm -f *.*~

xtest.o:	xtest.c
	$(CC) -c xtest.c $(CFLAGS) $(INCLUDES)

utils.o:	utils.c
	$(CC) -c utils.c $(CFLAGS) $(INCLUDES)

xfunc.o:	xfunc.c
	$(CC) -c xfunc.c $(CFLAGS) $(INCLUDES)

sampling.o:	sampling.c
	$(CC) -c sampling.c $(CFLAGS) $(INCLUDES)

timer.o:	timer.c
	$(CC) -c timer.c $(CFLAGS) $(INCLUDES)


clean::
	-rm -f *.o core xtest *~
