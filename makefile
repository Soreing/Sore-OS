# Automatically generate list of source files
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
C_OBJECTS = $(C_SOURCES:.c=.o)

OBJ = @{C_SOURCES:.c=.o}

# Default build target
all: KERNEL.SYS boot-loader clean

# build the kernel binary
KERNEL.SYS : kernel/kernel-entry.o kernel/irq.o ${C_OBJECTS}
	i386-elf-ld -Ttext 0x8200 -e main $^ -o $@  --oformat binary

# Generic rule for building C source files into object files
%.o : %.c ${HEADERS}
	i386-elf-gcc -c $< -o $@ -I ./

# Assemble kernel-entry
%.o : %.s
	i386-elf-as $< -o $@

# Link the Boot Loader
boot-loader : boot/boot-loader.o
	i386-elf-ld -Ttext 0x7c00 -e main $^ -o $@  --oformat binary

# Assemble the Boot Loader
boot/boot-loader.o : boot/boot.s 
	i386-elf-as $< -I "boot/" -o $@

# Clear all the intermediate files
clean:
	rm -fr *.bin *.dis *.o
	rm -fr kernel/*.o boot/*.o drivers/*.o
	