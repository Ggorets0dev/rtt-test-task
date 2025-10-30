#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "user_input.h"

// ==================
// Статические функции
// ==================
/**
 * @brief Безопасное извлечение данных из stdin с учетом размера буфера и
 * завершением чтения
 * @param buffer     Размер ключа (в битах)
 * @param size     Размер ключа (в битах)
 * @return Универсальный статус возврата
 */
static status_e safe_fgets(char *buffer, int size);
// ==================

static status_e safe_fgets(char *buffer, const int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return STATUS_ERROR;
    }

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        return STATUS_OK;
    } else {
        // Если пользователь ввёл строку длиннее буфера — очищаем stdin
        clear_input_buffer();
        return STATUS_ERROR;
    }
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

status_e validate_input(const char *str) {
    if (str == NULL || strlen(str) == 0) {
        return STATUS_ERROR;
    }

    for (size_t i = 0; str[i] != '\0'; ++i) {
        const unsigned char ch = (unsigned char)str[i];

        if (isspace(ch)) {
            return STATUS_ERROR;
        }
    }
    return STATUS_OK;
}

status_e get_login(char *buffer, const int size) {
    int status;

    setlocale(LC_ALL, "");

    printf("[!] Введите логин: ");
    fflush(stdout);

    status = safe_fgets(buffer, size);

    if (status != STATUS_OK) {
        log_message(LOG_ERROR, "Ошибка: не удалось считать введенные данные");
        return STATUS_ERROR;
    }

    status = validate_input(buffer);

    if (status != STATUS_OK) {
        log_message(LOG_ERROR, "Ошибка: логин не прошел валидацию");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

status_e get_password(char *buffer, const int size) {
    int status;
    struct termios oldt, newt;

    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        log_message(LOG_ERROR, "tcgetattr");
        return STATUS_ERROR;
    }

    newt = oldt;
    newt.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        log_message(LOG_ERROR, "tcsetattr");
        return STATUS_ERROR;
    }

    printf("[!] Введите пароль: ");
    fflush(stdout);

    status = safe_fgets(buffer, size);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // всегда восстанавливаем
    printf("\n");

    if (status != STATUS_OK) {
        log_message(LOG_ERROR, "Ошибка: не удалось считать введенные данные");
        return STATUS_ERROR;
    }

    status = validate_input(buffer);

    if (status != STATUS_OK) {
        log_message(LOG_ERROR, "Ошибка: пароль не прошел валидацию");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

status_e get_key_size(int* size) {
    if (size == NULL) {
        log_message(LOG_ERROR, "Неверный аргумент: size = NULL");
        return STATUS_ERROR;
    }

    char buffer[8];  // с запасом под "256" и '\0'

    printf("[!] Введите размер ключа (128 / 192 / 256 бит): ");
    fflush(stdout);

    if (safe_fgets(buffer, sizeof(buffer)) != STATUS_OK) {
        log_message(LOG_ERROR, "Ошибка чтения ввода");
        return STATUS_ERROR;
    }

    // Преобразуем строку в число (без проверки диапазона)
    char* endptr = NULL;
    *size = (int)strtol(buffer, &endptr, 10);

    if (buffer[0] == '\0' || endptr == buffer) {
        log_message(LOG_ERROR, "Пустой или некорректный ввод");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int get_crypto_key(unsigned char* key) {
    char line[100];
    char* token;
    int count = 0;

    printf("[!] Введите ключ в hex-формате (пробелы между байтами): ");

    if (!fgets(line, sizeof(line), stdin)) {
        return 0;
    }

    // Удаляем перенос строки в конце
    line[strcspn(line, "\n")] = '\0';

    // Разбиваем строку по пробелам
    token = strtok(line, " \t");

    while (token != NULL) {
        // Пропускаем пустые токены
        if (strlen(token) == 0) {
            token = strtok(NULL, " \t");
            continue;
        }

        // Проверяем, что это 2 символа
        if (strlen(token) != 2) {
            log_message(LOG_ERROR, "Ошибка: неверный формат байта (ожидается 2 hex-символа)", token);
            return 0;
        }

        // Проверяем, что оба символа — валидные hex
        for (int i = 0; i < 2; i++) {
            if (!isxdigit(token[i])) {
                log_message(LOG_ERROR, "Ошибка: содержит не hex-символ", token);
                return 0;
            }
        }

        // Преобразуем hex-строку в байт
        unsigned int byte;
        sscanf(token, "%2x", &byte);
        key[count++] = (unsigned char)byte;

        token = strtok(NULL, " \t");
    }

    return count;
}

