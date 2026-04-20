#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

//#define aprintf(dest, ...) snprintf(dest + strlen(dest), sizeof(dest) - strlen(dest), __VA_ARGS__) // deprecated...

#define _CUSTOM_COMPILER "g++"
#define _CUSTOM_ARGS "-Wall -Wextra -std=c++23"

void help();
void version();

char* is_extension(const char *filename, const char *extension) {
    char* dot = strrchr(filename, '.');
    if ((dot == NULL) || strcmp(dot, extension)) {
        return NULL;
    }
    return dot;
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

	bool has_file = false;
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
        char *extension[] = {".cpp", ".cc", ".cxx"};
   		for (int j = 0; j < 3; j++) {
    		if((dot = is_extension(argv[i], extension[j])) != NULL) {
            	has_file = true;
                fi = i;
				break;
        	}
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
	if(argc == 2) { // when has file
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
	if((cri != -1) && (ci != -1) || (cri != -1) && (ri != -1)) {
        fputs("execpp: Invalid operation\n", stderr);
        puts("Use \"execpp --help\" for more information.");
		return -1;
	}
	else if(compile_run) compile = run = true;
ARGCEQ2:

	size_t fileLen = strlen(argv[fi]);
    size_t cmdLen = strlen(_CUSTOM_COMPILER) + strlen(_CUSTOM_ARGS) + (fileLen << 1) + 50;
    char *FilenameNoExt = (char *)malloc(fileLen + 1);

    strncpy(FilenameNoExt, argv[fi], dot - argv[fi]);
    FilenameNoExt[dot - argv[fi]] = '\0';
	if(run && !compile) {
		goto ONLYRUN;
	}
	
	// compile...

    const char Prog[] = _CUSTOM_COMPILER;
    const char Args[] = _CUSTOM_ARGS;
    char *Exec = (char *)malloc(cmdLen);

    snprintf(Exec, cmdLen, "%s %s %s -o \"%s", Prog, Args, argv[fi], FilenameNoExt);

#ifdef _WIN32
    system(strcat(Exec, ".exe\""));
    if(compile && !run) {
        free(Exec);
        return 0;
    }
    /*
    char *FilenameExe = (char *)malloc(fileLen + 5);
    strcpy(FilenameExe, FilenameNoExt);
    strcat(FilenameExe, ".exe");
    */

    // run...
ONLYRUN:
    char *AbsPath = (char *)malloc(fileLen + 10);
    strcpy(AbsPath, ".\\");
    system(strcat(AbsPath, FilenameNoExt));

    //free(FilenameExe);
    free(AbsPath);
#else
    system(strcat(Exec, "\""));
    if(compile && !run) {
        free(Exec);
        return 0;
    }
    // run...
ONLYRUN:
    system(FilenameNoExt);
#endif // _WIN32

    if(compile) free(Exec);
    if(run) free(FilenameNoExt);
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
    puts("execpp vesion 2.0.1");
    puts("Copyright (c) 2026 EndDavid");
    puts("Licence: https://mit-license.org/");
}