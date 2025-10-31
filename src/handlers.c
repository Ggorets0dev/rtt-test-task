#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "logger.h"
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

    unsigned char key[MAX_CRYPTO_KEY_SIZE] = {0};
    unsigned char iv_login[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char iv_password[AES_CBC_BLOCK_SIZE] = {0};

    unsigned char* cipher_login = NULL;
    unsigned char* cipher_password = NULL;

    int cipher_login_len = 0;
    int cipher_password_len = 0;
    int key_size = 0;

    status_e status = STATUS_ERROR;
    FILE* file = NULL;

    do {
        // ==================
        // Читаем ввод пользователя
        // ==================
        do {
            status = get_login(login, sizeof(login));
        } while (status != STATUS_OK);

        do {
            status = get_password(password, sizeof(password));
        } while (status != STATUS_OK);

        do {
            status = get_key_size(&key_size);
            if (status != STATUS_OK)
                continue;

            status = generate_crypto_key(key, (crypto_key_size_e)key_size);
        } while (status != STATUS_OK);
        // ==================

        // ==================
        // Память под шифрованные буферы
        // ==================
        cipher_login = malloc(strlen(login) + AES_CBC_BLOCK_SIZE);
        cipher_password = malloc(strlen(password) + AES_CBC_BLOCK_SIZE);

        if (!cipher_login || !cipher_password) {
            log_message(LOG_ERROR, "Ошибка выделения памяти под зашифрованные данные");
            break;
        }
        // ==================

        // ==================
        // Генерация IV и шифрование логина
        // ==================
        if (generate_AES_CBC_vector(iv_login) != STATUS_OK) {
            log_message(LOG_ERROR, "Ошибка генерации IV для логина");
            break;
        }

        cipher_login_len = AES_encrypt(
            (unsigned char*)login, strlen(login),
            key, (crypto_key_size_e)key_size,
            iv_login, cipher_login
        );

        if (cipher_login_len == -1) {
            log_message(LOG_ERROR, "Ошибка шифрования логина");
            break;
        }
        // ==================

        // ==================
        // Генерация IV и шифрование пароля
        // ==================
        if (generate_AES_CBC_vector(iv_password) != STATUS_OK) {
            log_message(LOG_ERROR, "Ошибка генерации IV для пароля");
            break;
        }

        cipher_password_len = AES_encrypt(
            (unsigned char*)password, strlen(password),
            key, (crypto_key_size_e)key_size,
            iv_password, cipher_password
        );

        if (cipher_password_len == -1) {
            log_message(LOG_ERROR, "Ошибка шифрования пароля");
            break;
        }
        // ==================

        // ==================
        // Печать информации
        // ==================
        printf("[+] Ключ шифрования: ");
        print_hex_bytes(key, key_size / 8);

        printf("[+] Зашифрованный логин (%d байт): ", cipher_login_len);
        print_hex_bytes(cipher_login, cipher_login_len);

        printf("[+] Зашифрованный пароль (%d байт): ", cipher_password_len);
        print_hex_bytes(cipher_password, cipher_password_len);
        // ==================

		// ==================
		// Запись в файл
		// ==================
		if (out_file != NULL) {
			file = fopen(out_file, "wb");
			if (!file) {
				log_message(LOG_ERROR, "Ошибка открытия файла для записи");
				break;
			}

			size_t written = 0;
			bool write_error = false;

			written = fwrite(iv_login, 1, AES_CBC_BLOCK_SIZE, file);
			if (written != AES_CBC_BLOCK_SIZE) {
				log_message(LOG_ERROR, "Ошибка записи IV логина");
				write_error = true;
			}

			if (!write_error) {
				written = fwrite(iv_password, 1, AES_CBC_BLOCK_SIZE, file);
				if (written != AES_CBC_BLOCK_SIZE) {
				    log_message(LOG_ERROR, "Ошибка записи IV пароля");
				    write_error = true;
				}
			}

			if (!write_error) {
				written = fwrite(&cipher_login_len, sizeof(int), 1, file);
				if (written != 1) {
				    log_message(LOG_ERROR, "Ошибка записи длины логина");
				    write_error = true;
				}
			}

			if (!write_error) {
				written = fwrite(cipher_login, 1, cipher_login_len, file);
				if (written != (size_t)cipher_login_len) {
				    log_message(LOG_ERROR, "Ошибка записи зашифрованного логина");
				    write_error = true;
				}
			}

			if (!write_error) {
				written = fwrite(&cipher_password_len, sizeof(int), 1, file);
				if (written != 1) {
				    log_message(LOG_ERROR, "Ошибка записи длины пароля");
				    write_error = true;
				}
			}

			if (!write_error) {
				written = fwrite(cipher_password, 1, cipher_password_len, file);
				if (written != (size_t)cipher_password_len) {
				    log_message(LOG_ERROR, "Ошибка записи зашифрованного пароля");
				    write_error = true;
				}
			}

			if (write_error) {
				break;
			}

			printf("[!] Результат шифрования сохранён в бинарном виде: %s\n", out_file);
		}
		// ==================

        status = STATUS_OK;

    } while (0);

    // ==================
    // Очистка
    // ==================
    if (file) 				fclose(file);
    if (cipher_login) 		free(cipher_login);
    if (cipher_password) 	free(cipher_password);

    clear_crypto_buffer(iv_login, AES_CBC_BLOCK_SIZE);
    clear_crypto_buffer(iv_password, AES_CBC_BLOCK_SIZE);
    clear_crypto_buffer(key, sizeof(key));
    // ==================

    return status;
}

