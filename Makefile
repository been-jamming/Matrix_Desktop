CC = gcc
CFLAGS = -mwindows -O3

default: qdbmp.c matrix_desktop.c
	$(CC) qdbmp.c matrix_desktop.c $(CFLAGS) -o matrix_desktop

clean: matrix_desktop.exe
	del matrix_desktop.exe

