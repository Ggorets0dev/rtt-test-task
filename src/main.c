#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <getopt.h>

#include "logger.h"
#include "handlers.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");

    const char* input_file = NULL;
    const char* output_file = NULL;

    // Определение опций
    static struct option long_options[] = {
        {"decrypt",  required_argument, 0, 'd'},
        {"encrypt", optional_argument, 0, 'e'},
        {"help",   no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    // Разбор аргументов
    while ((c = getopt_long(argc, argv, "e:d:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                input_file = optarg;
                handle_decrypt(input_file);
                break;
            case 'e':
                output_file = optarg;
                handle_encrypt(output_file);
                break;
            case 'h':
                handle_help();
                break;
            case '?':
                log_message(LOG_ERROR, "Неизвестная опция или пропущен аргумент");
                handle_help();
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    return EXIT_SUCCESS;
}

