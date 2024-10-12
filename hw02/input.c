#include <stdio.h>

main()
{
	char c, s[80];
	int i; long l;
	float f; double d;

	scanf("%c %s %d %ld %f %lf", &c, s, &i, &l, &f, &d);
	printf("Output: %c %s %d %ld %.5f %.2lf\n", c, s, i, l, f, d);

	c = getchar();
	putchar(c);

	gets(s);
	puts(s);
}