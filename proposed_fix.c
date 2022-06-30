#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int main(int argc, char** argv) {
  char *to, *from, **av;
  size_t size;
  char* user_args;

  for (size = 0, av = argv + 1; *av; av++)
      size += strlen(*av) + 1;

  if (size == 0 || (user_args = malloc(size)) == NULL) {
    puts("Heap memory allocation failed, uh oh");
    return 1;
  }

  if (true) {
    for (to = user_args, av = argv + 1; (from = *av); av++) {
        while (*from) {
            if (from[0] == '\\' && from[1] != '\0' && !isspace((unsigned char)from[1]))
                from++;
            *to++ = *from++;
        }
        *to++ = ' ';
    }
    *--to = '\0';
  }

  puts("Wow tudo aconteceu tão bem haha que código bem feito");
  puts(user_args);

  free(user_args);

  return 0;
}
