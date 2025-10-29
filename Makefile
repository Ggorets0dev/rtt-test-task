# === Директории ===
BUILD_DIR = build
TARGET = $(BUILD_DIR)/crypt_tool
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = $(BUILD_DIR)/obj

# === Компилятор ===
CC = gcc
CFLAGS_BASE = -Wall -Wextra -I$(INC_DIR) -std=c17
LDFLAGS = -lssl -lcrypto

# === Режим сборки (по умолчанию release) ===
MODE ?= release

# === Список исходников и объектных файлов ===
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# === Режимы сборки ===
debug: CFLAGS = $(CFLAGS_BASE) -g -O0 -fno-omit-frame-pointer
debug:
	@echo "=== Сборка в ОТЛАДОЧНОМ режиме ==="
	$(MAKE) $(TARGET) CFLAGS="$(CFLAGS)"

release: CFLAGS = $(CFLAGS_BASE) -O2 -DNDEBUG
release:
	@echo "=== Сборка в РЕЛИЗНОМ режиме ==="
	$(MAKE) $(TARGET) CFLAGS="$(CFLAGS)"

# === Цель по умолчанию ===
all: release

# === Линковка ===
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === Компиляция ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# === Создание директорий ===
$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

# === Очистка ===
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

fclean: clean
	@echo "Полная очистка завершена"

re: fclean all

# === Проверка через Valgrind ===
check: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose \
		./$(TARGET) $(ARGS)

.PHONY: all debug release clean fclean re check
