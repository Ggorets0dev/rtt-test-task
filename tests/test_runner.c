#include "unity.h"
#include "logger.h"

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

void setUp(void) {

}

void tearDown(void) {

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

