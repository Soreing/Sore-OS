# Sore-OS
![OS Image](https://cdn.discordapp.com/attachments/611334944995672083/879725555124150323/Sore-OS.png)

# Description
Sore-OS is a primitive x86 architecture 32-bit Operating System with a FAT32 File System and a basic kernel capable of keyboard input and VGA output. The OS has a custom boot loader written in AT&T syntax.  
  

This project was started, motivated and influenced by ["Writing a Simple Operating System — from Scratch"](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) by Nick Blundell, then expanded on with the help of [OSDev Wiki](https://wiki.osdev.org/Main_Page)  

# Basic Design 
The Operating System boots into 16-bit Real mode, then with the help of the BIOS, it identifies the partition on the MBR sector, then finds KERNEL.SYS in the root directory of the FAT32 file system before changing into 32-bit Protected mode by configuring the Global Descriptor Table (GDT). 
  
 
After changing into 32-bit Protected mode, the kernel's main function is executed to configure the Interrupt Descriptor Table (IDT) and run the kernel program.

# Compiling the Operating System
```
Built binaries of the OS and the media-utility have been added to the /bin folder of the project.
```
To compile the boot-loader and/or the kernel, you must have a gcc cross compiler that compiles to i386 architecture. You can follow the steps outlined in the OSDev Wiki [tutorial](https://wiki.osdev.org/GCC_Cross-Compiler).  
```
Basically you will need to download the source code for binutils and gcc, then compile it from source to a specific architecture (i386). On Windows, you can use Cygwin.
```  
After you set up the cross compiler and added the executables to the system path, you can use the `makefile` in the root directory of the project to compile the files to create the `boot-loader` and `KERNEL.SYS`

# Creating a Bootable USB
The project folders contain a C++ native Windows program to format a USB device and install the necessary data on it to boot the OS. you can find it in `/_media_utility`. Place the compiled `boot-loader` program in the same directory as the media utility executable, then run the program. **The media device must be 4GiB or larger.**  
```
The media utility will ask you to enter the Physical Drive# to format a drive, and if there is no safety signature, you have to enter a random keyword to proceed.
```
**While I am confident in the program, I take no responsibility if you format the wrong drive, by your or my mistake!**  

# Running the OS in VirtualBox
After creating the bootable USB, you will need to place KERNEL.SYS file in the root directory for the OS.  
To run the Operating System in VirtualBox, you need to create a virtual machine disk by linking to your media device, then run VirtualBox as an administrator. You cal follow any guide or [this](https://www.howtogeek.com/187721/how-to-boot-from-a-usb-drive-in-virtualbox/).  