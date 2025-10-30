#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <stddef.h>

#include "logger.h"

/**
 * @brief Максимальная длина буфера для логина (включая завершающий NUL).
 */
#define MAX_LOGIN_LEN       128

/**
 * @brief Максимальная длина буфера для пароля (включая завершающий NUL).
 */
#define MAX_PASSWORD_LEN    128

/**
 * @brief Ввод логина из stdin (UTF-8)
 * @param buffer Буфер для записи логина
 * @param size Размер буфера в байтах
 * @return Код статуса выполнения как значение `status_e`
 */
status_e get_login(char* buffer, int size);

/**
 * @brief Ввод пароля с маскировкой из stdin
 * @param buffer Буфер для записи пароля
 * @param size Размер буфера в байтах
 * @return Код статуса выполнения как значение `status_e`
 */
status_e get_password(char* buffer, int size);

/**
 * @brief Получение размера криптографического ключа из stdin
 * @param size Указатель на переменную, в которую будет записан размер ключа
 * @return Код статуса выполнения как значение `status_e`
 */
status_e get_key_size(int* size);

/**
 * @brief Проверка корректности строки
 * @param str Строка для проверки
 * @return Код статуса выполнения как значение `status_e`
 */
status_e validate_input(const char *str);

/**
 * @brief Очистка буфера стандартного ввода
 * Функция считывает и отбрасывает все символы из stdin до символа '\n' или EOF
 */
void clear_input_buffer(void);

/**
 * @brief Получение криптографического ключа из stdin
 * @param key Буфер, куда будет записан ключ
 * @return Количество байт, записанных в key, или нулевое значение при ошибке.
 * @retval >0 Количество записанных байт.
 */
int get_crypto_key(unsigned char* key);

#endif // USER_INPUT_H

