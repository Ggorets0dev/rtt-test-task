#include <string.h>
#include <stdbool.h>

#include "handlers.h"
#include "crypto.h"
#include "user_input.h"

// ==================
// Статические функции
// ==================
/**
 * @brief Вывести массив байт в виде строки (hex)
 * @param data     Данные для вывода
 * @param size     Размер данных
 */
static void print_hex_bytes(const unsigned char* data, size_t size);
// ==================

static void print_hex_bytes(const unsigned char* data, size_t size) {
    if (size == 0) {
        printf("(пусто)\n");
        return;
    }

    for (size_t i = 0; i < size; i++) {
        printf("%02x", data[i]);
        if (i + 1 < size) {  // добавляем пробел только если это не последний байт
            printf(" ");
        }
    }

    printf("\n");
}

void handle_help(void) {
    printf("Использование: crypt_tool [--decrypt input_file] [--encrypt output_file] [--help]\n");
    printf("   --d, --decrypt   Войти в режим дешифрования (указать файл для чтения)\n");
    printf("   --o, --encrypt   Войти в режим шифрования (опционально: указать файл для записи)\n");
    printf("   --h, --help      Показать справку\n");
}

status_e handle_encrypt(const char* out_file) {
    char login[MAX_LOGIN_LEN];
    char password[MAX_PASSWORD_LEN];

    unsigned char key[MAX_CRYPTO_KEY_SIZE];

    unsigned char iv_login[AES_CBC_BLOCK_SIZE];
    unsigned char iv_password[AES_CBC_BLOCK_SIZE];

    int cipher_login_len;
    int cipher_password_len;

    int key_size;

    status_e status;

    // ==================
    // Читаем пользовательский ввод
    // ==================
    do {
        status = get_login(login, sizeof(login));
    } while (status != STATUS_OK);

    do {
        status = get_password(password, sizeof(password));
    } while (status != STATUS_OK);

    do {
        status = get_key_size(&key_size);

        if (status != STATUS_OK) {
            continue;
        }

        status = generate_crypto_key(key, (crypto_key_size_e)key_size);
    } while (status != STATUS_OK);
    // ==================

    unsigned char* cipher_login = malloc(strlen(login) + AES_CBC_BLOCK_SIZE);
    unsigned char* cipher_password = malloc(strlen(password) + AES_CBC_BLOCK_SIZE);

    if (cipher_login == NULL || cipher_password == NULL) {
        return STATUS_ERROR;
    }

    // ==================
    // Шифруем данные с помощью введенных параметров
    // ==================
    status = generate_AES_CBC_vector(iv_login);

    if (status != STATUS_OK) {
        free(cipher_login);
        free(cipher_password);
        return STATUS_ERROR;
    }

    cipher_login_len = AES_encrypt((unsigned char *)login, strlen(login), key, (crypto_key_size_e)key_size, iv_login, cipher_login);

    if (cipher_login_len == -1) {
        free(cipher_login);
        free(cipher_password);
        return STATUS_ERROR;
    }

    status = generate_AES_CBC_vector(iv_password);

    if (status != STATUS_OK) {
        free(cipher_login);
        free(cipher_password);
        return STATUS_ERROR;
    }

    cipher_password_len = AES_encrypt((unsigned char *)password, strlen(password), key, (crypto_key_size_e)key_size, iv_password, cipher_password);

    if (cipher_password_len == -1) {
        free(cipher_login);
        free(cipher_password);
        return STATUS_ERROR;
    }
    // ==================

    printf("[+] Ключ шифрования: ");
    print_hex_bytes(key, key_size / 8);

    printf("[+] Зашифрованный логин: ");
    print_hex_bytes(cipher_login, cipher_login_len);

    printf("[+] Зашифрованный пароль: ");
    print_hex_bytes(cipher_password, cipher_password_len);

    // ==================
    // Пишем результат в файл, если требуется
    // ==================
    if (out_file != NULL) {
        FILE *file = fopen(out_file, "wb");
        if (file == NULL) {
            log_message(LOG_ERROR, "Ошибка открытия файла для записи");
            return STATUS_ERROR;
        }

        // Сначала нужно записать длину каждого блока, чтобы потом знать, сколько читать

        fwrite(iv_login, 1, AES_CBC_BLOCK_SIZE, file);
        fwrite(iv_password, 1, AES_CBC_BLOCK_SIZE, file);

        fwrite(&cipher_login_len, sizeof(int), 1, file);
        fwrite(cipher_login, 1, cipher_login_len, file);

        fwrite(&cipher_password_len, sizeof(int), 1, file);
        fwrite(cipher_password, 1, cipher_password_len, file);

        fclose(file);
        printf("[!] Результат шифрования сохранён в бинарном виде: %s\n", out_file);
    }
    // ==================

    free(cipher_login);
    free(cipher_password);

    return STATUS_OK;
}

