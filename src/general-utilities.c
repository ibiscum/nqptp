/*
 * This file is part of the nqptp distribution (https://github.com/mikebrady/nqptp).
 * Copyright (c) 2021-2026 Mike Brady.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial licensing is also available.
 */

#include "general-utilities.h"
#include "debug.h"
#include <arpa/inet.h> // ntohl etc.
#include <errno.h>
#include <string.h>    // memcpy
#include <sys/stat.h>  // fstat
#include <unistd.h>    // ftruncate

void hcton64(uint64_t num, uint8_t *p) {
  uint64_t numc = num;
  numc = numc >> 32;
  uint32_t num_32 = numc;
  uint32_t rev = htonl(num_32);
  memcpy(p, &rev, sizeof(uint32_t));
  num_32 = num & 0xffffffff;
  p = p + 4;
  rev = htonl(num_32);
  memcpy(p, &rev, sizeof(uint32_t));
}

uint64_t nctoh64(const uint8_t *p) { // read 8 bytes from *p and do ntohl on two halves
  // this is to avoid possible aliasing violations
  uint64_t value = nctohl(p);
  uint64_t value_low = nctohl(p + 4);
  value = value << 32;
  value = value + value_low;
  return value;
}

uint32_t nctohl(const uint8_t *p) { // read 4 bytes from *p and do ntohl on them
  // this is to avoid possible aliasing violations
  uint32_t holder;
  memcpy(&holder, p, sizeof(holder));
  return ntohl(holder);
}

uint16_t nctohs(const uint8_t *p) { // read 2 bytes from *p and do ntohs on them
  // this is to avoid possible aliasing violations
  uint16_t holder;
  memcpy(&holder, p, sizeof(holder));
  return ntohs(holder);
}

uint64_t ntoh64(const uint64_t n) {
  uint64_t fiddle = n;
  uint32_t fiddle_hi = fiddle & 0xFFFFFFFF;
  fiddle_hi = ntohl(fiddle_hi);
  fiddle = fiddle >> 32;
  uint32_t fiddle_lo = fiddle & 0xFFFFFFFF;
  fiddle_lo = ntohl(fiddle_lo);
  fiddle = fiddle_hi;
  fiddle = fiddle << 32;
  fiddle = fiddle | fiddle_lo;
  return fiddle;
}

uint64_t get_time_now() {
  struct timespec tn;
  int rc = -1;
#if defined(CONFIG_FOR_DARWIN)
  rc = clock_gettime(CLOCK_UPTIME_RAW, &tn);
#elif defined(CLOCK_MONOTONIC_RAW)
  rc = clock_gettime(CLOCK_MONOTONIC_RAW, &tn);
#else
  rc = clock_gettime(CLOCK_MONOTONIC, &tn);
#endif
  if (rc != 0) {
    die("clock_gettime failed: %d, \"%s\".", errno, strerror(errno));
  }
  return (uint64_t)tn.tv_sec * 1000000000u + (uint64_t)tn.tv_nsec;
}

int shm_ftruncate(int fd, off_t length) {
  struct stat st;
  if ((fstat(fd, &st) == 0) && (st.st_size >= length))
    return 0;
  return ftruncate(fd, length);
}
