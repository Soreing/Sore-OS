# Prints a byte to the screen in Hexadecimal
#   Inputs:
#     %bl: Byte to be printed
print_byte:
	push %ax
	
	mov $0x0e, %ah
	
	mov $'0', %al
	mov %bl, %al			# Move byte to %al
	shr $4, %al				# Shift it 4 bits to the right
	call to_hex				# Convert the value into a digit
	int $0x10				# Print the value
	
	mov $'0', %al
	mov %bl, %al			# Move byte to %al
	and $0x0F, %al 			# Mask out the upper nibble
	call to_hex				# Convert the value into a digit
	int $0x10				# Print the value
	
	pop %ax
	ret 

# Converts the value in %al to a hexadecimal digit
#   Inputs:
#     %al: Value from 0-15
to_hex:
	cmp $10, %al
	jl 1f
	add $'7', %al
	ret
1:
	add $'0', %al
	ret
