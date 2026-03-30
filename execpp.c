#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#define aprintf(dest, ...) snprintf(dest + strlen(dest), sizeof(dest) - strlen(dest), __VA_ARGS__) // deprecated...
#define _CUSTOM_COMPILER "g++"
#define _CUSTOM_ARGS "-Wall -Wextra -std=c++23"

void help() {
    puts("Usage: execpp [FILE]");
    puts("Compile and excute a single cpp file.");
}

char* is_extension(const char *filename, const char *extension) {
    char* dot = strrchr(filename, '.');
    if ((dot == NULL) || strcmp(dot, extension)) {
        return NULL;
    }
    return dot;
}

int main(int argc, char *argv[]) {
    if(argc == 1) {
        help();
        return 0;
    }
    if(argc > 2) {
        fputs("execpp: Invalid args\n", stderr);
        puts("Use \"execpp --help\" for more information.");
        return -1;
    }
    if(!strcmp(argv[1],"--help")) {
        help();
        return 0;
    }

    bool res = false;
    char* dot;
    char *extension[] = {".cpp", ".cc", ".cxx"};
    for (int i = 0; i < 3; i++) {
        if((dot = is_extension(argv[1], extension[i])) != NULL) {
            res = true;
            break;
        }
    }
    if(!res) {
        fputs("execpp: Invalid file format\n", stderr);
        puts("Use \"execpp --help\" for more information.");
        return -1;
    }

    size_t fileLen = strlen(argv[1]);
    size_t cmdLen = strlen(_CUSTOM_COMPILER) + strlen(_CUSTOM_ARGS) + (fileLen << 1) + 50;
    char *FilenameNoExt = (char *)malloc(fileLen + 1);

    strncpy(FilenameNoExt, argv[1], dot - argv[1]);
    FilenameNoExt[dot - argv[1]] = '\0';

    const char Prog[] = _CUSTOM_COMPILER;
    const char Args[] = _CUSTOM_ARGS;
    char *Exec = (char *)malloc(cmdLen);

    snprintf(Exec, cmdLen, "%s %s %s -o \"%s", Prog, Args, argv[1], FilenameNoExt);

#ifdef _WIN32
    system(strcat(Exec, ".exe\""));
    /*
    char *FilenameExe = (char *)malloc(fileLen + 5);
    strcpy(FilenameExe, FilenameNoExt);
    strcat(FilenameExe, ".exe");
    */
    char *AbsPath = (char *)malloc(fileLen + 10);
    strcpy(AbsPath, ".\\");
    system(strcat(AbsPath, FilenameNoExt));

    //free(FilenameExe);
    free(AbsPath);
#else
    system(strcat(Exec, "\""));
    system(FilenameNoExt);
#endif // _WIN32

    free(Exec);
    free(FilenameNoExt);
    return 0;
}