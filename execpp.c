// Copyright (c) 2026 EnderDavid
// Licence: https://mit-license.org/

#include "execpp_utils.h"

void help();
void version();
void configurate();
void display_config();

int main(int argc, char *argv[]) {
	//init...

    switch(verify_integrity()) {
    case 0:
        break;
    case -1:
        return -1;
    case 1:
        configurate();
        return 0;
    default:
        return -1;
    }

    if(argc == 1) {
        puts("execpp: No input file");
        help();
        return 0;
    }
    if(argc > 5) {
        fputs("execpp: Too many arguments\n", stderr);
        puts("See \"execpp --help\" for more information.");
        return -1;
    }

	// check file & help or version arg...

	bool has_file = false, c_lang = false;
	char* dot;
	int fi = -1;
	for (int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "--help")) {
			help();
			return 0;
		} else if(!strcmp(argv[i], "--version")) {
			version();
            return 0;
        } else if(!strcmp(argv[i], "--configure")) {
            configurate();
            return 0;
		} else if(!strcmp(argv[i], "--display-config")) {
			display_config();
			return 0;
		}
    	if((dot = is_extension_c_cpp(argv[i]))) {
        	has_file = true;
            fi = i;
            if(is_extension(argv[i], ".c")) c_lang = true;
            // break;
        }
	}
    if(!has_file) {
        fputs("execpp: No input file or invalid file format\n", stderr);
        puts("See \"execpp --help\" for more information.");
        return -1;
    }

    if(!strcmp(argv[fi], "execpp.c") || !strcmp(argv[fi], "execpp.cpp") ||
        strstr(argv[fi], "\\execpp.c") || strstr(argv[fi], "\\execpp.cpp") ||
        strstr(argv[fi], "/execpp.c") || strstr(argv[fi], "/execpp.cpp")
    ) {
        printf("execpp: Recursive compilation detected!\n"
               "I'm not smart enough to rebuild myself... yet.\n"
        );
        return 114;
    }

	// check arguments...

	bool compile = false, run = false, compile_run = false, explicit_display = false;
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
        } else if(!strcmp(argv[i], "--explicit") || !strcmp(argv[i], "-e")) {
            explicit_display = true;
        } else {
			fprintf(stderr, "execpp: Invalid argument: %s\n", argv[i]);
        	puts("See \"execpp --help\" for more information.");
			return -1;
		}
	}
	if(((cri != -1) && (ci != -1)) || ((cri != -1) && (ri != -1))) {
        fputs("execpp: Invalid operation\n", stderr);
        puts("See \"execpp --help\" for more information.");
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

    snprintf(Exec, cmdLen, "%s %s \"%s\" -o \"%s\"", Prog, Args, FileCpp, FilenameNoExt);
    if(explicit_display) {
        printf("executing: %s\n", Exec);
    }

    ExecuteCmd(Exec);

    if(compile && !run) {
        goto END_ONLYCOMPILE;
    }

    // run...
ONLYRUN:
    if(strchr(FilenameNoExt, ' ')) quote(FilenameNoExt);
    if(explicit_display) {
        printf("executing: %s\n", FilenameNoExt);
    }
    system(FilenameNoExt);

END_ONLYCOMPILE:

    if(compile) free(Exec);
    free(FileCpp);
    free(FilenameNoExt);

	free(_CUSTOM_COMPILER);
	free(_CUSTOM_ARGS);
	free(_CUSTOM_COMPILER_GCC);
	free(_CUSTOM_ARGS_GCC);

	return 0;
}

