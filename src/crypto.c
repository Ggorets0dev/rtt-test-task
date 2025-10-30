#include <string.h>
#include <stdio.h>

#include "crypto.h"

// ==================
// Статические функции
// ==================
/**
 * @brief Получить алгоритм шифрования AES с помощью размера ключа
 * @param key_size     Размер ключа (в битах)
 * @return Указатель на функцию шифрования AES
 */
static const EVP_CIPHER* get_AES_cipher_type(crypto_key_size_e key_size);
// ==================

static const EVP_CIPHER* get_AES_cipher_type(const crypto_key_size_e key_size) {
    if (key_size == CRYPTO_KEY_128) {
        return EVP_aes_128_cbc();
    } else if (key_size == CRYPTO_KEY_192) {
        return EVP_aes_192_cbc();
    } else if (key_size == CRYPTO_KEY_256) {
        return EVP_aes_256_cbc();
    } else {
        // Недопустимая длина ключа
        return NULL;
    }
}

void crypto_init() {
    // Инициализация OpenSSL
    OpenSSL_add_all_algorithms();
}

status_e generate_crypto_key(unsigned char *key, const crypto_key_size_e key_size) {
    const int bytes = (int)key_size / 8;

    if (key_size != CRYPTO_KEY_128 && key_size != CRYPTO_KEY_192 && key_size != CRYPTO_KEY_256) {
        log_message(LOG_ERROR, "Неверная длина ключа: %d бит", key_size);
        return EXIT_FAILURE;
    }

    if (RAND_bytes(key, bytes) != 1) {
        log_message(LOG_ERROR, "Ошибка генерации ключа");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

status_e generate_AES_CBC_vector(unsigned char* iv) {
    if (RAND_bytes(iv, AES_CBC_BLOCK_SIZE) == 1) {
        return STATUS_OK;
    } else {
        return STATUS_ERROR;
    }
}

int AES_encrypt(const unsigned char* plaintext, const size_t plaintext_len,
            const unsigned char* key, const crypto_key_size_e key_size, const unsigned char* iv,
            unsigned char* ciphertext) {

    EVP_CIPHER_CTX* ctx = NULL;
    int len;
    int ciphertext_len;

    // ====================
    // Валидация аргументов
    // ====================
    if (plaintext == NULL || key == NULL || iv == NULL || ciphertext == NULL || plaintext_len == 0) {
        log_message(LOG_ERROR, "В качестве аргументов функции шифрования переданы некорректные значения");
        return -1;
    }
    // ====================

    const EVP_CIPHER* cipher_type = get_AES_cipher_type(key_size);

    if (cipher_type == NULL) {
        log_message(LOG_ERROR, "Не удалось определить функцию шифрования");
        return -1;
    }

    ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        log_message(LOG_ERROR, "Не удалось создать контекст шифрования");
        return -1;
    }

    // Инициализация шифрования с выбранным типом AES
    if (EVP_EncryptInit_ex(ctx, cipher_type, NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);

        log_message(LOG_ERROR, "Не удалось инициализировать контекст шифрования выбранными настройками");
        return -1;
    }

    // Основной цикл шифрования
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, (int)plaintext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);

        log_message(LOG_ERROR, "Не удалось выполнить шифрование с помощью указанного алгоритма");
        return -1;
    }

    ciphertext_len = len;

    // Финализируем шифрование
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int AES_decrypt(const unsigned char *ciphertext, const size_t ciphertext_len,
            const unsigned char *key, const crypto_key_size_e key_size, const unsigned char *iv,
            unsigned char *plaintext) {

    EVP_CIPHER_CTX *ctx = NULL;
    int len;
    int plaintext_len;

    // ====================
    // Валидация аргументов
    // ====================
    if (ciphertext == NULL || key == NULL || iv == NULL || plaintext == NULL || ciphertext_len == 0) {
        log_message(LOG_ERROR, "В качестве аргументов функции шифрованя переданы некорректные значения");
        return -1;
    }
    // ====================

    const EVP_CIPHER* cipher_type = get_AES_cipher_type(key_size);

    if (cipher_type == NULL) {
        log_message(LOG_ERROR, "Не удалось определить функцию шифрования");
        return -1;
    }

    ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        log_message(LOG_ERROR, "Не удалось создать контекст шифрования");
        return -1;
    }

    if (EVP_DecryptInit_ex(ctx, cipher_type, NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);

        log_message(LOG_ERROR, "Не удалось инициализировать контекст шифрования выбранными настройками");
        return -1;
    }

    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, (int)ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);

        log_message(LOG_ERROR, "Не удалось выполнить расшифрование с помощью указанного алгоритма");
        return -1;
    }

    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

