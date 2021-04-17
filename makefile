CC		:= gcc
PKGS	:= ncursesw
CFLAGS	:= -l
CFILE	:= cgol.c	 

.PHONY: run

run:
	$(CC) $(CFILE) $(CFLAGS) $(PKGS) -o cgol.out
