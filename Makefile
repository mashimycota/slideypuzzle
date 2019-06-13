CC=gcc
CFLAGS += -lncursesw
TARGETS = ./src/main ./src/gfx
OBJ = gfx.o main.o
HEADERS = ./src/gfx.h

slidey: gfx.o main.o $(HEADERS)
	$(CC) $^ -o $@ $(CFLAGS) 

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f *-o $(TARGETS)
