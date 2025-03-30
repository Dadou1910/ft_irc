# Compiler and flags
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

# Source and object files
SOURCES = $(wildcard *.cpp) $(wildcard cmds/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
NAME = ircserv

# Default target
all: $(NAME)

# Link object files into executable
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

# Pattern rule for compiling .cpp files to .o files
%.o: %.cpp Server.hpp irc.hpp Client.hpp
	$(CC) $(CFLAGS) -c $< -o $@

# Run the executable after building
ex: all
	./$(NAME) 6667 o

# Clean object files
clean:
	rm -f $(OBJECTS)

# Clean everything (including executable)
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

.PHONY: all ex clean fclean re