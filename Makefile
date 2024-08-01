CC = /usr/bin/gcc
FLAGS = -Wall -Werror -g
TARGETS = src/*.c
OUTPUT = sysprak-client

all: 
	@$(CC) $(FLAGS) $(TARGETS) -o $(OUTPUT) > /dev/null && echo 'done'

clean:
	@rm -f $(OUTPUT) > /dev/null && echo 'done'
