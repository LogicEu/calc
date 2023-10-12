#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_SIZE 32
#define TMP_NAME "tmp.c"
#define EXE_NAME "tmp.out"
#define CFLAGS " -O0 -std=c89 -o "
#define CC "gcc"

static const char fbody[] = "#include <stdio.h>\nint main(void) {\n\tlong n = ";
static const char fclosure[] = ";\n\tprintf(\"%ld\\n\", n);\n}\n";

static int test_pipe(const char* cmd, char* out)
{
    int status = EXIT_SUCCESS;
    FILE* file = popen(cmd, "r");
    if (!file || !fgets(out, CMD_SIZE, file)) {
        fprintf(stderr, "could not pipe command: %s\n", cmd);
        status = EXIT_FAILURE;
    }

    pclose(file);
    out[strlen(out) - 1] = 0;
    return status;
}

static int test_run(char* buf, const char* fmt, const char* expr, char* out)
{
    sprintf(buf, fmt, expr);
    return test_pipe(buf, out);
}

static int test(const char* expr)
{
    char buf[1024], n[64], m[64];
    FILE* file = fopen(TMP_NAME, "wb");
    if (!file) {
        fprintf(stderr, "could not open file: %s\n", TMP_NAME);
        return EXIT_FAILURE;
    }

    fwrite(fbody, sizeof(fbody) - 1, 1, file);
    fwrite(expr, strlen(expr), 1, file);
    fwrite(fclosure, sizeof(fclosure) - 1, 1, file);
    fclose(file);

    if (system(CC CFLAGS EXE_NAME " " TMP_NAME)) {
        fprintf(stderr, "could not compile: %s\n", TMP_NAME);
        return EXIT_FAILURE;
    }

    if (test_run(buf, "./calc '%s'", expr, n) ||
        test_run(buf, "./" EXE_NAME " '%s'", expr, m)) {
        return EXIT_FAILURE;
    } 

    fprintf(
        stdout,
        "'%s' -> %s: (%s == %s)\n", expr, !strcmp(n, m) ? "Passed" : "Failed", n, m
    );

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int i;
    if (argc < 2) {
        fprintf(stderr, "%s: missing argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; ++i) {
        if (test(argv[i])) {
            break;
        }
    }

    remove(EXE_NAME);
    remove(TMP_NAME);
    return i == argc;
}
