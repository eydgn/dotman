.PHONY: all clean distclean install uninstall run init analyze tidy valgrind release debug help format compdb test

PRJ := dotman
CC := gcc

# Version info
GIT_COMMIT := $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
BUILD_DATE := $(shell date -u '+%Y-%m-%d_%H:%M:%S')

# Verbose output control (use V=1 for verbose)
ifeq ($(V),1)
    Q :=
else
    Q := @
endif

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
                 -Wconversion -Wsign-conversion -MMD -MP

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
DEPS := $(OBJS:.o=.d)
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

# Add version info
CFLAGS += -DGIT_COMMIT='"$(GIT_COMMIT)"' -DGIT_BRANCH='"$(GIT_BRANCH)"' -DBUILD_DATE='"$(BUILD_DATE)"'

# Include dependency files
-include $(DEPS)

.DEFAULT_GOAL := all

all: $(TRG)
	$(Q)echo -e "$(GREEN)✓ Build complete [$(BUILD_TYPE)$(GREEN)]$(RESET)"

help:
	$(Q)echo -e "$(BOLD)Available targets:$(RESET)"
	$(Q)echo -e "  $(CYAN)all$(RESET)          - Build the project (default: debug)"
	$(Q)echo -e "  $(CYAN)debug$(RESET)        - Build with debug flags and sanitizers"
	$(Q)echo -e "  $(CYAN)release$(RESET)      - Build optimized release version"
	$(Q)echo -e "  $(CYAN)clean$(RESET)        - Remove build artifacts"
	$(Q)echo -e "  $(CYAN)distclean$(RESET)    - Remove all generated files"
	$(Q)echo -e "  $(CYAN)install$(RESET)      - Install to /usr/local/bin"
	$(Q)echo -e "  $(CYAN)uninstall$(RESET)    - Remove from /usr/local/bin"
	$(Q)echo -e "  $(CYAN)run$(RESET)          - Build and run the program"
	$(Q)echo -e "  $(CYAN)init$(RESET)         - Initialize git submodules"
	$(Q)echo -e "  $(CYAN)compdb$(RESET)       - Generate compile_commands.json"
	$(Q)echo -e "  $(CYAN)format$(RESET)       - Format source code with clang-format"
	$(Q)echo -e "  $(CYAN)analyze$(RESET)      - Run GCC static analyzer"
	$(Q)echo -e "  $(CYAN)tidy$(RESET)         - Run clang-tidy linter"
	$(Q)echo -e "  $(CYAN)valgrind$(RESET)     - Run with Valgrind memory checker"
	$(Q)echo -e "  $(CYAN)test$(RESET)         - Run tests (if available)"
	$(Q)echo -e "  $(CYAN)help$(RESET)         - Show this help message"
	$(Q)echo -e ""
	$(Q)echo -e "$(BOLD)Options:$(RESET)"
	$(Q)echo -e "  V=1              - Verbose output"
	$(Q)echo -e "  -j$(BOLD)N$(RESET)            - Parallel build with N jobs"

$(TRG): $(OBJS) | $(BLD_DIR)
	$(Q)echo -e "$(YELLOW)🔗 Linking$(RESET) $(BOLD)$@$(RESET)"
	$(Q)$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c | $(BLD_DIR)
	$(Q)echo -e "$(BLUE)🔨 Compiling$(RESET) $(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	$(Q)mkdir -p $@

clean:
	$(Q)echo -e "$(RED)🧹 Cleaning build artifacts$(RESET)"
	$(Q)rm -rf $(BLD_DIR)
	$(Q)echo -e "$(GREEN)✓ Clean complete$(RESET)"

distclean: clean
	$(Q)echo -e "$(RED)🗑️  Removing all generated files$(RESET)"
	$(Q)rm -rf .cache compile_commands.json valgrind-out.txt
	$(Q)echo -e "$(GREEN)✓ Distclean complete$(RESET)"

install: release
	$(Q)echo -e "$(CYAN)📦 Installing$(RESET) $(BOLD)$(PRJ)$(RESET) to /usr/local/bin"
	$(Q)install -d /usr/local/bin
	$(Q)install -m 755 $(TRG) /usr/local/bin/
	$(Q)echo -e "$(GREEN)✓ Installation complete$(RESET)"

uninstall:
	$(Q)echo -e "$(RED)🗑️  Uninstalling$(RESET) $(BOLD)$(PRJ)$(RESET)"
	$(Q)rm -f /usr/local/bin/$(PRJ)
	$(Q)echo -e "$(GREEN)✓ Uninstall complete$(RESET)"

run: $(TRG)
	$(Q)echo -e "$(GREEN)🚀 Running$(RESET) $(BOLD)$(PRJ)$(RESET)"
	$(Q)echo -e "$(YELLOW)─────────────────────────────────────$(RESET)"
	$(Q)./$(TRG)

release:
	$(Q)echo -e "$(BOLD)$(MAGENTA)🎯 Building RELEASE version$(RESET)"
	$(Q)$(MAKE) --no-print-directory RELEASE=1

debug:
	$(Q)echo -e "$(BOLD)$(CYAN)🐛 Building DEBUG version$(RESET)"
	$(Q)$(MAKE) --no-print-directory

init:
	$(Q)echo -e "$(CYAN)📥 Initializing submodules$(RESET)"
	$(Q)git submodule update --init --recursive
	$(Q)echo -e "$(GREEN)✓ Initialization complete$(RESET)"

compdb:
	$(Q)echo -e "$(CYAN)🔍 Generating compile_commands.json$(RESET)"
	$(Q)bear -- $(MAKE) clean all
	$(Q)echo -e "$(GREEN)✓ Compilation database generated$(RESET)"

format:
	$(Q)echo -e "$(MAGENTA)🎨 Formatting source code$(RESET)"
	$(Q)clang-format -i $(SRCS) $(SRC_DIR)/*.h 2>/dev/null || true
	$(Q)echo -e "$(GREEN)✓ Format complete$(RESET)"

analyze:
	$(Q)echo -e "$(MAGENTA)🔬 Running static analysis$(RESET)"
	$(Q)$(CC) $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES) \
	      -fanalyzer -fsyntax-only $(SRCS)
	$(Q)echo -e "$(GREEN)✓ Analysis complete$(RESET)"

tidy:
	$(Q)echo -e "$(MAGENTA)🧼 Running clang-tidy$(RESET)"
	$(Q)clang-tidy $(SRCS) -- $(COMMON_CFLAGS) $(SECURITY_FLAGS) $(DEBUG_CFLAGS) $(INCLUDES)
	$(Q)echo -e "$(GREEN)✓ Tidy complete$(RESET)"

valgrind:
	$(Q)echo -e "$(BOLD)$(YELLOW)🔍 Building for Valgrind$(RESET)"
	$(Q)$(MAKE) --no-print-directory clean
	$(Q)$(MAKE) --no-print-directory RELEASE=1 \
	    CFLAGS="$(COMMON_CFLAGS) $(SECURITY_FLAGS) -O1 -g3 $(INCLUDES)" \
	    LDFLAGS="-fPIE -pie -Wl,-z,relro,-z,now"
	$(Q)echo -e "$(YELLOW)🧪 Running Valgrind$(RESET)"
	$(Q)valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	         --verbose --log-file=valgrind-out.txt ./$(TRG)
	$(Q)echo -e "$(GREEN)✓ Valgrind complete, check valgrind-out.txt$(RESET)"

test:
	$(Q)echo -e "$(CYAN)🧪 Running tests$(RESET)"
	$(Q)echo -e "$(YELLOW)⚠ No tests configured yet$(RESET)"
