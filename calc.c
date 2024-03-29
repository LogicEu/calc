/* Copyright (c) 2023 Eugenio Arteaga A.

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

********************** calc.c ************************ */

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define NULL ((void*)0)
#define isbetween(c, min, max) ((c >= min) && (c <= max))
#define isdigit(c) isbetween(c, '0', '9')

extern int printf(const char*, ...);
extern long strtol(const char*, char**, int);

static void tokcpy(char* dst, const char* src)
{
    while (*src) {
        *dst++ = *src++;
    }
    *dst = 0;
}

static char* lex(const char* str, long* iter)
{
    static char token[0xff];
    long i = *iter, j = 0, err = 0;
    switch (str[i]) {
        case 0:
            return NULL;
        case ' ': case '\n': case '\t': case '\r':
            ++*iter;
            return lex(str, iter);
        case '0':
            if (str[i + 1] == 'x' || str[i + 1] == 'X') {
                token[j++] = str[i];
                token[j++] = str[i + 1];
            }
        case '1' ... '9':
            while (isdigit(str[i + j]) || ((str[i + 1] == 'x' || str[i + 1] == 'X') && 
                (isbetween(str[i + j], 'a', 'f') || isbetween(str[i + j], 'A', 'F')))) {
                token[j] = str[i + j];
                ++j;
            }
            break;
        case '(':
        case ')':
            token[j++] = str[i];
            break;
        case '=':
            token[j++] = str[i];
            if (str[i + 1] == '=') {
                token[j++] = str[i + 1];
            } else {
                ++err;
            }
            break;
        case '\'': case '"':
        case '$': case '#': case '?': case ';': case ':': case '\\': case '.':
        case '{': case '}': case '[': case ']': case '_': case '@': case ',':
            token[j++] = str[i];
            ++err;
            break;
        default:
            token[j++] = str[i];
            if ((str[i] == '!' && str[i + 1] == '=') ||
                ((str[i] == '>' || str[i] == '<') &&
                (str[i + 1] == '=' || str[i + 1] == str[i])) ||
                ((str[i] == '&' || str[i] == '|') && (str[i + 1] == str[i]))) {
                token[j++] = str[i + 1];
            }
    }
    
    token[j] = 0;
    *iter += j;
    if (err) {
        printf("calc: operator '%s' is not supported\n", token);
        token[0] = '?';
    }
    return token;
}

static int oppres(const char* op)
{
    switch (op[0]) {
        case '!': return 3 + 7 * (op[1] == '=');
        case '*':
        case '/':
        case '%': return 5;
        case '+':
        case '-': return 6;
        case '<':
        case '>': return 9 - 2 * (op[1] == op[0]);
        case '=': return 10;
        case '^': return 12;
        case '&': return 11 + 3 * (op[1] == op[0]);
        case '|': return 13 + 2 * (op[1] == op[0]);
    }
    return 16;
}

static long op(const long l, const long r, const char* op)
{
    switch (*op) {
        case '+': return l + r;
        case '-': return l - r;
        case '/': return l / r;
        case '*': return l * r;
        case '%': return l % r;
        case '^': return l ^ r;
        case '=': return l == r; 
        case '&': return op[1] == '&' ? l && r : l & r;
        case '|': return op[1] == '|' ? l || r : l | r;
        case '>': return op[1] == '>' ? l >> r : op[1] == '=' ? l >= r : l > r;
        case '<': return op[1] == '<' ? l << r : op[1] == '=' ? l <= r : l < r;
    }
    return 0;
}

static long uop(const long l, const char op)
{
    switch (op) {
        case '!': return !l;
        case '+': return +l;
        case '-': return -l;
        case '~': return ~l;
    }
    return 0;
}

