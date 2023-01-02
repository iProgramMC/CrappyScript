
C_FILES = \
	allocator.c  \
	builtin.c    \
	main.c       \
	nanoshell.c  \
	parser.c     \
	runner.c     \
	shell.c      \
	tokeniser.c

O_FILES = $(patsubst %.c,build/%.o,$(C_FILES))

TARGET = nsshell

CC = gcc

default: tool

clean:
	rm -rf build
	rm -f $(TARGET)

tool: $(TARGET)
	@echo "Done!"

$(TARGET): $(O_FILES)
	$(CC) $(O_FILES) -o $(TARGET)

build/%.o: %.c
	@mkdir -p build
	$(CC) -O2 -c $^ -o $@

