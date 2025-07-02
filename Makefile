# =====================================================
# Compiler and flags
# =====================================================
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lm

# =====================================================
# Folders
# =====================================================
SRC_DIR = src
DEMO_DIR = demo

# =====================================================
# Source files
# =====================================================
# Change 'main_file' to the main program you want to compile
MAIN_FILE = combined_main.c

SRCS = $(SRC_DIR)/canvas.c \
       $(SRC_DIR)/math3d.c \
       $(SRC_DIR)/renderer.c \
       $(DEMO_DIR)/$(MAIN_FILE)

# =====================================================
# Output binary
# =====================================================
TARGET = demo_combined

# =====================================================
# Build rules
# =====================================================
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
