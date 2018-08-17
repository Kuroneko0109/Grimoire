TOP_DIR = $(shell pwd)
LIBRARY = $(TOP_DIR)/library
OUTPUT_GLOBAL = $(shell pwd)/output
LIBRARY_ARCHIVE = $(OUTPUT_GLOBAL)/grimoire
export TOP_DIR
export LIBRARY
export LIBRARY_ARCHIVE
export OUTPUT_GLOBAL

all : library program
	@$(MAKE) -C $(OUTPUT_GLOBAL)

include $(TOP_DIR)/build.mk

library : output
	@$(MAKE) -C $(LIBRARY)

clean :
	@for dir in test $(OUTPUT_GLOBAL) $(LIBRARY);	\
	do	\
		$(MAKE) -C $$dir clean;	\
	done
	@rm -rf $(OUTPUT_GLOBAL)

.PHONY : all clean test library output
