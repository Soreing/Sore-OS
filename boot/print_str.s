# Prints a string to the screen
#    Inputs
#        %bx: Address of the string
print_str:
	mov $0x0e, %ah
	1: 
		mov (%bx), %al
		
		cmp $0, %al
		je 1f
		
		int $0x10
		add $1, %bx
		jmp 1b
	1:
	ret
