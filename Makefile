CC = cc
CXX = c++
CFLAGS =   -I. -Wall -Wextra -Werror
CXXFLAGS = -I. -Wall -Wextra -Werror -std=c++98

TARGET = webserv

BIN_DIR = bin

SRCS = ./src/main.cpp

OBJS = bin/main.o

vpath %.c  
vpath %.cpp ./src 

.DEFAULT_GOAL := $(TARGET)


all: $(TARGET)

debug: CXXFLAGS += -g
debug: fclean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/%.o: %.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BIN_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all
$(BIN_DIR): 
	mkdir -p $(BIN_DIR)
.PHONY: all clean fclean re
.SECONDARY:
