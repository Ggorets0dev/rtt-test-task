#include  <string.h>

#include "unity.h"

#include "crypto.h" // Тестируемая библиотека

// Функции для настройки и очистки
void setUp(void) {
    // вызывается перед каждым тестом
}

void tearDown(void) {
    // вызывается после каждого теста
}

void test_AES_encrypt_decrypt_correct(void) {
    // === Arrange ===
    unsigned char key[CRYPTO_KEY_128 / 8] = {0x00};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0x00};
    unsigned char plaintext[] = "TestMessage";
    unsigned char ciphertext[64];
    unsigned char decrypted[64];
    status_e status;

    int plaintext_len = strlen((char *)plaintext);

    status  = generate_crypto_key(key, CRYPTO_KEY_128);
    TEST_ASSERT_EQUAL_MESSAGE(STATUS_OK, status, "Encryption key could not be generated");

    status  = generate_AES_CBC_vector(iv);
    TEST_ASSERT_EQUAL_MESSAGE(STATUS_OK, status, "Encryption IV could not be generated");

    // === Act ===
    int cipher_len = AES_encrypt(plaintext,
                                 plaintext_len,
                                 key,
                                 CRYPTO_KEY_128,
                                 iv,
                                 ciphertext);

    // === Assert basic checks ===
    TEST_ASSERT_MESSAGE(cipher_len > 0, "Encryption failed: returned -1");
    TEST_ASSERT_EQUAL_MESSAGE(0, cipher_len % AES_CBC_BLOCK_SIZE, "Cipher length not multiple of AES block size");

    // === Decrypt and compare ===
    int decrypted_len = AES_decrypt(ciphertext,
                                    cipher_len,
                                    key,
                                    CRYPTO_KEY_128,
                                    iv,
                                    decrypted);

    decrypted[decrypted_len] = '\0';
    TEST_ASSERT_EQUAL_STRING((char *)plaintext, (char *)decrypted);
}

void test_AES_encrypt_null_args(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char plaintext[128];
    unsigned char ciphertext[128] = {0x11, 0x22, 0x33};

    // NULL ciphertext
    int result1 = AES_encrypt(NULL, sizeof(ciphertext), key, CRYPTO_KEY_256, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result1, "Expected -1 when ciphertext is NULL");

    // NULL key
    int result2 = AES_encrypt(ciphertext, sizeof(ciphertext), NULL, CRYPTO_KEY_256, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result2, "Expected -1 when key is NULL");

    // NULL iv
    int result3 = AES_decrypt(ciphertext, sizeof(ciphertext), key, CRYPTO_KEY_256, NULL, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result3, "Expected -1 when IV is NULL");

    // NULL plaintext
    int result4 = AES_decrypt(ciphertext, sizeof(ciphertext), key, CRYPTO_KEY_256, iv, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result4, "Expected -1 when plaintext is NULL");
}

void test_AES_encrypt_zero_length(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char plaintext[128] = {0};
    unsigned char ciphertext[128];

    int result = AES_encrypt(plaintext, 0, key, CRYPTO_KEY_256, iv, ciphertext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 when plaintext_len = 0");
}

void test_AES_encrypt_invalid_key_size(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char plaintext[128] = "TestPlaintext";
    unsigned char ciphertext[128];

    int result = AES_encrypt(plaintext, strlen((char*)plaintext), key, (crypto_key_size_e)999, iv, ciphertext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 when key_size is invalid");
}

void test_AES_decrypt_null_args(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char plaintext[128];
    unsigned char ciphertext[128] = {0x11, 0x22, 0x33};

    // NULL ciphertext
    int result1 = AES_decrypt(NULL, sizeof(ciphertext), key, CRYPTO_KEY_256, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result1, "Expected -1 when ciphertext is NULL");

    // NULL key
    int result2 = AES_decrypt(ciphertext, sizeof(ciphertext), NULL, CRYPTO_KEY_256, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result2, "Expected -1 when key is NULL");

    // NULL iv
    int result3 = AES_decrypt(ciphertext, sizeof(ciphertext), key, CRYPTO_KEY_256, NULL, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result3, "Expected -1 when IV is NULL");

    // NULL plaintext
    int result4 = AES_decrypt(ciphertext, sizeof(ciphertext), key, CRYPTO_KEY_256, iv, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result4, "Expected -1 when plaintext is NULL");
}

void test_AES_decrypt_zero_length(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[16] = {0};
    unsigned char plaintext[128];
    unsigned char ciphertext[128] = {0xAA, 0xBB, 0xCC};

    int result = AES_decrypt(ciphertext, 0, key, CRYPTO_KEY_256, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 when ciphertext_len = 0");
}

void test_AES_decrypt_invalid_key_size(void) {
    unsigned char key[CRYPTO_KEY_256 / 8] = {0};
    unsigned char iv[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char plaintext[128];
    unsigned char ciphertext[128] = {0x11, 0x22, 0x33};

    int result = AES_decrypt(ciphertext, sizeof(ciphertext), key, (crypto_key_size_e)999, iv, plaintext);
    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 when key_size is invalid");
}

