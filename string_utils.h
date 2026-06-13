// Copyright (c) 2026 EnderDavid
// Licence: https://mit-license.org/

#pragma once

#ifndef _EXECPP_STRING_UTILS
#define _EXECPP_STRING_UTILS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

char* strfcat(char *dest, char *src) {
    char *newString = (char *)malloc(strlen(dest) + strlen(src) + 2);

    strcpy(newString, src);    // newString = src
    strcat(newString, dest);   // newString += dest
    strcpy(dest, newString);   // dest = newString
    
    free(newString);
    return dest;
}

char* fgetl(FILE* stream) {
    if (!stream) return NULL;

    size_t capacity = 16;      // Initial buffer size
    size_t length   = 0;       // Number of characters read
    char* buf = (char *)malloc(capacity);
    if (!buf) return NULL;

    int ch;
    while ((ch = fgetc(stream)) != EOF && ch != '\n') {
        if (length + 1 >= capacity) {
            size_t new_cap = capacity << 1;
            char* tmp = (char *)realloc(buf, new_cap);
            if (!tmp) {
                free(buf);
                return NULL;
            }
            buf = tmp;
            capacity = new_cap;
        }
        buf[length++] = (char)ch;
    }

    // Boundary handling
    if (length == 0 && ch == EOF) {
        free(buf);
        return NULL;
    }

    buf[length] = '\0';
    return buf;
}

#endif // _EXECPP_STRING_UTILS