status_e handle_decrypt(const char* in_file) {
    FILE *file = NULL;
    unsigned char iv_login[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char iv_password[AES_CBC_BLOCK_SIZE] = {0};
    unsigned char key[MAX_CRYPTO_KEY_SIZE / 8] = {0};

    unsigned char* cipher_login = NULL;
    unsigned char* cipher_password = NULL;
    unsigned char* login = NULL;
    unsigned char* password = NULL;

    int cipher_login_len = 0, cipher_password_len = 0;
    int login_len = 0, password_len = 0;
    int key_size = 0;

    status_e status = STATUS_ERROR;

    do {
        file = fopen(in_file, "rb");
        if (!file) {
            log_message(LOG_ERROR, "Ошибка открытия файла");
            break;
        }

        // ==================
        // Читаем IV
        // ==================
        if (fread(iv_login, 1, AES_CBC_BLOCK_SIZE, file) != AES_CBC_BLOCK_SIZE ||
            fread(iv_password, 1, AES_CBC_BLOCK_SIZE, file) != AES_CBC_BLOCK_SIZE) {
            log_message(LOG_ERROR, "Ошибка чтения IV");
            break;
        }
        // ==================

        // ==================
        // Читаем зашифрованный логин
        // ==================
        if (fread(&cipher_login_len, sizeof(int), 1, file) != 1) {
            log_message(LOG_ERROR, "Ошибка чтения длины логина");
            break;
        }

        cipher_login = malloc(cipher_login_len);
        if (!cipher_login) {
            log_message(LOG_ERROR, "Ошибка выделения памяти для логина");
            break;
        }

        if (fread(cipher_login, 1, cipher_login_len, file) != (size_t)cipher_login_len) {
            log_message(LOG_ERROR, "Ошибка чтения зашифрованного логина");
            break;
        }
        // ==================

        // ==================
        // Читаем зашифрованный пароль
        // ==================
        if (fread(&cipher_password_len, sizeof(int), 1, file) != 1) {
            log_message(LOG_ERROR, "Ошибка чтения длины пароля");
            break;
        }

        cipher_password = malloc(cipher_password_len);
        if (!cipher_password) {
            log_message(LOG_ERROR, "Ошибка выделения памяти для пароля");
            break;
        }

        if (fread(cipher_password, 1, cipher_password_len, file) != (size_t)cipher_password_len) {
            log_message(LOG_ERROR, "Ошибка чтения зашифрованного пароля");
            break;
        }
        // ==================

        fclose(file);
        file = NULL;

        printf("[!] Зашифрованный логин (%d байт): ", cipher_login_len);
        print_hex_bytes(cipher_login, cipher_login_len);

        printf("[!] Зашифрованный пароль (%d байт): ", cipher_password_len);
        print_hex_bytes(cipher_password, cipher_password_len);

        // ==================
        // Получаем ключ
        // ==================
        do {
            key_size = get_crypto_key(key);
        } while (key_size == 0);
		// ==================

        // ==================
        // Расшифровываем
        // ==================
        login = malloc(cipher_login_len);
        password = malloc(cipher_password_len);

        if (!login || !password) {
            log_message(LOG_ERROR, "Ошибка выделения памяти для расшифрованных данных");
            break;
        }

        login_len = AES_decrypt(cipher_login, cipher_login_len, key, (crypto_key_size_e)(key_size * 8), iv_login, login);
        password_len = AES_decrypt(cipher_password, cipher_password_len, key, (crypto_key_size_e)(key_size * 8), iv_password, password);
		// ==================

        if (login_len > 0 && password_len > 0) {
            printf("[!] Расшифрованный логин: %s\n", (char*)login);
            printf("[!] Расшифрованный пароль: %s\n", (char*)password);
            status = STATUS_OK;
        } else {
            log_message(LOG_ERROR, "Ошибка расшифровки логина или пароля");
        }

    } while (0);

    // ==================
    // Очистка
    // ==================
    if (file) 				fclose(file);
    if (cipher_login) 		free(cipher_login);
    if (cipher_password) 	free(cipher_password);
    if (login) 				free(login);
    if (password) 			free(password);

    clear_crypto_buffer(iv_login, AES_CBC_BLOCK_SIZE);
    clear_crypto_buffer(iv_password, AES_CBC_BLOCK_SIZE);
    clear_crypto_buffer(key, sizeof(key));
    // ==================

    return status;
}


