#Compiler setting
C= gcc				#Set compiler to gcc

#Compiler Option setting
CFLAGS= -g -Wall
					#-g option is information for gdb into the binary
					#-Wall option is warnings for all questionable code

#Object files setting
OBJS= hello.o world.o main.o 	#Object files to be compiled

#Target file setting
TARGET=app.out			#Name the executable file to be created 'app.out'

#Target file creation rule
$(TARGET): $(OBJS)		#Create a target file named 'app.out' from the object files listed in OBJS
	    $(CC) -o $@ $(OBJS)	#Use gcc to link object files to create executable files
	    			#$@ is the name of the current target, meaning the target name(app.out)
	     
#main.o file creation rule
main.o: hello.h world.h main.c	#Files list(hello.h, world.h, main.c) required to create main.o
#world.o file creation rule
hello.o: hello.h hello.c	#Files list(hello.h, hello.c) required to create hello.o
#world.o file creation rule
world.o: world.h world.c	#Files list(world.h, world.c) required to create world.o

#Clean rule
clean:				#Run when 'make clean' is called
	    rm -f *.o		#Delete all object files
	        rm -f $(TARGET)	#Delete generated target file(app.out)