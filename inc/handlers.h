#ifndef HANDLERS_H
#define HANDLERS_H

#include "logger.h"

/**
 * @brief Выводит справку о возможностях ПО
 */
void handle_help(void);

/**
 * @brief Выполнить шифрование с последующим сохранением в файл
 * @param out_file     Файл для сохранения результатов шифрования (может быть NULL)
 * @return Статус вызова обработчика
 */
status_e handle_encrypt(const char* out_file);

/**
 * @brief Выполнить дешифрование с помощью данных из файла
 * @param in_file     Файл для чтения результатов шифрования
 * @return Статус вызова обработчика
 */
status_e handle_decrypt(const char* in_file);

#endif // HANDLERS_H
