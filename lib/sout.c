/*
Copyright (C) 2026 waxodium <waxodium@proton.me>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdarg.h>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <unistd.h>
#endif


void write_chunk(const char *buf, int length) {
    if (length <= 0) return;
#if defined(_WIN32)
    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, length, &written, NULL);
#else
    write(1, buf, length);
#endif
}

void sout(const char *format, ...) {
    static char buffer[1024];
    int pos = 0;
    va_list arguments;
    va_start(arguments, format);

    for (const char *cursor = format; *cursor != '\0'; cursor++) {
        if (pos >= 1023) {
            write_chunk(buffer, pos);
            pos = 0;
        }

        if (*cursor == '%' && *(cursor + 1) != '\0') {
            cursor++;
            switch (*cursor) {
                case 's': {
                    const char *text = va_arg(arguments, const char *);
                    if (text == NULL) text = "(null)";
                    while (*text != '\0') {
                        if (pos >= 1023) {
                            write_chunk(buffer, pos);
                            pos = 0;
                        }
                        buffer[pos++] = *text++;
                    }
                    break;
                }
                case 'd': {
                    int number = va_arg(arguments, int);
                    char digits[12];
                    int p = 0;
                    if (number == 0) {
                        buffer[pos++] = '0';
                    } else {
                        if (number < 0) {
                            buffer[pos++] = '-';
                            number = -number;
                        }
                        while (number > 0) {
                            digits[p++] = (number % 10) + '0';
                            number /= 10;
                        }
                        while (p > 0) {
                            if (pos >= 1023) {
                                write_chunk(buffer, pos);
                                pos = 0;
                            }
                            buffer[pos++] = digits[--p];
                        }
                    }
                    break;
                }
                case 'c': {
                    buffer[pos++] = (char)va_arg(arguments, int);
                    break;
                }
                case '%': {
                    buffer[pos++] = '%';
                    break;
                }
                default: {
                    buffer[pos++] = '%';
                    buffer[pos++] = *cursor;
                    break;
                }
            }
        } else {
            buffer[pos++] = *cursor;
        }
    }

    if (pos > 0) {
        write_chunk(buffer, pos);
    }

    va_end(arguments);
}
