
CC_DINGOO=mipsel-linux-gcc


CLIBS_DINGOO= -Wl,-rpath, -lSDL -lSDL_image -lpthread


INCLUDE_DINGOO=


CFLAGS_DINGOO=


TARGET_DINGOO=vmudingoo.dge

FILES=main.c vms/cpu.c vms/dingoo.c selector/selector.c

all: dingoo

dingoo:
	$(CC_DINGOO) $(INCLUDE_DINGOO) $(CFLAGS_DINGOO) $(CLIBS_DINGOO) \
			$(FILES) -o $(TARGET_DINGOO)

clean:
	rm $(TARGET) $(TARGET_DINGOO)
