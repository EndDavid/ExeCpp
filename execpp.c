#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "execpp_utils.h"

#define _CUSTOM_COMPILER "g++"
#define _CUSTOM_ARGS "-Wall -Wextra -std=c++23"
#define _CUSTOM_COMPILER_GCC "gcc"
#define _CUSTOM_ARGS_GCC "-Wall -Wextra -std=c23"

void help();
void version();

int main(int argc, char *argv[]) {
	//init...

	if(!check_gpp()) {
		fputs("execpp: g++ was not found\n", stderr);
		return -1;
	}
    if(argc == 1) {
        puts("execpp: No input file");
        help();
        return 0;
    }
    if(argc > 4) {
        fputs("execpp: Too many arguments\n", stderr);
        puts("Use \"execpp --help\" for more information.");
        return -1;
    }

	// check file & help or version arg...

	bool has_file = false, c_lang = false;
	char* dot;
	int fi = -1;
	for (int i = 1; i < argc; i++) {
		if(is_valid_arg(argv[i])) {
			if(!strcmp(argv[i], "--help")) {
				help();
				return 0;
			} else if(!strcmp(argv[i], "--version")) {
				version();
                return 0;
			} else {
                continue;
			}
		}
    	if(dot = is_extension_c_cpp(argv[i])) {
        	has_file = true;
            fi = i;
            if(is_extension(argv[i], ".c")) c_lang = true;
            break;
        }
	}
    if(!has_file) {
        fputs("execpp: No input file or invalid file format\n", stderr);
        puts("Use \"execpp --help\" for more information.");
        return -1;
    }

	// check arguments...

	bool compile = false, run = false, compile_run = false;
	int ci = -1, ri = -1, cri = -1;
	if(argc == 2) { // when has_file
		compile = run = true;
		goto ARGCEQ2;
	}
	for (int i = 1; i < argc; i++) {
		if(i == fi) continue;
		if(!strcmp(argv[i], "--compile") || !strcmp(argv[i], "-c")) {
			ci = i;
			compile = true;
		} else if(!strcmp(argv[i], "--run") || !strcmp(argv[i], "-r")) {
			ri = i;
			run = true;
		} else if(!strcmp(argv[i], "--compile-run") || !strcmp(argv[i], "-cr") || !strcmp(argv[i], "-rc")) {
			cri = i;
			compile_run = true;
		} else {
			fprintf(stderr, "execpp: Invalid argument: %s\n", argv[i]);
        	puts("Use \"execpp --help\" for more information.");
			return -1;
		}
	}
	if(((cri != -1) && (ci != -1)) || ((cri != -1) && (ri != -1))) {
        fputs("execpp: Invalid operation\n", stderr);
        puts("Use \"execpp --help\" for more information.");
		return -1;
	}
	else if(compile_run) compile = run = true;
ARGCEQ2:

    const char *_custom_compiler = (c_lang ? _CUSTOM_COMPILER_GCC : _CUSTOM_COMPILER);
    const char *_custom_args = (c_lang ? _CUSTOM_ARGS_GCC : _CUSTOM_ARGS);

    char *FileCpp = (char *)malloc(strlen(argv[fi]) + 1);
    strcpy(FileCpp, argv[fi]);
    convert_to_real_path(FileCpp);
    dot = is_extension_c_cpp(FileCpp);

    size_t fileLen = strlen(FileCpp);
    size_t cmdLen = strlen(_custom_compiler) + strlen(_custom_args) + (fileLen << 1) + 50;

    char *FilenameNoExt = (char *)malloc(fileLen);

    strncpy(FilenameNoExt, FileCpp, dot - FileCpp);
    FilenameNoExt[dot - FileCpp] = '\0';

    if(run && !compile) {
		goto ONLYRUN;
	}
	
	// compile...

    const char *Prog = _custom_compiler;
    const char *Args = _custom_args;
    char *Exec = (char *)malloc(cmdLen);

    snprintf(Exec, cmdLen, "%s %s \"%s\" -o \"%s", Prog, Args, FileCpp, FilenameNoExt);
    //printf("%s %s \"%s\" -o \"%s", Prog, Args, argv[fi], FilenameNoExt);
    //return 0;

#ifdef _WIN32
    system(strcat(Exec, ".exe\""));
    if(compile && !run) {
        goto END_ONLYCOMPILE;
    }

    // run...
ONLYRUN:

    strfcat(FilenameNoExt, "\"");
    strcat(FilenameNoExt, "\"");
    system(FilenameNoExt);

#else
    system(strcat(Exec, "\""));
    if(compile && !run) {
        goto ONLYCOMPILE;
    }
    // run...
ONLYRUN:

    strfcat(FilenameNoExt, "\'");
    strcat(FilenameNoExt, "\'");
    system(FilenameNoExt);

#endif // _WIN32

END_ONLYCOMPILE:

    if(compile) free(Exec);
    free(FileCpp);
    free(FilenameNoExt);

    return 0;
}


void help() {
    puts("Usage: execpp [OPTION]... <FILE>");
    puts("Compile and excute a single cpp file.");
    puts("");
    puts(
        "Options:\n"
        "  -cr, --compile-run        Compile and run specified cpp file [by default]\n"
        "  -c,  --compile            Compile specified cpp file\n"
        "  -r,  --run                Run specified cpp file, with executable file compiled\n"
        "       --help               Display this help and exit\n"
        "       --version            Show version number and exit\n"
    );
}

void version() {
    puts("execpp vesion 2.3.0");
    puts("Copyright (c) 2026 EndDavid");
    puts("Licence: https://mit-license.org/");
}