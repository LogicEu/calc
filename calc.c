#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float operation(char* op, float a, float b)
{
	float n = 0.;
	if (!strcmp(op, "+")) n = a + b;
	else if (!strcmp(op, "-")) n = a - b;
	else if (!strcmp(op, ".") || !strcmp(op, "*")) n = a * b;
	else if (!strcmp(op, "/")) n = a / b;
	else printf("'%s' is not a valid operation.\n", op);
	return n;
}

int main(int argc, char* argv[])
{
	float num;
	if (argc == 1) {
		printf("\n");
		printf("Use 'calc -help' to read instructions.\n");
	}
	
    else if (argc == 2) {
		if (!strcmp(argv[1], "-help")) {
			printf("\n");
			printf("\n");
			printf("-------------TERMINAL CALCULATOR ------------\n");	
			printf("Call the calculator using 'calc a symbol b',\n");
			printf("where a and b are the numbers to be entered\n");
			printf("and the symbol is the operation to perform.\n");
			printf("Example: 'calc 5 . 2'\n");
			printf("'10.000'\n");
			printf("                                v0.0.1 - Beta\n");
		}
		else printf("%s\n", argv[1]);
	} else {
		if (argc % 2) printf("Please enter a valid operation\n");
		else {
			num = atof(argv[1]);
			for (int i = 1; i < argc; i ++) {
				if (i % 2 == 0)	{
					num = operation(argv[i], num, atof(argv[i + 1]));
				}
			}
			printf("%.3f\n", num);
		}
	}
	return 0;
}
