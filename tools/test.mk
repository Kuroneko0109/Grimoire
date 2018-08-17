CFLAGS = -c -g -Wall -W -fPIC -I$(LIBRARY)
export CFLAGS

test : output library
	$(MAKE) -C $@
	$(MAKE) -C $(OUTPUT_GLOBAL)

output :
	@mkdir -p $@
	@cp -r tools/Makefile.test $@/Makefile

program : test
