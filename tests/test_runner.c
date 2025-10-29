#include "unity.h"

void test_encrypt_correct(void);

void test_encrypt_wrong_param(void);

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_encrypt_correct);
    RUN_TEST(test_encrypt_wrong_param);

    return UNITY_END();
}