status_e handle_decrypt(const char* in_file) {
    FILE *file = fopen(in_file, "rb");  // бинарный режим

    if (!file) {
        log_message(LOG_ERROR, "Ошибка открытия файла");
        return STATUS_ERROR;
    }

    unsigned char iv_login[AES_CBC_BLOCK_SIZE];
    unsigned char iv_password[AES_CBC_BLOCK_SIZE];
    unsigned char key[MAX_CRYPTO_KEY_SIZE / 8];

    int cipher_login_len, cipher_password_len;
    int login_len, password_len;
    int key_size;

    // ==================
    // Читаем IV
    // ==================
    if (fread(iv_login, 1, AES_CBC_BLOCK_SIZE, file) != AES_CBC_BLOCK_SIZE) {
        log_message(LOG_ERROR, "Ошибка чтения IV логина");
        fclose(file);
        return STATUS_ERROR;
    }

    if (fread(iv_password, 1, AES_CBC_BLOCK_SIZE, file) != AES_CBC_BLOCK_SIZE) {
        log_message(LOG_ERROR, "Ошибка чтения IV пароля");
        fclose(file);
        return STATUS_ERROR;
    }
    // ==================

    // ==================
    // Читаем логи и пароль
    // ==================
    if (fread(&cipher_login_len, sizeof(int), 1, file) != 1) {
        log_message(LOG_ERROR, "Ошибка чтения длины логина");
        fclose(file);
        return STATUS_ERROR;
    }

    unsigned char* cipher_login = malloc(cipher_login_len);

    if (!cipher_login) {
        log_message(LOG_ERROR, "Ошибка выделения памяти для логина");
        fclose(file);
        return STATUS_ERROR;
    }

    if (fread(cipher_login, 1, cipher_login_len, file) != (size_t)cipher_login_len) {
        log_message(LOG_ERROR, "Ошибка чтения зашифрованного логина");
        free(cipher_login);
        fclose(file);
        return STATUS_ERROR;
    }

    if (fread(&cipher_password_len, sizeof(int), 1, file) != 1) {
        log_message(LOG_ERROR, "Ошибка чтения длины пароля");
        free(cipher_login);
        fclose(file);
        return STATUS_ERROR;
    }

    unsigned char* cipher_password = malloc(cipher_password_len);

    if (!cipher_password) {
        log_message(LOG_ERROR, "Ошибка выделения памяти для пароля");
        free(cipher_login);
        fclose(file);
        return STATUS_ERROR;
    }

    if (fread(cipher_password, 1, cipher_password_len, file) != (size_t)cipher_password_len) {
        log_message(LOG_ERROR, "Ошибка чтения зашифрованного пароля");
        free(cipher_login);
        free(cipher_password);
        fclose(file);
        return STATUS_ERROR;
    }
    // ==================

    fclose(file);

    printf("[!] Зашифрованный логин (%d байт): ", cipher_login_len);
    print_hex_bytes(cipher_login, cipher_login_len);

    printf("[!] Зашифрованный пароль (%d байт): ", cipher_password_len);
    print_hex_bytes(cipher_password, cipher_password_len);

    // Получаем ключ шифрования от пользователя
    key_size = get_crypto_key(key);

    // ==================
    // Расшифровываем данные с помощью введенных параметров
    // ==================
    unsigned char* login = malloc(cipher_login_len);
    unsigned char* password = malloc(cipher_password_len);

    if (!login || !password) {
        free(cipher_login);
        free(cipher_password);

        if (login)      free(login);
        if (password)   free(password);

        log_message(LOG_ERROR, "Ошибка чтения зашифрованного пароля");
        return STATUS_ERROR;
    }

    login_len = AES_decrypt(cipher_login, cipher_login_len, key, (crypto_key_size_e)(key_size * 8), iv_login, login);
    password_len = AES_decrypt(cipher_password, cipher_password_len, key, (crypto_key_size_e)(key_size * 8), iv_password, password);
    // ==================

    if (login_len > 0 && password_len > 0) {
        printf("[!] Расшифрованный логин: %s\n", (char*)login);
        printf("[!] Расшифрованный пароль: %s\n", (char*)password);
    } else {
        log_message(LOG_ERROR, "Ошибка расшифровки логина или пароля");
    }

    free(cipher_login);
    free(cipher_password);
    free(login);
    free(password);

    return !(login_len > 0 && password_len > 0);
}

