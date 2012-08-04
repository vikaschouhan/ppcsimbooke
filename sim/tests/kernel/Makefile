# Replace this with the prefix of your cross-gcc prefix
PREFIX := powerpc-e500-eabi
#PREFIX := powerpc-unknown-linux-gnu

CC := $(PREFIX)-gcc
AS := $(PREFIX)-as
LD := $(PREFIX)-ld
OBJDUMP := $(PREFIX)-objdump

PROJ_FILE := e500
SRCROOT   := ../..

TEXT_BASE := 0xfffff000
DATA_BASE :=

LDFLAGS := -L -n -N -nostdlib -Wl,-Ttext=$(TEXT_BASE) -Wl,-Map=$(PROJ_FILE).map -Wa,-me500x2 -Wa,-mspe
CFLAGS := -mregnames -Wa,-me500x2 -Wa,-mspe -I. -I$(SRCROOT)/inc
EXTCFLAGS := -D__ASSEMBLY__
EXTLDFLAGS :=
# extra -D flags
EXTFLAGS := -DCONFIG_E500 -DCONFIG_BOOKE -DCONFIG_FSL_EMB_PERFMON

$(PROJ_FILE).elf: $(PROJ_FILE).o
	$(CC) $(LDFLAGS) $(EXTFLAGS) $(EXTLDFLAGS) -o $@ $^

$(PROJ_FILE).o: $(PROJ_FILE).S
	$(CC) $(CFLAGS) $(EXTFLAGS) $(EXTCFLAGS) -c -o $@ $^

all: $(PROJ_FILE).elf
	echo "Making all"

clean:
	rm -rf *.o *.elf *.map

distclean:
	rm -rf *.o *.elf *.map

dump:
	$(OBJDUMP) -hrt $(PROJ_FILE).elf

dis:
	$(OBJDUMP) -xD $(PROJ_FILE).elf
