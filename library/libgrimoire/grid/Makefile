OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

all :	$(OBJS)
	@$(MAKE) -C peer
	@$(MAKE) -C acceptor 

.c.o :
	@$(CC) $(CFLAGS) -o $@ $<
	@$(AR) rcs $(LIBRARY_ARCHIVE) $@

clean :
	@rm -f *.o
	@$(MAKE) -C peer clean
	@$(MAKE) -C acceptor clean

.PHONY: clean all
