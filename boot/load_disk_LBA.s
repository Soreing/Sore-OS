# Loads sectors from the hard disk booted from.
#    Inputs
#        %ch: Cylinder number
#        %dh: Head number
#        %cl: Sector number
#        %al: Number of sectors to load
#        %es:%bx: Destination Address
load_disk:
	push %bx				# Save the original value of %bx
	
	mov BOOT_DRIVE, %dl 	# Select the drive we booted from
	lea DAP_SIZE, %si		# Load the address of the DAP to %si
	mov $0x42, %ah			# Select BIOS's extended read sector function
	int $0x13				# Tell BIOS to read the sectors
	
	pop %bx					# Restore the original value of %bx
	ret						# if an error occured, CF is set


DAP_SIZE: 		.byte 0x10
DAP_RSVD:		.byte 0x00
DAP_SECTORS:	.word 0
DAP_DST_LOW:	.word 0
DAP_DST_HIG:	.word 0
DAP_LBA_1:		.word 0
DAP_LBA_2:		.word 0
DAP_LBA_3:		.word 0
DAP_LBA_4:		.word 0
