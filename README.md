# Тестовое задание ООО "РТТ"

Программное обеспечение для выполнения симметричного шифрования.

**Стек:** C17/Linux.

## Общее описание

На данный момент в ПО поддерживается только алгоритм AES в режиме CBC. Размер ключа может принимать значение 128/192/256 бит.

## Возможности

ПО имеет следующие возможности:

* **Шифрование** логина/пароля, введенных через консольный интерфейс, с последующим опциональным сохранением в файл.

* **Дешифрование** логина/пароля по пути к файлу, куда они ранее были сохранены после шифрования.

## Сборка

Для сборки в релизном режиме: `make all`

Для сборки в отладочном режиме: `make debug`

Для очистки последней выполненной сборки: `make clean`

## Юнит-тесты

Для сборки и запуска тестов: `make test`

Последний вывод в результате запуска:

```bash
=== Запуск тестов ===
tests/test_runner.c:103:test_AES_encrypt_null_args:PASS
tests/test_runner.c:104:test_AES_encrypt_zero_length:PASS
tests/test_runner.c:105:test_AES_encrypt_invalid_key_size:PASS
tests/test_runner.c:111:test_AES_decrypt_null_args:PASS
tests/test_runner.c:112:test_AES_decrypt_zero_length:PASS
tests/test_runner.c:113:test_AES_decrypt_invalid_key_size:PASS
tests/test_runner.c:114:test_AES_encrypt_decrypt_correct:PASS
tests/test_runner.c:120:test_validate_input_NULL:PASS
tests/test_runner.c:121:test_validate_input_empty:PASS
tests/test_runner.c:122:test_validate_input_space:PASS
tests/test_runner.c:123:test_validate_input_valid:PASS
tests/test_runner.c:129:test_get_login_valid:PASS
tests/test_runner.c:130:test_get_password_valid:PASS
tests/test_runner.c:131:test_get_crypto_key_valid:PASS
tests/test_runner.c:132:test_get_crypto_key_invalid_hex:PASS

-----------------------
15 Tests 0 Failures 0 Ignored 
OK
```

## Справка

```bash
./build/crypt_tool --help                                                                 ✔ 
Использование: crypt_tool [--decrypt input_file] [--encrypt output_file] [--help]
   --d, --decrypt   Войти в режим дешифрования (указать файл для чтения)
   --o, --encrypt   Войти в режим шифрования (опционально: указать файл для записи)
   --h, --help      Показать справку
```

## Пример работы

Работа обработчика шифрования:

```bash
./build/crypt_tool --encrypt=Test

[!] Введите логин: СотрудникРТТ
[!] Введите пароль: 
[!] Введите размер ключа (128 / 192 / 256 бит): 128
[+] Ключ шифрования: fd 8b d5 20 d2 06 19 93 6c 8c c2 ab 51 63 d7 c0
[+] Зашифрованный логин: 1f 7a 03 de 89 5e d7 26 65 72 e7 1d e4 0a 28 fb fc 86 35 f2 2a 66 5f 24 88 fe 7c c4 4d 15 41 af
[+] Зашифрованный пароль: 1a 1f 0d e0 af e3 78 13 d1 e0 f1 35 79 96 fe ff
[!] Результат шифрования сохранён в бинарном виде: Test
```

Работа обработчика дешифрования:

```bash
./build/crypt_tool --decrypt=Test

[!] Зашифрованный логин (32 байт): 1f 7a 03 de 89 5e d7 26 65 72 e7 1d e4 0a 28 fb fc 86 35 f2 2a 66 5f 24 88 fe 7c c4 4d 15 41 af
[!] Зашифрованный пароль (16 байт): 1a 1f 0d e0 af e3 78 13 d1 e0 f1 35 79 96 fe ff
[!] Введите ключ в hex-формате (пробелы между байтами): fd 8b d5 20 d2 06 19 93 6c 8c c2 ab 51 63 d7 c0
[!] Расшифрованный логин: СотрудникРТТ
[!] Расшифрованный пароль: 12345
```

## Интеграция с Valgrind

Запуск ПО через Valgrind может быть выполнен с помощью: `make check`.

Дополнительно следует указать аргументы через `ARGS`. Пример: `make check ARGS="--encrypt=Test" `.

## Зависимости

В ПО присутствуют следующие зависимости:

* Библиотека **тестирования** [Unity](https://github.com/ThrowTheSwitch/Unity)

* Библиотека **шифрования** [OpenSSL](https://github.com/openssl/openssl)
