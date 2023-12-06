# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c++11 -g

# Libraries
LIBS = -lcurl -lgumbo

# Target executable
TARGET = crawler

# Source files
SRCS = main.cpp crawler.cpp crawlDepth.cpp parallelCrawler.cpp
# crawlerBreadth.cpp crawlerHelper.cpp
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	$(RM) $(TARGET)
