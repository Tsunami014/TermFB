CC=gcc
CFLAGS=
OUT=a.out

all: build

build:
	$(CC) $(CFLAGS) -o $(OUT) main.c getch.c listdir.c textList.c render.c renderStructs.c actions.c config.c dirList.c

debug: CFLAGS+=-DDEBUG_ON
debug: build

clean:
	rm -rf $(OUT)

