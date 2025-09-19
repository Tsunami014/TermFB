CC=gcc
CFLAGS=

ifeq ($(OS),Windows_NT)
    OUT=termfb.exe
    RM=del $(OUT)
else
    OUT=termfb
    RM=rm -rf $(OUT)
endif

all: build

build:
	$(CC) $(CFLAGS) -o $(OUT) main.c getch.c listdir.c textList.c render.c renderStructs.c actions.c config.c dirList.c

debug: CFLAGS+=-DDEBUG_ON
debug: build

clean:
	$(RM)

