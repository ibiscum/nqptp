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

static void test_nqptp_clock_sources_source_invariants(__attribute__((unused)) void **state) {
  const char *path = TOP_SRCDIR "/src/nqptp-clock-sources.c";
  char *text = read_text_file(path);
  assert_non_null(text);

  assert_non_null(strstr(text, "if (strlen(client_shared_memory_interface_name) >="));
  assert_non_null(strstr(text, "client_shared_memory_interface_name != NULL ? client_shared_memory_interface_name"));
  assert_non_null(strstr(text, "if ((client_id < 0) || (client_id >= MAX_CLIENTS))"));
  assert_non_null(strstr(text, "debug(1, \"cannot getaddrinfo for ip: %s.\", sender_string);"));
  assert_non_null(strstr(text, "// first, see if you can find it anywhere"));

  free(text);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_nqptp_clock_sources_source_invariants),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
