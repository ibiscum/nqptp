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

static void test_nqptp_message_handlers_source_invariants(__attribute__((unused)) void **state) {
  char *source = read_text_file(TOP_SRCDIR "/src/nqptp-message-handlers.c");
  assert_non_null(source);

  assert_null(strstr(source, "char hexcharbuffer[16384];"));
  assert_non_null(strstr(source, "static void hex_string("));
  assert_null(strstr(source, "buf + 86"));

  assert_non_null(strstr(source, "else if ((strcmp(command, \"T\") == 0) && (ip_list == NULL))"));
  assert_null(strstr(source, "(command == NULL) || ((strcmp(command, \"T\") == 0)"));

  assert_non_null(strstr(source, "allocated_working_buf = malloc((size_t)recv_len + 1);"));
  assert_non_null(strstr(source, "allocated_working_buf[recv_len] = '\\0';"));

  free(source);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_nqptp_message_handlers_source_invariants),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
