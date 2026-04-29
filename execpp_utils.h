#pragma once

#ifndef _EXECPP_UTILS
#define _EXECPP_UTILS

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* is_extension(const char *filename, const char *extension) {
    char* dot = strrchr(filename, '.');
    if ((dot == NULL) || strcmp(dot, extension)) {
        return NULL;
    }
    return dot;
}
char* is_extension_c_cpp(const char *filename) {
    char* dot;
    char *extension[] = {".cpp", ".cc", ".cxx", ".c"};
   	for (int i = 0; i < 4; i++) {
    	if(dot = is_extension(filename, extension[i])) {
            return dot;
        }
   	}
    return NULL;
}
bool is_valid_arg(const char *arg) {
    const char *valid_args[] = {
        "--version", "--help", "--compile", 
        "--run", "--compile-run", "-c", "-r", "-cr", "-rc", NULL
    };
    
    for (int i = 0; valid_args[i] != NULL; i++) {
        if (!strcmp(arg, valid_args[i])) {
            return true;
        }
    }
    
    return false;
}
int check_gpp() {
#ifdef _WIN32
    int result = system("g++ --version >nul 2>&1");
#else
    int result = system("g++ --version >/dev/null 2>&1");
#endif // _WIN32
    return result == 0;
}

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#include <limits.h>

char* resolve_real_path(const char* path) {
    if (!path || *path == '\0') return NULL;

    char* result = NULL;

#ifdef _WIN32
    size_t len = strlen(path);
    char* win_path = (char*)malloc(len + 1);
    if (!win_path) return NULL;
    strcpy(win_path, path);
    for (size_t i = 0; i < len; i++) {
        if (win_path[i] == '/') win_path[i] = '\\';
    }

    DWORD buf_size = MAX_PATH;
    char* buffer = NULL;
    DWORD needed = 0;

    while(true) {
        buffer = (char*)realloc(buffer, buf_size);
        if (!buffer) {
            free(win_path);
            return NULL;
        }
        needed = GetFullPathNameA(win_path, buf_size, buffer, NULL);
        if (needed == 0) {
            free(win_path);
            free(buffer);
            return NULL;
        }
        if (needed < buf_size) break;
        buf_size = needed;
    }
    free(win_path);

    // 3. 返回结果
    result = (char*)malloc(needed + 1);
    if (result) strcpy(result, buffer);
    free(buffer);

#else
    // POSIX
    result = realpath(path, NULL);
#endif // _WIN32

    return result;
}
char* convert_to_real_path(char* path) {
    strcpy(path, resolve_real_path(path));
    return path;
}

char* strfcat(char *dest, char *src) {
    char *newString = (char *)malloc(strlen(dest) + strlen(src) + 2);

    strcpy(newString, src);    // newString = src
    strcat(newString, dest);   // newString += dest
    strcpy(dest, newString);   // dest = newString
    
    free(newString);
    return dest;
}

#endif // _EXECPP_UTILS