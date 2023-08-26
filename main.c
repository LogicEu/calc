#define NULL ((void*)0)
#define isbetween(c, min, max) ((c >= min) && (c <= max))
#define isdigit(c) ((c >= '0') && (c <= '9'))
#define tokget(cmp, tok, i) do { tok[++i] = str[; } while (cmp); ++i;

void exit(int);
extern int printf(const char* fmt, ...);
extern long strtol(const char* str, char** endptr, int base);
extern char* strcpy(char* dst, const char* src);

static char* lex(const char* str, long* iter)
{
    static char token[0xff];
    long i = *iter, j = 0;
    switch(str[i]) {
        case 0: 
            return NULL;
        case ' ':
        case '\n':
        case '\t':
        case '\r':
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
        default:
            token[j++] = str[i];
            if ((str[i] == '>' || str[i] == '<' || str[i] == '&' || str[i] == '&') &&
                 str[i] == str[i + 1]) {
                token[j++] = str[i + 1];
            }
            break;
    }
    token[j] = 0;
    *iter += j;
    return token;
}

static int oppres(const char* op)
{
    switch (op[0]) {
        case '!': return 3;
        case '*':
        case '/':
        case '%': return 5;
        case '+':
        case '-': return 6;
        case '<':
        case '>': return 9 - 2 * (op[1] == op[0]);
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
        case '>': return p[1] == '>' ? l >> r : l > r;
        case '<': return p[1] == '<' ? l << r : l < r;
        case '&': return p[1] == '&' ? l && r : l & r;
        case '|': return p[1] == '|' ? l || r : l | r;
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

static long parse(const char* str)
{
    int accepts = 1, parens[0xff] = {0}, parencount = 1;
    long out[0xff], outcount = 0, stackcount = 0, unarycount = 0, i = 0, n;
    char* tok = lex(str, &i), unary[0xff], stack[0xff][4];
    while (tok) {
        switch (*tok) {
            case 0:
                break;
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '$': case '#': case '?': case '\'': case '"':
            case '{': case '}': case '[': case ']': case '^': case '@':
                printf("calc: error parsing invalid simbol: %s\n", tok);
                exit(1);
            case '0':
                if (tok[1] == 'x' || tok[1] == 'X') {
                    out[outcount++] = strtol(tok + 2, NULL, 16);
                } else {
                    out[outcount++] = strtol(tok + 1, NULL, 10);
                }
                accepts = 0;
                break;
            case '1' ... '9':
                out[outcount++] = strtol(tok, NULL, 10);
                accepts = 0;
                break;
            case '(':
                strcpy(stack[stackcount++], tok);
                parens[parencount++] = unarycount;
                accepts = 1;
                break;
            case ')':
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
                accepts = 0;
                break;
            case '!': case '~':
                unary[unarycount++] = *tok;
                break;
            case '-': case '+':
                if (accepts) {
                    unary[unarycount++] = *tok;
                    break;
                }
            default:
                while (unarycount > parens[parencount - 1]) {
                    out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
                }

                while (stackcount && *stack[stackcount - 1] != '(' &&
                    oppres(tok) >= oppres(stack[stackcount - 1])) {
                    n = out[--outcount];
                    out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
                }
                strcpy(stack[stackcount++], tok);
                accepts = 1;
        }
        tok = lex(str, &i);
    }

    while (unarycount) {
        out[outcount - 1] = uop(out[outcount - 1], unary[--unarycount]);
    }

    while (stackcount) {
        n = out[--outcount];
        out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
    }

    return out[0];
}

int main(const int argc, const char** argv)
{
    if (argc < 2) {
        printf("Usage:\n$ calc '1 + (2 - 3) * 4 / 5'\n");
        return 1;
    }
    printf("%ld\n", parse(argv[1]));
    return 0;
}
