CC=gcc
CFLAGS += -lncursesw
TARGETS = ./src/main ./src/gfx
OBJ = ./src/gfx.o ./src/main.o
HEADERS = ./src/gfx.h

slidey: $(OBJ) $(HEADERS)
	$(CC) $^ -o $@ $(CFLAGS) 

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f *-o $(TARGETS)
	rm -f ./slidey
