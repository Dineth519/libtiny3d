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
OBJ_DIR = build/obj
BUILD_DIR = build
BIN_DIR = build/demo

# =====================================================
# Library
# =====================================================
LIB_OBJS = $(OBJ_DIR)/canvas.o \
           $(OBJ_DIR)/math3d.o \
           $(OBJ_DIR)/renderer.o \
           $(OBJ_DIR)/animation.o \
           $(OBJ_DIR)/lighting.o

LIB = $(BUILD_DIR)/libtiny3d.a

# =====================================================
# Executables
# =====================================================
CLOCK_TARGET = $(BIN_DIR)/clock_face
SOCCER_TARGET = $(BIN_DIR)/soccer_ball

# =====================================================
# Build rules
# =====================================================
all: $(LIB) $(CLOCK_TARGET) $(SOCCER_TARGET)

# Ensure directories exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile .c to .o inside build/obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library in build/, then clean obj/
$(LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^
	rm -rf $(OBJ_DIR)

# Link clock_face into build/demo/
$(CLOCK_TARGET): demo/main.c $(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -ltiny3d $(LDFLAGS)

# Link soccer_ball into build/demo/
$(SOCCER_TARGET): demo/main1.c $(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -ltiny3d $(LDFLAGS)

# Clean everything except build folders
clean:
	rm -f $(BUILD_DIR)/libtiny3d.a $(CLOCK_TARGET) $(SOCCER_TARGET)

