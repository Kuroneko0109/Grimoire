CFLAGS = -c -g -Wall -W -fPIC -I$(LIBRARY)
export CFLAGS

output :
	@mkdir -p $(OUTPUT_GLOBAL)
	@cp -r tools/Makefile.so $(OUTPUT_GLOBAL)/Makefile

program :
