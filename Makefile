#########################################
# Windows flags
#########################################
# CFLAGS = -I../include -pthread -O1
# LIB    = ../lib/win32/libglfw.a
# LFLAGS = midterm.res $(LIB) -glfw -opengl32 -lopengl32 -pthread -lm

#########################################
# Linux flags
#########################################
CFLAGS = -I../include -pthread -O1
LIB    = ../lib/x11/libglfw.a
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm

all: midterm

midterm: midterm.o cfonts.o fonttex.o
	gcc midterm.o cfonts.o fonttex.o $(LFLAGS) -o midterm

midterm.o: midterm.c
	gcc -c $(CFLAGS) midterm.c

cfonts.o: cfonts.c
	gcc $(CFLAGS) -c cfonts.c

fonttex.o: fonttex.c
	gcc $(CFLAGS) -c fonttex.c
  
clean:
	rm -f midterm
	rm -f *.o
