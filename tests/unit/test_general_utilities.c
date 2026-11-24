#include <arpa/inet.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

#include "general-utilities.h"

static void test_nctohs_reads_network_u16(__attribute__((unused)) void **state) {
  uint8_t bytes[2] = {0x12, 0x34};
  assert_int_equal(nctohs(bytes), 0x1234);
}

static void test_nctohl_reads_network_u32(__attribute__((unused)) void **state) {
  uint8_t bytes[4] = {0x12, 0x34, 0x56, 0x78};
  assert_int_equal(nctohl(bytes), 0x12345678U);
}

static void test_nctoh64_reads_network_u64(__attribute__((unused)) void **state) {
  uint8_t bytes[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
  assert_true(nctoh64(bytes) == UINT64_C(0x0123456789ABCDEF));
}

static void test_hcton64_round_trip(__attribute__((unused)) void **state) {
  uint8_t bytes[8] = {0};
  uint64_t value = UINT64_C(0xA1B2C3D4E5F60718);
  hcton64(value, bytes);
  assert_true(nctoh64(bytes) == value);
}

static void test_ntoh64_matches_halves(__attribute__((unused)) void **state) {
  uint64_t host = UINT64_C(0xCAFEBABE11223344);
  uint32_t hi = htonl((uint32_t)(host >> 32));
  uint32_t lo = htonl((uint32_t)(host & UINT32_C(0xffffffff)));
  uint64_t packed = ((uint64_t)lo << 32) | hi;
  assert_true(ntoh64(packed) == host);
}

static void test_get_time_now_is_nonzero(__attribute__((unused)) void **state) {
  uint64_t now = get_time_now();
  assert_true(now > 0);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_nctohs_reads_network_u16),
      cmocka_unit_test(test_nctohl_reads_network_u32),
      cmocka_unit_test(test_nctoh64_reads_network_u64),
      cmocka_unit_test(test_hcton64_round_trip),
      cmocka_unit_test(test_ntoh64_matches_halves),
      cmocka_unit_test(test_get_time_now_is_nonzero),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