static long eval(const char* str, long* output)
{
    int parens[0xff] = {0}, parencount = 1, expecting;
    long operands[0xff] = {0}, count = 0, opcount = 0, uopcount = 0, index = 0, n;
    char ops[0xff][4], uops[0xff], *tok = lex(str, &index);
    expecting = !!tok;
    while (tok) {
        switch (*tok) {
            case 'a' ... 'z':
            case 'A' ... 'Z':
                printf("calc: invalid symbol in expression: %s\n", tok);
                /* fallthrough */
            case '?':
                return EXIT_FAILURE;
            case '0':
                if (tok[1] == 'x' || tok[1] == 'X') {
                    operands[count++] = strtol(tok + 2, NULL, 16);
                } else {
                    operands[count++] = strtol(tok + 1, NULL, 8);
                }
                expecting = 0;
                break;
            case '1' ... '9':
                operands[count++] = strtol(tok, NULL, 10);
                expecting = 0;
                break;
            case '(':
                tokcpy(ops[opcount++], tok);
                parens[parencount++] = uopcount;
                expecting = 1;
                break;
            case ')':
                if (expecting || parencount <= 1) {
                    printf("calc: invalid token when expecing value: %s\n", tok);
                    return EXIT_FAILURE;
                }

                while (uopcount > parens[parencount - 1]) {
                    operands[count - 1] = uop(operands[count - 1], uops[--uopcount]);
                }

                while (opcount && *ops[opcount - 1] != '(') {
                    n = operands[--count];
                    operands[count - 1] = op(operands[count - 1], n, ops[--opcount]);
                }

                --parencount;
                while (uopcount > parens[parencount - 1]) {
                    operands[count - 1] = uop(operands[count - 1], uops[--uopcount]);
                }

                opcount = opcount ? opcount - 1 : opcount;
                expecting = 0;
                break;
            case '!': case '~':
                uops[uopcount++] = *tok;
                expecting = 1;
                break;
            case '-': case '+':
                if (expecting) {
                    uops[uopcount++] = *tok;
                    break;
                } /* fallthrough */
            default:
                if (expecting) {
                    printf("calc: invalid token when expecting value: %s\n", tok);
                    return EXIT_FAILURE;
                }

                while (uopcount > parens[parencount - 1]) {
                    operands[count - 1] = uop(operands[count - 1], uops[--uopcount]);
                }

                while (opcount && *ops[opcount - 1] != '(' &&
                    oppres(tok) >= oppres(ops[opcount - 1])) {
                    n = operands[--count];
                    operands[count - 1] = op(operands[count - 1], n, ops[--opcount]);
                }
                tokcpy(ops[opcount++], tok);
                expecting = 1;
        }

        if (count >= 0xff || opcount >= 0xff || uopcount >= 0xff) {
            printf("calc: expression too long to parse: %s\n", str);
            return EXIT_FAILURE;
        }
        tok = lex(str, &index);
    }

    if (expecting || parencount > 1) {
        printf("calc: invalid unfinished expression: '%s'\n", str);
        return EXIT_FAILURE;
    }

    while (uopcount) {
        operands[count - 1] = uop(operands[count - 1], uops[--uopcount]);
    }

    while (opcount) {
        n = operands[--count];
        operands[count - 1] = op(operands[count - 1], n, ops[--opcount]);
    }

    *output = operands[0];
    return EXIT_SUCCESS;
}

static int calc_usage(int status)
{
    printf(
        "usage:\ncalc '1 + (2 - 3) * 4 / 5'\n"
        "<expr>\t: evaluate <expr> and print the result to stdout\n"
        "-d\t: print output in decimal base (default)\n"
        "-o\t: print output in octal base with a leading zero\n"
        "-x\t: print output in hexadecimal base using lower case format\n"
        "-X\t: print output in hexadecimal base using upper case format\n"
        "-h\t: print this help message\n"
    );
    return status;
}

int main(const int argc, const char** argv)
{
    long i, output, status;
    char fmt[8] = "%ld\n";
    if (argc < 2) {
        return calc_usage(EXIT_FAILURE);
    }

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][2] == 0) {
            const char c = argv[i][1];
            if (c == 'h') {
                return calc_usage(EXIT_SUCCESS);
            } else if (c == 'o' || c == 'x' || c == 'X' || c == 'd') {
                fmt[2] = c;
                continue;
            }
        }

        if (!(status = eval(argv[i], &output))) {
            if (fmt[2] == 'o' || fmt[2] == 'x' || fmt[2] == 'X') {
                printf(fmt[2] == 'o' ? "0" : "0x");
            }
            printf(fmt, output);
        }
    }
    return status;
}

