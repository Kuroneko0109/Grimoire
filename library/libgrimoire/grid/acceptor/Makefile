OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

all :	$(OBJS)

.c.o :
	@$(CC) $(CFLAGS) -o $@ $<
	@$(AR) rcs $(LIBRARY_ARCHIVE) $@

clean :
	@rm -f *.o

.PHONY: clean all
