#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Универсальный код возврата
 */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR
} status_e;

/**
 * @brief Уровень лога
 */
typedef enum log_type {
    LOG_INFO,
    LOG_ERROR
} log_type_e;

/**
 * @brief Логирует сообщение с определенным уровнем
 *
 * @param type  Уровень лога
 * @param fmt   Строка для форматирования
 */
void log_message(log_type_e type, const char* fmt, ...);

#endif // LOGGER_H
