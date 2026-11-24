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

static void test_nqptp_logging_config_source_invariants(__attribute__((unused)) void **state) {
  char *configure = read_text_file(TOP_SRCDIR "/configure.ac");
  char *debug_c = read_text_file(TOP_SRCDIR "/src/debug.c");
  char *makefile_top = read_text_file(TOP_SRCDIR "/Makefile.am");
  char *makefile_src = read_text_file(TOP_SRCDIR "/src/Makefile.am");

  assert_non_null(configure);
  assert_non_null(debug_c);
  assert_non_null(makefile_top);
  assert_non_null(makefile_src);

  assert_non_null(strstr(configure, "AC_ARG_WITH([journald-logging]"));
  assert_non_null(strstr(configure, "CONFIG_USE_JOURNALD_LOGGING"));
  assert_non_null(strstr(configure, "SYSTEMD_CFLAGS"));
  assert_non_null(strstr(configure, "SYSTEMD_LIBS"));

  assert_non_null(strstr(makefile_top, "SUBDIRS = src tests"));
  assert_non_null(strstr(makefile_src, "AM_CPPFLAGS = -I$(top_srcdir)/include $(SYSTEMD_CFLAGS)"));
  assert_non_null(strstr(makefile_src, "LDADD = $(SYSTEMD_LIBS)"));

  assert_non_null(strstr(debug_c, "#ifdef CONFIG_USE_JOURNALD_LOGGING"));
  assert_non_null(strstr(debug_c, "sd_journal_send("));
  assert_non_null(strstr(debug_c, "fprintf(stderr, \"%s\\n\", message);"));

  free(configure);
  free(debug_c);
  free(makefile_top);
  free(makefile_src);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_nqptp_logging_config_source_invariants),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
