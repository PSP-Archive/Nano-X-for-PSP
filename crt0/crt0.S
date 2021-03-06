# Mostly written by tyranid inspired by nem's work

		.set noreorder

		.text

		.extern main

#include "as_reg_compat.h"


##############################################################################

		# Support for _init() and _fini().
		.global _init
		.global _fini
		#.global _main_thread_attr
		.type	_init, @function
		.type	_fini, @function

		# The .weak keyword ensures there's no error if
		# _init/_fini aren't defined.
		.weak	_init
		.weak	_fini
		
		.weak   _main_thread_attr

		.global _start
		.ent _start

_start:
		addiu 	$sp, -0x10
		sw	$ra, 0($sp)	
		sw	$s0, 4($sp)
		sw	$s1, 8($sp)

		move	$s0, $a0				# Save args
		move	$s1, $a1

#		jal	kmain
#		nop

		la  	$a0, _main_thread_name	# Main thread setup
		la		$a1, _main
		li		$a2, 0x20				# Priority
		li		$a3, 0x40000			# Stack size
#		lui		$t0, 0x8000				# Attributes
#		la		$t0, _main_thread_attr

                la              $t0, _main_thread_attr
                bne             $t0, $0, 1f
                nop
                j               2f
                lui             $t0, 0x8000                             # Attributes
1:
                lw              $t0, 0($t0)
2:
		jal		sceKernelCreateThread
		move	$t1, $0

		move	$a0, $v0				# Start thread
		move	$a1, $s0
		jal		sceKernelStartThread
		move	$a2, $s1

		lw		$ra, 0($sp)
		lw		$s0, 4($sp)
		lw		$s1, 8($sp)
		move	$v0, $0
		jr 		$ra
		addiu	$sp, 0x10

		.end _start

# Second stage main function
		.ent _main
_main:
		# Do mainy stuff ;)
		addiu	$sp, -0x10
		sw		$ra, 0($sp)

		la		$t0, g_elf_namesize
		sw		$a0, 0($t0)
		la		$t0, g_elf_name
		sw		$a1, 0($t0)
		
		la		$t0, _fbss
		la		$t1, _end

# Clear BSS
	loop:
		sw		$0,0($t0)
		sltu	$v0, $t0, $t1
		bne		$v0, $0, loop
		addiu	$t0, $t0, 4

		la		$gp, _gp
# Call libc initialisation

		jal		_pspsdk_libc_init
		nop

		la		$t0, _init
		beqz	$t0, 1f
		nop
		jalr	$t0
		nop
1:
		jal		main
		nop

		.end _main

		.global _exit
		.ent _exit

_exit:
# Exit
		la		$t0, _fini
		beqz	$t0, 2f
		nop

		jalr	$t0
		nop
2:

# Call libc deinit
		jal _pspsdk_libc_deinit
		nop

		lw		$ra, 0($sp)
		jr		$ra
		addiu	$sp, 0x10
			
		.end _exit

_main_thread_name:
		.asciiz	"user_main"
##############################################################################


		.section	.lib.ent,"wa",@progbits
__lib_ent_top:
		.word 0
		.word 0x80000000
		.word 0x00010104
		.word __entrytable


		.section	.lib.ent.btm,"wa",@progbits
__lib_ent_bottom:
		.word	0


		.section	.lib.stub,"wa",@progbits
__lib_stub_top:


		.section	.lib.stub.btm,"wa",@progbits
__lib_stub_bottom:
		.word	0


##############################################################################

		.section	".xodata.sceModuleInfo","wa",@progbits

__moduleinfo:
		.word	0x01011006
#		.word	0x01011000

		.ascii	"PSPSDK"		#up to 28 char
		.align	5

		.word	_gp
		.word	__lib_ent_top
		.word	__lib_ent_bottom
		.word	__lib_stub_top
		.word	__lib_stub_bottom

##############################################################################

		.section	.rodata.entrytable,"a",@progbits
__entrytable:
		.word 0xD632ACDB
		.word 0xF01D73A7
		.word _start
		.word __moduleinfo
#		.word module_info
		.word 0



###############################################################################

		.data
		.global g_elf_name
g_elf_name: .word 0
		.global g_elf_namesize
g_elf_namesize: .word 0
		.global g_sr
g_sr:		.word 0

###############################################################################

		.bss
