CC=gcc
CFLAGS=
OUT=a.out

all: build

build:
	$(CC) $(CFLAGS) -o $(OUT) main.c getch.c listdir.c

debug: CFLAGS+=-DDEBUG_ON
debug: build

clean:
	rm -rf $(OUT)

