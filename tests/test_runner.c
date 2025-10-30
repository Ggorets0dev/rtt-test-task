#include <unistd.h>
#include <fcntl.h>
#include <stdio_ext.h>

#include "unity.h"
#include "logger.h"

// ==================
// Функции тестов (взяты из других модулей)
// ==================
void test_AES_encrypt_null_args(void);
void test_AES_encrypt_zero_length(void);
void test_AES_encrypt_invalid_key_size(void);

void test_AES_decrypt_null_args(void);
void test_AES_decrypt_zero_length(void);
void test_AES_decrypt_invalid_key_size(void);

void test_AES_encrypt_decrypt_correct(void);

void test_validate_input_NULL(void);
void test_validate_input_empty(void);
void test_validate_input_space(void);
void test_validate_input_valid(void);

void test_get_login_valid(void);
void test_get_password_valid(void);
void test_get_crypto_key_valid(void);
void test_get_crypto_key_invalid_hex(void);
// ==================

// ==================
// Статические функции и переменные
// ==================
/**
 * @brief Сохраненный дескриптор вывода
 */
static FILE* saved_stdout = NULL;

/**
 * @brief Отключить вывод в STDOUT
 */
static void disable_stdout(void);

/**
 * @brief Включить вывод в STDOUT
 */
static void enable_stdout(void);
// ==================

// Отключает вывод printf
static void disable_stdout(void) {
    if (saved_stdout != NULL) {
        // Уже отключено
        return;
    }

    const char *null_path = "/dev/null";

    fflush(stdout);
    saved_stdout = stdout;                     // сохраняем указатель
    FILE *null_out = freopen(null_path, "w", stdout);
    if (null_out == NULL) {
        // если не удалось перенаправить, возвращаем всё обратно
        stdout = saved_stdout;
        saved_stdout = NULL;
    }
}

// Включает обратно вывод printf
static void enable_stdout(void) {
    if (saved_stdout == NULL) {
        // Отключение не происходило
        return;
    }

    freopen("/dev/tty", "w", stdout);

    saved_stdout = NULL;
}


void setUp(void) {
    disable_stdout();
}

void tearDown(void) {
    enable_stdout();
}

void log_message(log_type_e type, const char *fmt, ...) {
    (void)type;
    (void)fmt;
    // Подавляем вывод логов с помощью переопределения функции в тестовом проекте
}

int main(void) {
    UNITY_BEGIN();

    // ============
    // Тесты функций шифрования
    // ============
    RUN_TEST(test_AES_encrypt_null_args);
    RUN_TEST(test_AES_encrypt_zero_length);
    RUN_TEST(test_AES_encrypt_invalid_key_size);
    // ============

    // ============
    // Тесты функций дешифрования
    // ============
    RUN_TEST(test_AES_decrypt_null_args);
    RUN_TEST(test_AES_decrypt_zero_length);
    RUN_TEST(test_AES_decrypt_invalid_key_size);
    RUN_TEST(test_AES_encrypt_decrypt_correct);
    // ============

    // ============
    // Тесты валидации входных данных
    // ============
    RUN_TEST(test_validate_input_NULL);
    RUN_TEST(test_validate_input_empty);
    RUN_TEST(test_validate_input_space);
    RUN_TEST(test_validate_input_valid);
    // ============

    // ============
    // Тесты получения входных данных
    // ============
    RUN_TEST(test_get_login_valid);
    RUN_TEST(test_get_password_valid);
    RUN_TEST(test_get_crypto_key_valid);
    RUN_TEST(test_get_crypto_key_invalid_hex);
    // ============

    return UNITY_END();
}

