# === Директории ===
BUILD_DIR = build
TARGET = $(BUILD_DIR)/crypt_tool
TARGET_TEST_RUNNER = $(BUILD_DIR)/test_runner
SRC_DIR = src
INC_DIR = inc
TP_UNITY_DIR = third-party/unity
OBJ_DIR = $(BUILD_DIR)/obj

# === Компилятор ===
CC = gcc
CFLAGS_BASE = -Wall -Wextra -I$(INC_DIR) -I$(TP_UNITY_DIR) -std=c17
LDFLAGS = -lssl -lcrypto

# === Режим сборки (по умолчанию release) ===
MODE ?= release

# === Список исходников и объектных файлов ===
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS_TP = $(wildcard $(TP_UNITY_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# === Режимы сборки ===
release: CFLAGS = $(CFLAGS_BASE) -O2 -DNDEBUG
release:
	@echo "=== Сборка в РЕЛИЗНОМ режиме ==="
	$(MAKE) $(TARGET) CFLAGS="$(CFLAGS)"

debug: CFLAGS = $(CFLAGS_BASE) -g -O0 -fno-omit-frame-pointer
debug:
	@echo "=== Сборка в ОТЛАДОЧНОМ режиме ==="
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

# === Тесты ===
TEST_DIR = tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.test.o, $(TEST_SRCS))

# Убираем src/main.c из сборки тестов
OBJS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

test: CFLAGS = $(CFLAGS_BASE) -g -O0
test: $(TARGET_TEST_RUNNER)
	@echo "=== Запуск тестов ==="
	@./$(TARGET_TEST_RUNNER)

$(TARGET_TEST_RUNNER): $(OBJS_NO_MAIN) $(SRCS_TP) $(TEST_OBJS) | $(BUILD_DIR)
	@echo "=== Сборка тестового раннера ==="
	$(CC) $(CFLAGS) $(OBJS_NO_MAIN) $(SRCS_TP) $(TEST_OBJS) -o $@ $(LDFLAGS)

# Компиляция тестовых файлов (в отдельные object-файлы)
$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


re: fclean all

# === Проверка через Valgrind ===
check: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose \
		./$(TARGET) $(ARGS)

.PHONY: all debug release clean fclean re check test
