#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <stddef.h>

#include "logger.h"

// Максимальные размеры буфера
#define MAX_LOGIN_LEN       128
#define MAX_PASSWORD_LEN    128

// Функция ввода логина (UTF-8)
status_e get_login(char* buffer, size_t size);

// Функция ввода пароля (маскированный ввод)
status_e get_password(char* buffer, size_t size);

status_e get_key_size(int* size);

// Проверка корректности строки (валидация)
status_e validate_input(const char *str);

// Очистка буфера ввода (stdin)
void clear_input_buffer(void);

int get_crypto_key(unsigned char* key);

#endif // USER_INPUT_H

