.PHONY: all clean install uninstall run init analyze tidy valgrind release debug

PRJ := dotman
CC := gcc

# Colors and emojis
RESET := \033[0m
BOLD := \033[1m
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
MAGENTA := \033[35m
CYAN := \033[36m

# Common flags for both builds
COMMON_CFLAGS := -std=gnu17 -pedantic -Wall -Wextra -Werror \
                 -Wformat=2 -Wformat-security -Wnull-dereference -Warray-bounds=2 \
                 -Wimplicit-fallthrough=3 -Wstrict-prototypes -Wmissing-prototypes \
                 -Wstrict-overflow=2 -Wstringop-overflow=4 -Wshadow=local \
                 -Wconversion -Wsign-conversion

# Security flags (both builds)
SECURITY_FLAGS := -D_FORTIFY_SOURCE=2 -fstack-protector-strong \
                  -fstack-clash-protection -fcf-protection

# Debug-specific flags
DEBUG_CFLAGS := -g3 -Og -DDEBUG \
                -fsanitize=address,undefined \
                -fno-omit-frame-pointer

# Release-specific flags
RELEASE_CFLAGS := -O3 -DNDEBUG -flto -march=native -mtune=native

# Debug-specific linker flags
DEBUG_LDFLAGS := -fsanitize=address,undefined

# Release-specific linker flags
RELEASE_LDFLAGS := -fPIE -pie -Wl,-z,relro,-z,now -flto

# Directories
SRC_DIR := src
BLD_DIR := bld
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BLD_DIR)/%.o)
TRG := $(BLD_DIR)/$(PRJ)

# Include paths
INCLUDES := -I$(SRC_DIR) -Icvector

# Set flags based on build type
ifdef RELEASE
    CFLAGS := $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(RELEASE_CFLAGS) $(INCLUDES)
    LDFLAGS := $(RELEASE_LDFLAGS)
    BUILD_TYPE := $(BOLD)$(MAGENTA)RELEASE$(RESET)
else
    CFLAGS := $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES)
    LDFLAGS := $(DEBUG_LDFLAGS)
    BUILD_TYPE := $(BOLD)$(CYAN)DEBUG$(RESET)
endif

all: $(TRG)
	@echo "$(GREEN)✓ Build complete [$(BUILD_TYPE)$(GREEN)]$(RESET)"

$(TRG): $(OBJS) | $(BLD_DIR)
	@echo "$(YELLOW)🔗 Linking$(RESET) $(BOLD)$@$(RESET)"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c | $(BLD_DIR)
	@echo "$(BLUE)🔨 Compiling$(RESET) $(notdir $<)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	@mkdir -p $@

clean:
	@echo "$(RED)🧹 Cleaning build artifacts$(RESET)"
	@rm -rf $(BLD_DIR)
	@rm -rf .cache
	@rm -rf compile_commands.json
	@echo "$(GREEN)✓ Clean complete$(RESET)"

install: $(TRG)
	@echo "$(CYAN)📦 Installing$(RESET) $(BOLD)$(PRJ)$(RESET) to /usr/local/bin"
	@install -d /usr/local/bin
	@install -m 755 $(TRG) /usr/local/bin/
	@echo "$(GREEN)✓ Installation complete$(RESET)"

uninstall:
	@echo "$(RED)🗑️  Uninstalling$(RESET) $(BOLD)$(PRJ)$(RESET)"
	@rm -f /usr/local/bin/$(PRJ)
	@echo "$(GREEN)✓ Uninstall complete$(RESET)"

run: $(TRG)
	@echo "$(GREEN)🚀 Running$(RESET) $(BOLD)$(PRJ)$(RESET)"
	@echo "$(YELLOW)─────────────────────────────────────$(RESET)"
	@./$(TRG)

release:
	@echo "$(BOLD)$(MAGENTA)🎯 Building RELEASE version$(RESET)"
	@$(MAKE) --no-print-directory RELEASE=1

debug:
	@echo "$(BOLD)$(CYAN)🐛 Building DEBUG version$(RESET)"
	@$(MAKE) --no-print-directory

init:
	@echo "$(CYAN)📥 Initializing submodules$(RESET)"
	@git submodule update --init --recursive
	@echo "$(CYAN)🔍 Generating compile_commands.json$(RESET)"
	@bear -- make
	@echo "$(GREEN)✓ Initialization complete$(RESET)"

analyze:
	@echo "$(MAGENTA)🔬 Running static analysis$(RESET)"
	@$(CC) $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES) \
	      -fanalyzer -fsyntax-only $(SRCS)
	@echo "$(GREEN)✓ Analysis complete$(RESET)"

tidy:
	@echo "$(MAGENTA)🧼 Running clang-tidy$(RESET)"
	@clang-tidy $(SRCS) -- $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES)
	@echo "$(GREEN)✓ Tidy complete$(RESET)"

valgrind:
	@echo "$(BOLD)$(YELLOW)🔍 Building for Valgrind$(RESET)"
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory RELEASE=1 CFLAGS="$(COMMON_CFLAGS) $(SECURITY_FLAGS) -O3 -DNDEBUG -g3 $(INCLUDES)" LDFLAGS="-fPIE -pie -Wl,-z,relro,-z,now"
	@echo "$(YELLOW)🧪 Running Valgrind$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	         --verbose --log-file=valgrind-out.txt ./$(TRG)
	@echo "$(GREEN)✓ Valgrind complete, check valgrind-out.txt$(RESET)"
