#include <string.h>

#include "unity.h"
#include "user_input.h" // Тестируемая библиотека

// === Подмена STDIN ===
static FILE* redirect_stdin(const char *input) {
    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);
    fputs(input, tmp);
    rewind(tmp);
    stdin = tmp;
    return tmp;
}

void test_validate_input_NULL(void) {
    TEST_ASSERT_EQUAL(STATUS_ERROR, validate_input(NULL));
}

void test_validate_input_empty(void) {
    TEST_ASSERT_EQUAL(STATUS_ERROR, validate_input(""));
}

void test_validate_input_space(void) {
    TEST_ASSERT_EQUAL(STATUS_ERROR, validate_input("test user"));
}

void test_validate_input_valid(void) {
    TEST_ASSERT_EQUAL(STATUS_OK, validate_input("user123"));
}

// -------------------------
// get_crypto_key()
// -------------------------
void test_get_crypto_key_valid(void) {
    const char* msg = "AA BB CC\n";
    unsigned char key[10];

    FILE *f = redirect_stdin(msg);
    int count = get_crypto_key(key);
    fclose(f);

    TEST_ASSERT_EQUAL_INT(3, count);
    TEST_ASSERT_EQUAL_UINT8(0xAA, key[0]);
    TEST_ASSERT_EQUAL_UINT8(0xBB, key[1]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, key[2]);
}

void test_get_crypto_key_invalid_hex(void) {
    const char* msg = "AA LB G\n";
    unsigned char key[10];

    FILE *f = redirect_stdin(msg);
    int count = get_crypto_key(key);
    fclose(f);

    TEST_ASSERT_EQUAL_INT(0, count);  // должно вернуть 0 из-за ошибки
}

// -------------------------
// get_login() и get_password()
// -------------------------
void test_get_login_valid(void) {
    const char* msg = "valid_login\n";
    char buf[20];
    status_e status;

    FILE *f = redirect_stdin(msg);
    status = get_login(buf, sizeof(buf));
    fclose(f);

    TEST_ASSERT_EQUAL_MESSAGE(STATUS_OK, status, "Expected STATUS_OK in login read");
    TEST_ASSERT_EQUAL_MESSAGE(0, strncmp(msg, buf, strlen(msg) - 1), "Input from STDIN is not equal with buffer");
}

void test_get_password_valid(void) {
    const char* msg = "valid_password\n";
    char buf[20];
    status_e status;

    FILE *f = redirect_stdin(msg);
    status = get_password(buf, sizeof(buf));
    fclose(f);

    TEST_ASSERT_EQUAL_MESSAGE(STATUS_OK, status, "Expected STATUS_OK in login read");
    TEST_ASSERT_EQUAL_MESSAGE(0, strncmp(msg, buf, strlen(msg) - 1), "Input from STDIN is not equal with buffer");
}