#define main(a, b) main_utf8(a, b)
#include "../../lua.c"
#include "unicode.h"

int wmain(int argc, wchar_t **wargv) {
	char **argv = calloc(argc, sizeof(char*));
	for (int i = 0; i < argc; ++i) {
		argv[i] = w2u(wargv[i]);
	}

	int ret = main_utf8(argc, argv);

	for (int i = 0; i < argc; ++i) {
		free(argv[i]);
	}
	free(argv);
	return ret;
}
