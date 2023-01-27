SHELL = /bin/sh

NAME=snes_mapper
PREFIX=/usr/local
CFLAGS=-ansi -Wall -Wextra -std=c89 -pedantic -O2 -D_POSIX_C_SOURCE=200809L
LIBSPATH=-L/usr/local/lib
INCLUDES=-I/usr/local/include
LIBS=-s -lm -lxcb -lxcb-xtest
CC=cc

build: snes_mapper.c
	${CC} ${CFLAGS} snes_mapper.c \
	${INCLUDES} ${LIBSPATH} -o ${NAME} ${LIBS}

install:
	cp ${NAME} ${PREFIX}/bin/
	chmod 755 ${PREFIX}/bin/${NAME}

uninstall:
	rm ${PREFIX}/bin/${NAME}

clean:
	rm -f ${NAME}
