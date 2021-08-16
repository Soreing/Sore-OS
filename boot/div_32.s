# Divides a 32 bit number by a 16 bit number
#    Inputs
#        %ax: Dividend Lower 16 bits
#        %dx: Dividend Higher 16 bits
#        %cx: Divisor
#    Output
#        %dx: 16 bit remainder
#        LOWER: Quotient Lower 16 bits
#        UPPER: Quotient higher 16 bits
#    RegOverwrites
#        %ax, %dx
#    Errors:
#        No check for division by 0 (currently can't occur)
div32:
	push %bx					# Save the original value of %bx
	push %dx					# Save the higher 16 bits of the dividend
	
	xor %dx, %dx				# Clear the higher 16 bits of the dividend
	div %cx						# Divide the lower 16 bits of the dividend by the divisor
	mov %dx, %bx				# Move the remainder to %bx to be used later
	mov %ax, LOWER				# Move the quotient to LOWER
	
	xor %dx, %dx				# Clear the higher 16 bits of the dividend
	pop %ax						# Restore the higher 16 bits of the dividend in %ax
	div %cx						# Divide the higher 16 bits of the dividend by the divisor
	mov %ax, UPPER 				# Move the quotient to UPPER
	
	xor %ax, %ax				# Clear the lower 16 bits of the dividend
	div %cx						# Divide the last remainder treated as higher 16 bits by the divisor
	
	add %bx, %dx				# Add the remainders from the higher and lower 16 bits division
	cmp %cx, %dx				# If the remainder is larger than the divisor
	jl 1f						
	inc %ax						# Increment the remaining quotient and
	sub %cx, %dx				# Subtract the divisor from the remainder
1:
	add %ax, LOWER				# Add remaining quotient to LOWER
	jnc 2f
	incw UPPER					# Increment UPPER if a carry occured 
2:	
	pop %bx						# Restore the original value of %bx
	ret
	
LOWER: 		.word 0
UPPER: 		.word 0

