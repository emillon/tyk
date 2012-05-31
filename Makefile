EXEC=tyk
OBJ=tyk.o

CC=gcc
CFLAGS=-Wall -Wextra
LIBS=
LDFLAGS=
RM=rm -f

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $+

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXEC)
