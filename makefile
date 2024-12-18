### ------ Setting up the compiling of C files ------ ###
# Specify the compiler for C files
CC = gcc

# Compiling flags
CFLAGS = -Wall

# Libraries
LIBS = -lm

# Output
OUT = -o

### ------ Setting up the compiling of Python files ------ ###
# Specify the Python interpreter - use your own. 
PYTHON = /bin/python3 /home/dorr/Desktop/MyFiles/BA3/Project/

# Specify file names and their relative paths
C_FILE = Code/project.c

# Specify the name of your Python file
PYTHON_FILE = Code/project.py

### ------ Default target -> order in which you want the files to be run this is what will actually happen in your terminal ------ ###
all: compile_c run_c_executable run_python clean

### --- Line command (target) to compile the C file --- ###
compile_c: $(C_FILE)
	$(CC) $(CFLAGS) $(C_FILE) $(LIBS) $(OUT) $(basename $(C_FILE))


run_c_executable: compile_c
	./$(basename $(C_FILE))

### --- Target to run the Python files --- ###
run_python: $(basename $(C_FILE))
	$(PYTHON) $(PYTHON_FILE)
	
# Clean target to remove compiled files
clean:
	rm -f $(basename $(C_FILE))
