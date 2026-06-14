// Copyright (c) 2026 EnderDavid
// Licence: https://mit-license.org/

#pragma once

#ifndef _EXECPP_STRING_UTILS
#define _EXECPP_STRING_UTILS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

char* strfcat_realloc(char **dest, const char *src) {
    if (!dest || !src) return dest ? *dest : NULL;

    size_t len_src = strlen(src);
    size_t len_dest = *dest ? strlen(*dest) : 0;
    size_t new_size = len_src + len_dest + 1;

    char *temp = realloc(*dest, new_size);
    if (!temp) return NULL;
    
    *dest = temp;

    if (len_dest > 0) {
        memmove(*dest + len_src, *dest, len_dest + 1);
    } else {
        (*dest)[len_src] = '\0';
    }

    memcpy(*dest, src, len_src);

    return *dest;
}

#define strfcat(dest, src) strfcat_realloc(&(dest), src)

char* fgetl(FILE* stream) {
    if (!stream) return NULL;

    size_t capacity = 16;      // Initial buffer size
    size_t length   = 0;       // Number of characters read
	
    char* buf = (char *)malloc(capacity);
    if (!buf) return NULL;

    int ch;
    while ((ch = fgetc(stream)) != EOF && ch != '\n') {
		if (ch == '\r') {
            int next_ch = fgetc(stream);
            if (next_ch != '\n' && next_ch != EOF) {
                ungetc(next_ch, stream); 
            }
            break;
        }

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