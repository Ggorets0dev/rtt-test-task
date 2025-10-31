#include <unistd.h>
#include <stdio_ext.h>

#include "unity.h"
#include "logger.h"

// ==================
// Функции тестов (взяты из других модулей)
// ==================
/**
 * @brief Тест функции AES_encrypt: переданы нулевые аргументы (NULL-указатели).
 */
void test_AES_encrypt_null_args(void);

/**
 * @brief Тест функции AES_encrypt: длина входных данных равна нулю.
 */
void test_AES_encrypt_zero_length(void);

/**
 * @brief Тест функции AES_encrypt: использован недопустимый размер ключа.
 */
void test_AES_encrypt_invalid_key_size(void);

/**
 * @brief Тест функции AES_decrypt: переданы нулевые аргументы (NULL-указатели).
 */
void test_AES_decrypt_null_args(void);

/**
 * @brief Тест функции AES_decrypt: длина входных данных равна нулю.
 */
void test_AES_decrypt_zero_length(void);

/**
 * @brief Тест функции AES_decrypt: использован недопустимый размер ключа.
 */
void test_AES_decrypt_invalid_key_size(void);

/**
 * @brief Тест корректности шифрования и расшифрования AES.
 *
 * Проверяет, что после шифрования и последующего расшифрования
 * исходные данные совпадают с результатом.
 */
void test_AES_encrypt_decrypt_correct(void);

/**
 * @brief Тест validate_input: передан NULL-указатель.
 */
void test_validate_input_NULL(void);

/**
 * @brief Тест validate_input: передана пустая строка.
 */
void test_validate_input_empty(void);

/**
 * @brief Тест validate_input: строка содержит пробел.
 */
void test_validate_input_space(void);

/**
 * @brief Тест validate_input: корректная строка.
 */
void test_validate_input_valid(void);

/**
 * @brief Тест get_login: корректный ввод логина.
 */
void test_get_login_valid(void);

/**
 * @brief Тест get_password: корректный ввод пароля.
 */
void test_get_password_valid(void);

/**
 * @brief Тест get_crypto_key: корректное получение криптографического ключа.
 */
void test_get_crypto_key_valid(void);

/**
 * @brief Тест get_crypto_key: ввод недопустимого шестнадцатеричного ключа.
 */
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

    const char* null_path = "/dev/null";

    fflush(stdout);
    saved_stdout = stdout;
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

	const char* tty_path = "/dev/tty";

	if (freopen(tty_path, "w", stdout) == NULL) {
		// Не удалось выполнить перенаправление
		return;
	}

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

