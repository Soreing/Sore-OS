# Tests if the A20 line is enabled
# Sets CF if the test fails
A20Test:
	pusha					# Save all registers
	
	xor %bx, %bx			# 
	mov %bx, %es			# Set %es:%bx to 0x 0000 0500
	mov $0x0500, %bx		#  
	
	movb $1, %es:(%bx)		# Put the value 1 at %es:%bx
	
	mov $0xFFFF, %bx		#
	mov %bx, %es			# Set %es:%bx to 0x FFFF 0510
	mov $0x0510, %bx		# (same as 0x 0000 0500 if A20 is disabled)
	
	movb %es:(%bx), %al		# Read the value at %es:%bx into %al
	
	xor $1, %al				# Check if %al is 1
	jnz 1f					# If %al is not 1, A20 is enabled
	stc						# Otherwise set the carry flag
1:
	popa					# Restore the value of all registers
	ret
	