#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "logger.h"

/**
 * @brief Возможные размеры ключей для алгоритмов шифрования
 */
typedef enum {
    CRYPTO_KEY_128 = 128, /**< Ключ длиной 128 бит */
    CRYPTO_KEY_192 = 192, /**< Ключ длиной 192 бита */
    CRYPTO_KEY_256 = 256  /**< Ключ длиной 256 бит */
} crypto_key_size_e;

/**
 * @brief Размер блока для AES в режиме CBC (в байтах)
 */
#define AES_CBC_BLOCK_SIZE 16

/**
 * @brief Максимальный размер ключа AES (в битах)
 */
#define MAX_CRYPTO_KEY_SIZE (CRYPTO_KEY_256)

/**
 * @brief Инициализирует криптографический модуль
 */
void crypto_init();

/**
 * @brief Генерирует случайный ключ указанной длины
 *
 * @param key       Указатель на буфер для записи сгенерированного ключа
 * @param key_size  Размер ключа (в битах), одно из значений crypto_key_size_e
 *
 * @return Код состояния выполнения
 * @retval 0    Успешное завершение
 * @retval >0   Ошибка при генерации ключа
 */
status_e generate_crypto_key(unsigned char *key, crypto_key_size_e key_size);

/**
 * @brief Генерирует случайный вектор инициализации (IV) для режима AES-CBC
 *
 * @param iv  Указатель на буфер длиной AES_CBC_BLOCK_SIZE байт для записи IV
 *
 * @return Код состояния выполнения
 * @retval 0    Успешное завершение
 * @retval >0   Ошибка при генерации IV
 */
status_e generate_AES_CBC_vector(unsigned char* iv);

/**
 * @brief Шифрует данные с использованием алгоритма AES
 *
 * @param plaintext      Указатель на входные данные (открытый текст)
 * @param plaintext_len  Длина входных данных в байтах
 * @param key            Указатель на ключ шифрования
 * @param key_size       Размер ключа (значение перечисления crypto_key_size_e)
 * @param iv             Указатель на вектор инициализации
 * @param ciphertext     Указатель на буфер для зашифрованных данных
 *
 * @return Размер зашифрованного сообщения (в байтах)
 * @retval >0       Успешное завершение
 * @retval <0       Ошибка при шифровании
 */
int AES_encrypt(const unsigned char* plaintext, size_t plaintext_len,
            const unsigned char* key, crypto_key_size_e key_size, const unsigned char* iv,
            unsigned char* ciphertext);

/**
 * @brief Расшифровывает данные, зашифрованные с использованием AES
 *
 * @param ciphertext     Указатель на входные данные (зашифрованный текст)
 * @param ciphertext_len Длина зашифрованных данных в байтах
 * @param key            Указатель на ключ расшифрования
 * @param key_size       Размер ключа (значение перечисления crypto_key_size_e)
 * @param iv             Указатель на вектор инициализации
 * @param plaintext      Указатель на буфер для расшифрованных данных
 *
 * @return Размер расшифрованного сообщения (в байтах)
 * @retval >0       Успешное завершение
 * @retval <0       Ошибка при шифровании
 */
int AES_decrypt(const unsigned char* ciphertext, size_t ciphertext_len,
            const unsigned char* key, crypto_key_size_e key_size, const unsigned char *iv,
            unsigned char* plaintext);

#endif // CRYPTO_H

