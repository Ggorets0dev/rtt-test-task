# === Директории ===
BUILD_DIR      := build
OBJ_DIR        := $(BUILD_DIR)/obj
SRC_DIR        := src
INC_DIR        := inc
TEST_DIR       := tests
TP_UNITY_DIR   := third-party/unity

TARGET          := $(BUILD_DIR)/crypt_tool
TARGET_TEST_RUNNER := $(BUILD_DIR)/test_runner

# === Компилятор и флаги ===
CC       := gcc
CFLAGS_BASE := -Wall -Wextra -std=c17 -I$(INC_DIR) -I$(TP_UNITY_DIR) -MMD -MP
LDFLAGS  := -lssl -lcrypto

# === Режим сборки (по умолчанию release) ===
MODE ?= release
ifeq ($(MODE),debug)
  CFLAGS := $(CFLAGS_BASE) -g -O0 -fno-omit-frame-pointer
  BUILD_MSG := "=== Сборка в ОТЛАДОЧНОМ режиме ==="
else
  CFLAGS := $(CFLAGS_BASE) -O2 -DNDEBUG
  BUILD_MSG := "=== Сборка в РЕЛИЗНОМ режиме ==="
endif

# === Источники и объекты ===
SRCS      := $(wildcard $(SRC_DIR)/*.c)
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS      := $(OBJS:.o=.d)

# === Тестовые файлы ===
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.test.o,$(TEST_SRCS))
SRCS_TP   := $(wildcard $(TP_UNITY_DIR)/*.c)
OBJS_NO_MAIN := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

# === Цель по умолчанию ===
all: $(TARGET)

# === Основная сборка ===
$(TARGET): $(OBJS) | $(BUILD_DIR)
	@echo $(BUILD_MSG)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Сборка завершена: $@"

# === Компиляция .c → .o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# === Тестовый раннер ===
$(TARGET_TEST_RUNNER): $(OBJS_NO_MAIN) $(SRCS_TP) $(TEST_OBJS) | $(BUILD_DIR)
	@echo "=== Сборка тестового раннера ==="
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Сборка завершена: $@"

# Компиляция тестовых файлов
$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# === Запуск тестов ===
test: $(TARGET_TEST_RUNNER)
	@echo "=== Запуск тестов ==="
	@./$(TARGET_TEST_RUNNER)

# === Проверка через Valgrind ===
check: MODE=debug
check: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(TARGET) $(ARGS)

# === Создание директорий ===
$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

# === Очистка ===
clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BUILD_DIR)
	@echo "Полная очистка завершена"

re: fclean all

# === Зависимости ===
-include $(DEPS)

.PHONY: all clean fclean re test check

