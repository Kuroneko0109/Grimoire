BUILD_DIR = grid common config datastructure system delivery chrono db # math nlp #security

all : $(BUILD_DIR)

$(BUILD_DIR) :
	@echo " ====== Build $@ ======"
	@$(MAKE) -C $@

clean :
	@for dir in $(BUILD_DIR) ;	\
	do	\
		echo " ====== clean $$dir ======";	\
		$(MAKE) -C $$dir clean;	\
	done

.PHONY : all clean $(BUILD_DIR)
