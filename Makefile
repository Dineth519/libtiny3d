# =====================================================
# Compiler and flags
# =====================================================
CC = gcc
CFLAGS = -Wall -Iinclude
AR = ar
ARFLAGS = rcs
LDFLAGS = -lm

# =====================================================
# Folders
# =====================================================
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = build/demo
VISUAL_DIR = tests/visual_tests

# =====================================================
# Source files
# =====================================================
LIB_SOURCES = $(SRC_DIR)/canvas.c \
              $(SRC_DIR)/math3d.c \
              $(SRC_DIR)/renderer.c \
              $(SRC_DIR)/animation.c \
              $(SRC_DIR)/lighting.c

LIB = $(BUILD_DIR)/libtiny3d.a

# =====================================================
# Executables
# =====================================================
CLOCK_TARGET = $(BIN_DIR)/clock_face
SOCCER_TARGET = $(BIN_DIR)/soccer_ball

# =====================================================
# Build all
# =====================================================
all: $(LIB) $(CLOCK_TARGET) $(SOCCER_TARGET) | $(VISUAL_DIR)

# =====================================================
# Ensure directories exist
# =====================================================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(VISUAL_DIR):
	mkdir -p $(VISUAL_DIR)

# =====================================================
# Build static library without leaving .o files
# =====================================================
$(LIB): | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(LIB_SOURCES)
	$(AR) $(ARFLAGS) $@ *.o
	rm -f *.o

# =====================================================
# Build executables
# =====================================================
$(CLOCK_TARGET): demo/main.c $(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -ltiny3d $(LDFLAGS)

$(SOCCER_TARGET): demo/main1.c $(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -ltiny3d $(LDFLAGS)

# =====================================================
# Run examples
# =====================================================
run: all
	@echo "Running clock_face..."
	@$(CLOCK_TARGET) $(VISUAL_DIR)
	@echo "Running soccer_ball..."
	@$(SOCCER_TARGET) $(VISUAL_DIR)

# =====================================================
# Clean
# =====================================================
clean:
	rm -f $(BUILD_DIR)/libtiny3d.a $(CLOCK_TARGET) $(SOCCER_TARGET)