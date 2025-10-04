PRJ := dotman
CC := gcc
CFLAGS := -std=gnu17 -pedantic -Wall -Wextra -Werror -g3 \
          -Wformat=2 -Wformat-security -Wnull-dereference -Warray-bounds=2 \
          -Wimplicit-fallthrough=3 -Wstrict-prototypes -Wmissing-prototypes \
          -Wstrict-overflow=2 -Wstringop-overflow=4 -Wshadow=local \
          -Wconversion -Wsign-conversion \
          -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fstack-clash-protection \
          -fcf-protection -fPIE -pie -Wl,-z,relro,-z,now

SRC_DIR := src
BLD_DIR := bld

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BLD_DIR)/%.o)
TRG := $(BLD_DIR/$($PRJ))

CFLAGS += -I$(SRC_DIR) -Icvector

ifdef RELEASE
    CFLAGS += -O3 -DNDEBUG
else
    CFLAGS += -Og -DDEBUG -fsanitize=address,undefined -fanalyzer -fno-omit-frame-pointer
endif

all: $(TRG)

$(TRG): $(OBJS) | $(BLD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c | $(BLD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BLD_DIR)
	rm -rf .cache
	rm -rf compile_commands.json

install: $(TARGET)
	intstall  -d /usr/local/bin
	intstall  -m 755 $(TRG) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(PRJ)

run: $(TRG)
	./$(TRG)

init:
	bear -- make

analyze:
	$(CC) $(CFLAGS) -fanalyzer -fsyntax-only $(SRCS)

tidy:
	clang-tidy $(SRCS) -- $(CFLAGS)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	         --verbose --log-file=valgrind-out.txt ./$(TARGET)

.PHONY: all clean install uninstall run init analyze tidy valgrind
