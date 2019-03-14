BUILD_DIR = grid common config datastructure system delivery chrono math #security

all : $(BUILD_DIR)

$(BUILD_DIR) :
	@$(MAKE) -C $@

clean :
	@for dir in $(BUILD_DIR) ;	\
	do	\
		$(MAKE) -C $$dir clean;	\
	done

.PHONY : all clean $(BUILD_DIR)
