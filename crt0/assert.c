#include <assert.h>
#include <errno.h>

int my_errno=0;

extern int *__errno _PARAMS ((void))
{
	return &my_errno;
}

int atoi(const char *s)
{
  int neg = 0, ret = 1;

  for (;ret;++s) {
    switch(*s) {
      case ' ':
      case '\t':
        continue;
      case '-':
        ++neg;
      case '+':
        ++s;
      default:
        ret = 0;
        break;
    }
  }
  /* calculate the integer value. */
  for (; ((*s >= '0') && (*s <= '9')); ) ret = ((ret * 10) + (int)(*s++ - '0'))
;
  return ((neg == 0) ? ret : -ret);
}

void __assert(const char * s, int n, const char *s1)
{
}
