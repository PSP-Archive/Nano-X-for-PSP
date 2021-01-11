OBJS=crt0.o assert.o

PSP_CC=psp-gcc

# Include directories
PSP_INCS := -I$(PSPSDK)/include -I. $(PSP_INCS) -I/usr/local/pspdev/psp/sdk/include/

# C compiler flags
#PSP_CFLAGS := -O2 -G0 -Wall -mdivide-breaks -march=r4000 -mgp32 -mlong32 $(PSP_CFLAGS)
PSP_CFLAGS := -O2 -G0 -Wall -mdivide-breaks -mgp32 -mlong32 $(PSP_CFLAGS)

# C++ compiler flags
#PSP_CXXFLAGS := -O2 -G0 -Wall -mdivide-breaks -march=r4000 -mgp32 -mlong32 $(PSP_CXXFLAGS)
PSP_CXXFLAGS := -O2 -G0 -Wall -mdivide-breaks -mgp32 -mlong32 $(PSP_CXXFLAGS)

all:$(OBJS)

crt0.o:crt0.S
	$(PSP_CC) $(PSP_CFLAGS) $(PSP_INCS) -c $< -o $@
#	psp-gcc -I. -I/usr/local/pspdev/include -I/usr/local/pspdev/psp/sdk/include -g -c -xassembler -O -o $@ $^

assert.o:assert.c
	$(PSP_CC) $(PSP_CFLAGS) $(PSP_INCS) -c $< -o $@
#	psp-gcc -c -Wall -O2 -G0 -Wall -mdivide-breaks -mgp32 -mlong32  -I. -I/usr/local/pspdev/include -I/usr/local/pspdev/psp/sdk/include -o $@ $^

.PHONY:clean

clean:
	rm -rf $(OBJS)