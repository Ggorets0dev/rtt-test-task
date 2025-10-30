#include "unity.h"

void test_AES_encrypt_null_args(void);
void test_AES_encrypt_zero_length(void);
void test_AES_encrypt_invalid_key_size(void);

void test_AES_decrypt_null_args(void);
void test_AES_decrypt_zero_length(void);
void test_AES_decrypt_invalid_key_size(void);

void test_AES_encrypt_decrypt_correct(void);

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_AES_encrypt_null_args);
    RUN_TEST(test_AES_encrypt_zero_length);
    RUN_TEST(test_AES_encrypt_invalid_key_size);

    RUN_TEST(test_AES_decrypt_null_args);
    RUN_TEST(test_AES_decrypt_zero_length);
    RUN_TEST(test_AES_decrypt_invalid_key_size);

    RUN_TEST(test_AES_encrypt_decrypt_correct);

    return UNITY_END();
}

