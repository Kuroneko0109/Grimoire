DIRS = libgrimoire

$(DIRS) :
	@$(MAKE) -C $@

all : $(DIRS)

clean :
	@for dir in $(DIRS);	\
	do	\
		$(MAKE) -C $$dir clean;	\
	done

.PHONY : $(DIRS) all clean
