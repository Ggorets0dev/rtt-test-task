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

## Зависимости

В ПО присутствуют следующие зависимости:

* Библиотека тестирования [Unity]([GitHub - ThrowTheSwitch/Unity: Simple unit testing for C](https://github.com/ThrowTheSwitch/Unity))

* Библиотека шифрования [OpenSSL]([GitHub - openssl/openssl: TLS/SSL and crypto library](https://github.com/openssl/openssl))


