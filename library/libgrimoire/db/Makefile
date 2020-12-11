OBJS = $(patsubst %c, %o, $(wildcard *.c))

.c.o :
	@$(CC) $(CFLAGS) $< -o $@
	@$(AR) rcs $(ARCHIVE) $@

all : $(OBJS)

clean :
	@rm -f *.o

.PHONY : all clean