void configurate() {
    puts("Please configurate your compiler & compilation parameters...");
    puts("(Ctrl + C to cancel)");

#define _gpp _CUSTOM_COMPILER
#define _gcc _CUSTOM_COMPILER_GCC
#define _arg_gpp _CUSTOM_ARGS
#define _arg_gcc _CUSTOM_ARGS_GCC

_GPP_SET:
    printf("G++ Compiler: ");
    _gpp = fgetl(stdin);
    if(!check_compiler(_gpp)) {
        convert_to_real_path(_gpp);
        if(!check_compiler(_gpp)) {
            printf("Invalid compiler \"%s\"\n", _gpp);
            goto _GPP_SET;
        }
    }
#ifdef _WIN32
    if(strchr(_gpp, '\\') || strchr(_gpp, '/')) {
		convert_to_real_path(_gpp);
		if(strchr(_gpp, ' ')) quote(_gpp);
	}
#else
	if(strchr(_gpp, '/')) {
		convert_to_real_path(_gpp);
		if(strchr(_gpp, ' ')) quote(_gpp);
	}
#endif // _WIN32

_GCC_SET:
    printf("GCC Compiler: ");
    _gcc = fgetl(stdin);
    if(!check_compiler(_gcc)) {
        convert_to_real_path(_gcc);
        if(!check_compiler(_gcc)) {
            printf("Invalid compiler \"%s\"\n", _gcc);
            goto _GCC_SET;
        }
    }
#ifdef _WIN32
    if(strchr(_gcc, '\\') || strchr(_gcc, '/')) {
		convert_to_real_path(_gcc);
		if(strchr(_gcc, ' ')) quote(_gcc);
	}
#else
	if(strchr(_gcc, '/')) {
		convert_to_real_path(_gcc);
		if(strchr(_gcc, ' ')) quote(_gcc);
	}
#endif // _WIN32

_GPP_ARGS:
    printf("G++ Compilation parameters: ");
    _arg_gpp = fgetl(stdin);

// Will not verify parameter legality generally.
// If want, please define VERIFY_PARA_LEGALITY 1 in execpp_utils.h, which is an "experimental feature".
	if(!para_valid_gpp(_arg_gpp)) {
		puts("Invalid G++ Compilation parameters");
		goto _GPP_ARGS;
	}

_GCC_ARGS:
	printf("GCC Compilation parameters: ");
	_arg_gcc = fgetl(stdin);
	if(!para_valid_gcc(_arg_gcc)) {
		puts("Invalid G++ Compilation parameters");
		goto _GCC_ARGS;
	}

	FILE* fcomp = fopen("./.execpp_properties/custom_compiler", "w");
    FILE* farg = fopen("./.execpp_properties/custom_args", "w");
	fprintf(fcomp, "%s\n%s\n", _gpp, _gcc);
	fprintf(farg, "%s\n%s\n", _arg_gpp, _arg_gcc);
	fclose(fcomp);
	fclose(farg);

    FILE *fnote = fopen("./.execpp_properties/note", "w");
    fputs("For each config, C++ in line 1 and C in line 2.\n", fnote);
    fclose(fnote);

	puts("Configuration complete.");
#undef _gpp
#undef _gcc
#undef _arg_gpp
#undef _arg_gcc
}

void display_config() {
#ifdef _WIN32
    puts("G++ & GCC compiler:");
    system("type .\\.execpp_properties\\custom_compiler");
    puts("G++ & GCC compilation parameters:");
    system("type .\\.execpp_properties\\custom_args");
#else
    puts("G++ & GCC compiler:");
    system("cat ./.execpp_properties/custom_compiler");
    puts("G++ & GCC compilation parameters:");
    system("cat ./.execpp_properties/custom_args");
#endif // _WIN32

    /*
	FILE* fcomp = fopen("./.execpp_properties/custom_compiler", "r");
    FILE* farg = fopen("./.execpp_properties/custom_args", "r");
	...
	fclose(fcomp);
	fclose(farg);
    */
}

void help() {
    puts("Usage: execpp [OPTION]... <FILE>");
    puts("Compile and execute a single cpp file.");
    puts("");
    puts(
        "Options:\n"
        "  -cr, --compile-run        Compile and run specified cpp file [by default]\n"
        "  -c,  --compile            Compile specified cpp file\n"
        "  -r,  --run                Run specified cpp file, with executable file compiled\n"
        "  -e,  --explicit           Explicitly display current running command\n"
        "       --configure          Configurate compile options\n"
		"       --display-config     Display current configuration\n"
        "       --help               Display this help and exit\n"
        "       --version            Show version number and exit\n"
    );
}

void version() {
    puts("execpp vesion 3.0.0");
    puts("Copyright (c) 2026 EnderDavid");
    puts("Licence: https://mit-license.org/");
}