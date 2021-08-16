# Finds a file in a folder by comparing each character in the
# file's short name to a template string, and if not a match,
# move to the next file entry (32 byte) till the end of the folder
#    Inputs
#        %si: Address to file name template
#        %bx: Address to the first file in a folder
#    Outputs
#        %ax: Cluster ID (LOW)
#        %dx: Cluster ID (HIGH) 
find_file:
	push %cx					# Save the value of %cx which will be the counter 
	push %bx					# Save the value of %bx which will be iterated
1:
	push %bx					# Save %bx to be restored in each file iteration
	push %si					# Save %si to be restored in each file iteration
	
	mov (%bx), %al				# Check the 1st byte of the file entry
	or %al, %al					# If it is zero, it's the end of the folder
	jz 3f						# and jump out of the loop with an error

	xor %cx, %cx				# Reset the counter %cx for each file 
	2:
		lodsb					# Load 1 byte of the source (template string) to %al
		mov (%bx), %dl			# Load 1 byte of the file name to %dl
		
		xor %al, %dl			# compare the byte of the template and the filename
		jnz 2f					# if they are not equal, jump out of the name test loop
		
		cmp $10, %cx			# if the counter reached 10, all characters matched
		je 1f					# and should jump out of the file iteration loop
		
		inc %cx					# Increment the counter
		inc %bx					# Increment the file name address
		
		jmp 2b					# Do next iteration of the name test loop
	2:
	pop %si						# Restore the value of the file name template pointer
	pop %bx						# Restore the value of the file pointer
	add $32, %bx				# Move the file pointer to the next file (add 32)
	jmp 1b						# Do the next iteration of the file loop
3:
	stc							# On error set the carry flag
1:
	pop %si						# Restore the value of the file name template pointer
	pop %bx						# Restore the value of the file pointer
	mov 0x1A(%bx), %ax			# Move the lower 16 bits of the cluster LBA at offset 1Ah to %ax
	mov 0x14(%bx), %dx			# Move the higher 16 bits of the cluster LBA at offset 14h to %dx
	
	pop %bx						# Restore the original value of %bx 
	pop %cx						# Restore the original value of %cx 
	
	ret
	