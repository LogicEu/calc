#define NULL ((void*)0)
#define isdigit(c) (c >= '0') && (c <= '9')
#define tokget(cmp, tok, i) do { tok[++i] = str[; } while (cmp); ++i;

extern int printf(const char* fmt, ...);
extern long atol(const char* str);
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
        case '0' ... '9':
            while (isdigit(str[i + j])) {
                token[j] = str[i + j];
                ++j;
            }
            break;
        default:
            token[j++] = str[i];
            token[j] = str[i] == str[i + 1] ? str[i + 1] : 0;
            j += !!token[j];
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
    switch(*p) {
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

static long parse(const char* str)
{
    long out[0xff], outcount = 0, stackcount = 0, i = 0, n;
    char* tok = lex(str, &i), stack[0xff][4];
    while (tok) {
        switch (*tok) {
            case 0:
                break;
            case '0' ... '9':
                out[outcount++] = atol(tok);
                break;
            case '(':
                strcpy(stack[stackcount++], tok);
                break;
            case ')':
                while (stackcount && *stack[stackcount - 1] != '(') {
                    n = out[--outcount];
                    out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
                }
                stackcount = stackcount ? stackcount - 1: stackcount;
                break;
            default:
                while (stackcount && oppres(tok) >= oppres(stack[stackcount - 1])) {
                    if (*stack[stackcount - 1] == '(') {
                        --stackcount;
                        break;
                    }
                    n = out[--outcount];
                    out[outcount - 1] = op(out[outcount - 1], n, stack[--stackcount]);
                }
                strcpy(stack[stackcount++], tok);
        }
        tok = lex(str, &i);
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
