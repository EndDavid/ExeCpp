// Copyright (c) 2026 EnderDavid
// Licence: https://mit-license.org/

#pragma once

#ifndef _EXECPP_UTILS
#define _EXECPP_UTILS

#include "string_utils.h"

#define VERIFY_GCC_EXIST 1
#define VERIFY_PARA_LEGALITY 0   // These methods about verifying parameters' legality are considered as Not Planned, but will be reserved.

char *_CUSTOM_COMPILER;
char *_CUSTOM_ARGS;
char *_CUSTOM_COMPILER_GCC;
char *_CUSTOM_ARGS_GCC;

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
    	if((dot = is_extension(filename, extension[i]))) {
            return dot;
        }
   	}
    return NULL;
}
bool is_valid_arg(const char *arg) {
    const char *valid_args[] = {
        "--version", "--help",
		"--compile", "--run", "--compile-run", "--explicit",
		"-c", "-r", "-cr", "-rc", "-e",
		"--configure", "--display-config", NULL
    };
    
    for (int i = 0; valid_args[i] != NULL; i++) {
        if (!strcmp(arg, valid_args[i])) {
            return true;
        }
    }
    
    return false;
}

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
    #define quote(str) (strfcat(str, "\""), strcat(str, "\""))
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(path) mkdir(path, 0755)
    #define quote(str) (strfcat(str, "\'"), strcat(str, "\'"))
#endif // _WIN32

#include <limits.h>

#ifdef _WIN32
int ExecuteCmd(const char* cmd) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return -1;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    ZeroMemory(&pi, sizeof(pi));

    char* cmdLine = _strdup(cmd);

    BOOL success = CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, 
                                  CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    free(cmdLine);

    if (!success) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return -1;
    }

    CloseHandle(hWritePipe); 

    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exitCode;
}

#else
#define ExecuteCmd system
#endif // _WIN32

#ifdef _WIN32
char* resolve_real_path(const char* path) {
    if (!path || *path == '\0') return NULL;

    char* result = NULL;

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
        buffer = (char *)realloc(buffer, buf_size);
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

    result = (char*)malloc(needed + 1);
    if (result) strcpy(result, buffer);
    free(buffer);

    return result;
}
#else // POSIX
#define resolve_real_path(path) realpath((path), NULL)
#endif // _WIN32

#define convert_to_real_path(path) strcpy((path), resolve_real_path(path))

#if VERIFY_GCC_EXIST
int check_compiler(char *_compiler) {
    char *_Cmd = (char *)malloc(strlen(_compiler) + 30);
    strcpy(_Cmd, _compiler);

#ifdef _WIN32
    strcat(_Cmd, " --version >nul 2>&1");
#else
    strcat(_Cmd, " --version >/dev/null 2>&1");
#endif // _WIN32

    int result = system(_Cmd);
    free(_Cmd);

    return result == 0;
}
#else

#define check_compiler(_compiler) true

#endif

// These methods are considered as Not Planned, but will be reserved.
#if VERIFY_PARA_LEGALITY
bool _para_valid(char *_compiler, char *_para) {
	size_t cmdLen1 = strlen(_compiler) + strlen(*_para);
	char *exec_test = (char *)malloc(cmdLen1);

	snprintf(exec_test, cmdLen1, "%s %s .execpp_test_main.cpp -o .execpp_test_main", _compiler, _para);
    bool res = system(exec_test);

    free(exec_test);
    remove(".execpp_test_main.cpp");
    remove(".execpp_test_main");
    remove(".execpp_test_main.exe");
    return res == 0;
}
bool para_valid_gpp(char *_para) {
    FILE* tmp1 = fopen("./.execpp_test_main.cpp", "w");
    fputs("int main() {}\n", tmp1);
    fclose(tmp1);
    return _para_valid(_CUSTOM_COMPILER, _para);
}
bool para_valid_gcc(char *_para) {
    FILE* tmp1 = fopen("./.execpp_test_main.c", "w");
    fputs("int main() {}\n", tmp1);
    fclose(tmp1);
    return _para_valid(_CUSTOM_COMPILER_GCC, _para);
}
#else
#define para_valid_gpp(_para) true
#define para_valid_gcc(_para) true
#endif

#include <errno.h>

int create_directory(const char *path) {
    return MKDIR(path) == 0 || errno == EEXIST ? 0 : -1;
}

int verify_integrity() {
    if(create_directory("./.execpp_properties") != 0) return -1;

    FILE *fgpp = fopen("./.execpp_properties/custom_compiler", "r");
	if(fgpp == NULL) {
		return 1;
	} else {
		_CUSTOM_COMPILER = fgetl(fgpp);
        if(strchr(_CUSTOM_COMPILER, ' ')) quote(_CUSTOM_COMPILER);
		if(!check_compiler(_CUSTOM_COMPILER)) {
            fprintf(stderr, "execpp: G++ compiler %s invalid\n", _CUSTOM_COMPILER);
            return 1;
        }
		_CUSTOM_COMPILER_GCC = fgetl(fgpp);
        if(strchr(_CUSTOM_COMPILER_GCC, ' ')) quote(_CUSTOM_COMPILER_GCC);
		if(!check_compiler(_CUSTOM_COMPILER_GCC)) {
            fprintf(stderr, "execpp: GCC compiler %s invalid\n", _CUSTOM_COMPILER_GCC);
            return 1;
        }
	}
	fclose(fgpp);
	FILE *farg = fopen("./.execpp_properties/custom_args", "r");
	if(farg == NULL) {
		return 1;
	} else {
		_CUSTOM_ARGS = fgetl(farg);
		_CUSTOM_ARGS_GCC = fgetl(farg);
	}
	fclose(farg);

    FILE *fnote = fopen("./.execpp_properties/note", "w");
    fputs("For each config, C++ in line 1 and C in line 2.\n", fnote);
    fclose(fnote);

    return 0;
}

#endif // _EXECPP_UTILS