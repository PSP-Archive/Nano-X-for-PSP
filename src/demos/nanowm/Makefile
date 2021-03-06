##############################################################################
# Microwindows template Makefile
# Copyright (c) 2000 Martin Jolicoeur, Greg Haerr
##############################################################################

ifndef MW_DIR_SRC
MW_DIR_SRC := $(CURDIR)/../..
endif
MW_DIR_RELATIVE := demos/nanowm/
include $(MW_DIR_SRC)/Path.rules
include $(CONFIG)

######################## Additional Flags section ############################

# Directories list for header files
INCLUDEDIRS +=
# Defines for preprocessor
DEFINES +=

# Compilation flags for C files OTHER than include directories
CFLAGS +=
# Preprocessor flags OTHER than defines
CPPFLAGS +=
# Linking flags
LDFLAGS +=

############################# targets section ################################

ifeq ($(NANOX), Y)
ifeq ($(NANOWM), Y)

# If you want to create a library with the objects files, define the name here
LIBNAME =

# List of objects to compile
OBJS := \
	$(MW_DIR_OBJ)/demos/nanowm/nanowm.o \
	$(MW_DIR_OBJ)/demos/nanowm/events.o \
	$(MW_DIR_OBJ)/demos/nanowm/wlist.o \
	$(MW_DIR_OBJ)/demos/nanowm/clients.o \
	$(MW_DIR_OBJ)/demos/nanowm/actions.o \
	$(MW_DIR_OBJ)/demos/nanowm/assert.o

ifeq ($(ARCH), ECOS)
TARGET = $(MW_DIR_BIN)/nanowm.o
else
TARGET = $(MW_DIR_BIN)/nanowm
endif

all: default $(TARGET)

endif
endif

######################### Makefile.rules section #############################

include $(MW_DIR_SRC)/Makefile.rules

######################## Tools targets section ###############################
ifeq ($(SHAREDLIBS), Y)
$(MW_DIR_BIN)/nanowm: $(OBJS) $(NANOXCLIENTLIBS) $(CONFIG)
	@echo "Linking $(patsubst $(MW_DIR_BIN)/%,%,$@) ..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(CCNANOXCLIENTLIBS)
else
#ifeq ($(ARCH), ECOS)
#$(MW_DIR_BIN)/nanowm.o: $(OBJS) $(CONFIG)
#	@echo "Linking $(patsubst $(MW_DIR_BIN)/%,%,$@) ..."
#	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -Wl,-r -Wl,--defsym -Wl,nanowm_main=main -o XX.o
#	$(NM) XX.o | grep -v _main | grep ' T' | awk -f $(MW_DIR_SRC)/ecos/retain.awk | xargs $(OBJCOPY) XX.o $@
#	rm -f XX.o
ifeq ($(ARCH), PSP)
$(MW_DIR_BIN)/nanowm: $(OBJS) $(NANOXCLIENTLIBS) $(CONFIG)
	@echo "Linking $(patsubst $(MW_DIR_BIN)/%,%,$@) ..."
	psp-ld -O0 -G0 -L/usr/local/pspdev/lib/gcc/psp/4.0.0 -L/usr/local/pspdev/psp/sdk/lib -Ttext 8900000 -q  -o $@ $(OBJS) /root/tmp/crt0/crt0.o $(NANOXCLIENTLIBS)$(NANOXCLIENTLIBS) -lpspdebug -lpsplibc -lpspkernel -lgcc
else
$(MW_DIR_BIN)/nanowm: $(OBJS) $(NANOXCLIENTLIBS) $(CONFIG)
	@echo "Linking $(patsubst $(MW_DIR_BIN)/%,%,$@) ..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(NANOXCLIENTLIBS)
endif
endif
