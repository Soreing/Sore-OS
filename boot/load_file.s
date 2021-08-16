# Loads all clusters of a file into memory from a starting cluster index
#    Input
#        %ax: Cluster index Lower 16 bits
#        %dx: Cluster index Higher 16 bits 
#        %bx: Memory address where the file is loaded
#    RegOverwrites
#        %ax, %bx, %cx, %dx
load_file:
	
	push %ax					# Save the lower 16 bits of the cluster index for loading the FAT
	push %dx					# Save the higher 16 bits of the cluster index for loading the FAT
	
	sub $2,  %ax				# Subtract 2 from the lower 16 bits of the LBA
	sbb $0,  %dx				# Decrement the higher 16 bits of the LBA if a borrow occured
	
	xor %cx, %cx				# Clear %cx to be used for shifting the LBA
	mov 0x7E62, %cl				# Load the shift value to multiply by Sectors per Cluster
	
	shl %cl, %dx				# Shift multiply the higher 16 bits of the LBA
	rol %cl, %ax				# Rotate multiply the lower 16 bits of the LBA
	push %ax					# Save the lower 16 bits to be used for masking
	
	mov 0x7E63, %cl				# Load the mask to move bits from lower to higher 16 bits
	and %cx, %ax				# Pick out the lower n rotated bits 
	or  %ax, %dx				# Merge the selected bits to the higher 16 bits
	pop %ax						# Restore the original value of the lower 16 bits
	not %cx						# Invert the mask to keep the needed bits
	and %cx, %ax				# Remove the lower n rotated bits with the mask
	
	mov 0x7E5E, %cx				# Load the lower 16 bit LBA of the Cluster region and 
	add %cx, %ax				# add it to the lower 16 bit sector offset just calculated
	mov 0x7E60, %cx				# Load the higher 16 bit LBA of the Cluster region and 
	adc %cx, %dx				# add it to the higher 16 bit sector offset just calculated

	mov %ax, DAP_LBA_1			# Move the LBA lower 16 bits to DAP_LBA_1 for reading sectors
	mov %dx, DAP_LBA_2			# Move the LBA higher 16 bits to DAP_LBA_2 for reading sectors
	
	movw %bx, DAP_DST_LOW		# Set the destination address to %bx
	
	xor %cx, %cx				# Clear and move the value of Sectors per Cluster into %cx
	movb 0x7E0D, %cl			# 
	movw %cx, DAP_SECTORS		# Set the number of sectors to read to Sectors per Cluster

	call load_disk				# Call the function to load sectors from disk to memory
	jc load_cluster_error		# If the carry (error) flag is set, jump to load_cluster_error 
	
	pop %dx						# Restore the original higher 16 bits of the cluster index  
	pop %ax						# Restore the original lower 16 bits of the cluster index 
	mov $128, %cx				# Move 128 into %cx for division (128 clusters in each FAT sector)
	call div32					# Divide the cluster index by 128 to find the FAT sector to load
	shl $2, %dx					# Shift multiply the remainder (cluster offset) to convert to bytes
	push %dx					# Save the cluster offset on the FAT sector
	
	mov LOWER, %ax				# Move the lower 16 bits of the division result (FAT Sector) to %ax
	mov UPPER, %dx				# Move the higher 16 bits of the division result (FAT Sector) to %dx
	mov 0x7E5A, %cx				# Load the lower 16 bit LBA of the FAT region and
	add %cx, %ax				# add it to the lower 16 bit FAT Sector just calculated
	mov 0x7E5C, %cx				# Load the higher 16 bit LBA of the FAT region and
	adc %cx, %dx				# add it to the higher 16 bit FAT Sector just calculated
	pop %cx						# Restore the value of the cluster offset in %cx
	
	mov %ax, DAP_LBA_1			# Move the FAT LBA lower 16 bits to DAP_LBA_1 for reading sectors
	mov %dx, DAP_LBA_2			# Move the FAT LBA hihger 16 bits to DAP_LBA_2 for reading sectors
	
	movw $0x8000, DAP_DST_LOW	# Set the destination address to 0x 0000 8000
	movw $0x0001, DAP_SECTORS	# Set the number of sectors to read to 1
	
	call load_disk				# Call the function to load sectors from disk to memory
	jc load_fat_error			# If the carry (error) flag is set, jump to load_fat_error
	
	push %bx					# Save the destination address %bx for later use
	mov $0x8000, %bx			# Move 0x8000, the address of the FAT sector to %bx
	add %cx, %bx				# Add the cluster offset to %bx
	mov (%bx),  %ax				# Move the next cluster index lower 16 bits to %ax
	mov 2(%bx), %dx				# Move the next cluster index higher 16 bits to %dx
	pop %bx						# Restore the destination address to %bx
	
	cmp $0xFFFF, %ax			#  
	jne 1f						# If the next cluster index is 0x 0FFFF FFFF,
	cmp $0x0FFF, %dx			# The file ended and return from the function
	jne 1f						# Otherwise load the next cluster
	ret							#
1:
	push %ax					# Save 
	mov $0x0200, %ax		
	mov 0x7E62,  %cl
	shl %cl, %ax
	add %ax, %bx				# Add 0x0200 (512) to the destination address
	pop %ax
	jmp load_file				# and load the next cluster
	
load_cluster_error:
	pop %dx
	pop %ax
load_fat_error:
	ret
	