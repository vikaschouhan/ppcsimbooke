# Replace this with the prefix of your cross-gcc prefix
PREFIX := powerpc-e500-eabi
#PREFIX := powerpc-unknown-linux-gnu

CC := $(PREFIX)-gcc
AS := $(PREFIX)-as
LD := $(PREFIX)-ld
OBJDUMP := $(PREFIX)-objdump

SRCROOT := ../..

PROJ_FILE := test_001

LDFLAGS := -L -n -N -nostdlib -T e500v2_eabi_default.lcf -Wl,-Map=$(PROJ_FILE).map -Wa,-me500x2 -Wa,-mspe
CFLAGS := -mregnames -Wa,-me500x2 -Wa,-mspe -I. -I$(SRCROOT)/inc -nostdlib
EXTCFLAGS := -D__ASSEMBLY__
EXTLDFLAGS :=
# extra -D flags
EXTFLAGS := -DCONFIG_E500 -DCONFIG_BOOKE -DCONFIG_FSL_EMB_PERFMON

$(PROJ_FILE).elf: common.o $(PROJ_FILE).o startup.o
	$(CC) $(LDFLAGS) $(EXTFLAGS) $(EXTLDFLAGS) -o $@ $^

%.o: %.S
	$(CC) $(CFLAGS) $(EXTFLAGS) $(EXTCFLAGS) -c -o $@ $^

%.o: %.c
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
