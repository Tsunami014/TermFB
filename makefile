OUTNAME=termfb
CC=gcc
CFLAGS=

ifeq ($(OS),Windows_NT)
    OUT=$(OUTNAME).exe
    RM=del $(OUT)
else
    OUT=$(OUTNAME)
    RM=rm -rf $(OUT)
endif

all: build

build:
	$(CC) $(CFLAGS) -o $(OUT) main.c getch.c listdir.c textList.c render.c renderStructs.c actions.c config.c dirList.c

debug: CFLAGS+=-DDEBUG_ON
debug: build

clean:
	$(RM)

