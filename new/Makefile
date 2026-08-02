CC_LINUX = gcc
CC_WIN = x86_64-w64-mingw32-gcc

CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS_WIN = -static -static-libgcc -static-libstdc++

SRC_DIR = src
OBJ = .obj
BUILD_DIR = build

GET_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ)/$(PLATFORM)/%.o, $(wildcard $(SRC_DIR)/*.c))

ifeq ($(OS),Windows_NT)
    PLATFORM = win
    EXT = .exe
    TARGET_CC = $(CC_WIN)
else
    PLATFORM = linux
    EXT =
    TARGET_CC = $(CC_LINUX)
endif

TARGET = $(BUILD_DIR)/qcodeplus$(EXT)

all: linux win

linux:
	@$(MAKE) PLATFORM=linux EXT= TARGET_CC=$(CC_LINUX) build_platform

win:
	@$(MAKE) PLATFORM=win EXT=.exe TARGET_CC=$(CC_WIN) LDFLAGS="$(LDFLAGS_WIN)" build_platform

build_platform:
	@$(MAKE) $(TARGET) OBJS="$(GET_OBJS)"

$(OBJ)/$(PLATFORM)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(TARGET_CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(TARGET_CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

run: all
	@clear
	@echo "--- Running on $(PLATFORM) ---"
	@./$(TARGET)

clean:
	@rm -rf $(OBJ) $(BUILD_DIR)

.PHONY: all linux win clean build_platform run
