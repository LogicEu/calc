/*

Copyright (c) 2023 Eugenio Arteaga A.

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

*/

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
    switch(str[i]) {
        case 0:
            return NULL;
        case ' ': case '\n': case '\t': case '\r':
            ++(*iter);
            return lex(str, iter);
        case '0':
            if (str[i + 1] == 'x' || str[i + 1] == 'X') {
                token[j++] = str[i];
                token[j++] = str[i + 1];
            }
        case '1' ... '9':
            while (isdigit(str[i + j]) || isbetween(str[i + j], 'a', 'f') ||
                isbetween(str[i + j], 'A', 'F')) {
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
        token[1] = 0;
        return token;
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

static long op(const long l, const long r, const char* p)
{
    switch (*p) {
        case '+': return l + r;
        case '-': return l - r;
        case '/': return l / r;
        case '*': return l * r;
        case '%': return l % r;
        case '^': return l ^ r;
        case '=': return l == r; 
        case '&': return p[1] == '&' ? l && r : l & r;
        case '|': return p[1] == '|' ? l || r : l | r;
        case '>': return p[1] == '>' ? l >> r : p[1] == '=' ? l >= r : l > r;
        case '<': return p[1] == '<' ? l << r : p[1] == '=' ? l <= r : l < r;
    }
    return 0;
}

static long uop(const long l, const char p)
{
    switch (p) {
        case '!': return !l;
        case '+': return +l;
        case '-': return -l;
        case '~': return ~l;
    }
    return 0;
}

static long parse(const char* str, long* output)
{
    int expecting, parens[0xff] = {0}, parencount = 1;
    long out[0xff] = {0}, outcount = 0, stackcount = 0, unarycount = 0, i = 0, n;
    char unary[0xff], stack[0xff][4], *tok = lex(str, &i);
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
                    out[outcount++] = strtol(tok + 2, NULL, 16);
                } else {
                    out[outcount++] = strtol(tok + 1, NULL, 8);
                }
                expecting = 0;
                break;
            case '1' ... '9':
                out[outcount++] = strtol(tok, NULL, 10);
                expecting = 0;
                break;
            case '(':
                tokcpy(stack[stackcount++], tok);
                parens[parencount++] = unarycount;
                expecting = 1;
                break;
            case ')':
                if (expecting || parencount <= 1) {
                    printf("calc: invalid token when expecing value: %s\n", tok);
                    return EXIT_FAILURE;
                }

                while (unarycount > parens[parencount - 1]) {
                    out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
                }

                while (stackcount && *stack[stackcount - 1] != '(') {
                    n = out[--outcount];
                    out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
                }

                --parencount;
                while (unarycount > parens[parencount - 1]) {
                    out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
                }

                stackcount = stackcount ? stackcount - 1: stackcount;
                expecting = 0;
                break;
            case '!': case '~':
                unary[unarycount++] = *tok;
                expecting = 1;
                break;
            case '-': case '+':
                if (expecting) {
                    unary[unarycount++] = *tok;
                    break;
                }
                /* fallthrough */
            default:
                if (expecting) {
                    printf("calc: invalid token when expecting value: %s\n", tok);
                    return EXIT_FAILURE;
                }

                while (unarycount > parens[parencount - 1]) {
                    out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
                }

                while (stackcount && *stack[stackcount - 1] != '(' &&
                    oppres(tok) >= oppres(stack[stackcount - 1])) {
                    n = out[--outcount];
                    out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
                }
                tokcpy(stack[stackcount++], tok);
                expecting = 1;
        }
        tok = lex(str, &i);
    }

    if (expecting || parencount > 1) {
        printf("calc: invalid unfinished expression: '%s'\n", str);
        return EXIT_FAILURE;
    }

    while (unarycount) {
        out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
    }

    while (stackcount) {
        n = out[--outcount];
        out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
    }

    *output = out[0];
    return EXIT_SUCCESS;
}

int main(const int argc, const char** argv)
{
    int i;
    long output;

    if (argc < 2) {
        printf("usage:\ncalc '1 + (2 - 3) * 4 / 5'\n");
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; ++i) {
        if (parse(argv[i], &output)) {
            return EXIT_FAILURE;
        }
        printf("%ld\n", output);
    }

    return EXIT_SUCCESS;
}

