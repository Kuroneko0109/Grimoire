OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

CFLAGS = -c -Wall -fpic -I$(LIBRARY)

#EXTLIBS = -lpthread -lssl

all : $(OBJS)
	@cp $(OBJS) $(OUTPUT_GLOBAL)/

.c.o :
	@$(CC) $(CFLAGS) -o $@ $<

clean :
	@rm -f *.o

.PHONY: clean all
