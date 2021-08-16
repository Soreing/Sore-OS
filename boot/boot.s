.code16
.text
.org 0x0

.global main

main:
	mov %dl, BOOT_DRIVE			# Save the boot drive number in a global label
	mov $0x7C00, %bp			# Set the stack's base to 0x7C00
	mov %sp, %bp				# Set the stack pointer to the base (empty)
	
	mov $0x7DBE, %bx			# Moves EBX to the Partition Table
	
boot_loader:
	cmpb $0x0C, 4(%bx)			# Checks if the partition contains a FAT32 file system
	jne 1f						# Else a Boot Error is displayed
	
	cmpb $0x00, 3(%bx)			# Checks if the Sector Number of the Partition is nonzero
	je 1f						# Else a Boot Error is displayed
	
	mov 8(%bx), %ax				# Move the LBA low word to DAP_LBA_1 for reading
	mov %ax, DAP_LBA_1			#
	mov 10(%bx), %ax			# Move the LBA high word to DAP_LBA_2 for reading
	mov %ax, DAP_LBA_2			# 
	
	movw $0x7E00, DAP_DST_LOW	# Set the destination address to 0x 0000 7E00
	movw $0x0001, DAP_SECTORS	# Read 1 sector
	call load_disk

	call kernel_loader			# Call the File System's Boot Sector

enable_a20:
	call A20Test				# Test if A20 line is enabled
	jnc load_gdt				# If CF is not set, move to next task
	
	mov $0x2401, %ax			# Select BIOS's enable A20 function
	int $0x15					# Tell BIOS to enable line A20
	
	call A20Test				# Test if A20 line is enabled
	jnc load_gdt				# If CF is not set, move to next task
	
	in $0x92, %al				# Get byte from port 0x92 into %al
	or $2, %al					# Set the 2nd bit of %al
	out %al, $0x92				# Get byte in %al to port 0x92
	
	call A20Test				# Test if A20 line is enabled
	jnc load_gdt				# If CF is not set, move to next task
	jmp 2f
	
load_gdt:
	
	mov $OK_MSG, %bx
	call print_str
	
	cli
	lgdt (gdt_descriptor)
	
	
	mov %cr0, %eax
	or $0x01, %eax
	mov %eax, %cr0
	
	ljmp $0x08, $init_pm
	
.code32
#CODE_SEG=0x00
DATA_SEG: .long (gdt_data - gdt_start)
init_pm:

	mov DATA_SEG, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov $0x90000, %ebp
	mov %ebp, %esp
	
	call 0x8200
	
	jmp .

.code16
1:
	mov $BOOT_ERROR, %bx
	call print_str
	jmp .						# Hangs
	
2:
	mov $A20_ERROR, %bx
	call print_str
	
	jmp .						# Hangs


.include "print_byte.s"
.include "print_str.s"
.include "load_disk_LBA.s"
.include "A20_test.s"

gdt_start:
gdt_null:
	.long 0x00
	.long 0x00
	
gdt_code:
	.word 0xffff
	.word 0x0
	.byte 0x0
	.byte 0b10011010
	.byte 0b11001111
	.byte 0x0
gdt_data:
	.word 0xffff
	.word 0x0
	.byte 0x0
	.byte 0b10010010
	.byte 0b11001111
	.byte 0x0
gdt_end:

gdt_descriptor:
	.word ((gdt_end - gdt_start) - 1)
	.long gdt_start		

BOOT_DRIVE:		.byte 0
BOOT_ERROR:		.asciz "Missing Bootable Partition!\n\r"
FSYS_ERROR:		.asciz "Failed To Load File System Boot Sector!\n\r"
A20_ERROR:		.asciz "Failed to Enable Line A20!\n\r"
OK_MSG:			.asciz "OK!\n\r"

.fill (510-(.-main)), 1, 0
BIOS_NUMBER: .word 0xAA55
.fill (612-(.-main)), 1, 0

#define SEC_PER_CLUS 0x7E0D
#define SEC_PER_HEAD 0x7E18
#define HEAD_PER_CYL 0x7E1A
#define SFT_PER_CLUS 0x7E62
#define MASK_PER_SFT 0x7E63
kernel_loader:
	
	mov $0x0000, %dx			# Set the Root Folder Cluster Index to
	mov $0x0002, %ax			# 0x 0000 0002 (%dx:%ax)
	mov $0x8200, %bx			# Set the load address to 0x8200
	call load_file				# Load the root folder into the memory
	jc 1f
	
	mov $KERNEL_NAME, %si		# Set the kernel file's name to %si
	mov $0x8200, %bx			# Set the start address of the folder to 0x8200
	call find_file				# Find the file's start cluster index
	jc 2f

	mov $0x8200, %bx			# Set the load address to 0x8200
	call load_file				# Load the kernel into the memory
	jc 3f
	
	ret

1:
	mov $ROOT_ERROR, %bx
	call print_str
	jmp .
2:
	mov $FIND_ERROR, %bx
	call print_str
	jmp .
3:
	mov $KERN_ERROR, %bx
	call print_str
	jmp .

.include "div_32.s"
.include "load_file.s"
.include "find_file.s"

FIND_ERROR:		.asciz "Kernel File Not Found!\n\r"
ROOT_ERROR:		.asciz "Failed To Load Root!\n\r"
KERN_ERROR:		.asciz "Failed To Load Kernel!\n\r"
KERNEL_NAME:	.ascii "KERNEL  SYS"

