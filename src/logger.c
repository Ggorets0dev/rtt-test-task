#include "logger.h"

__attribute__((weak)) void log_message(log_type_e type, const char *fmt, ...) {
    const char *type_str = (type == LOG_ERROR) ? "-" : "!";
    FILE *out = (type == LOG_ERROR) ? stderr : stdout;

    // Вывод лога
    fprintf(out, "[%s] ", type_str);

    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fprintf(out, "\n");
    fflush(out);
}
