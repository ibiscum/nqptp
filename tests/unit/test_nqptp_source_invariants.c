#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

static char *read_text_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (f == NULL)
    return NULL;
  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }
  long size = ftell(f);
  if (size < 0) {
    fclose(f);
    return NULL;
  }
  if (fseek(f, 0, SEEK_SET) != 0) {
    fclose(f);
    return NULL;
  }
  char *buf = malloc((size_t)size + 1);
  if (buf == NULL) {
    fclose(f);
    return NULL;
  }
  size_t n = fread(buf, 1, (size_t)size, f);
  fclose(f);
  if (n != (size_t)size) {
    free(buf);
    return NULL;
  }
  buf[n] = '\0';
  return buf;
}

static void test_nqptp_source_invariants(__attribute__((unused)) void **state) {
  const char *path = TOP_SRCDIR "/src/nqptp.c";
  char *text = read_text_file(path);
  assert_non_null(text);

  assert_null(strstr(text, "int epoll_fd;"));
  assert_non_null(strstr(text, "// try to set a real time scheduling policy with a priority of 5"));
  assert_non_null(strstr(text, "param.sched_priority = 5;"));
  assert_non_null(strstr(text, "if (msg == NULL) {"));
  assert_non_null(strstr(text, "warn(\"Could not allocate %zu bytes for announce message.\", msg_length);"));
  assert_non_null(strstr(text, "hints.ai_family = ip_family;"));
  assert_non_null(strstr(text, "hints.ai_flags = 0;"));
  assert_null(strstr(text, "hints.ai_family = AF_UNSPEC;"));

  free(text);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_nqptp_source_invariants),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
