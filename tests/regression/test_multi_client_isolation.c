#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <cmocka.h>

#include "nqptp-shm-structures.h"

static const char *kKitchen = "/nqptp-test-kitchen";
static const char *kFoyer = "/nqptp-test-foyer";
static const char *kPeerIP = "192.168.1.209";

static void shm_path(const char *name, char *out, size_t out_len) {
  const char *leaf = name;
  if (leaf[0] == '/')
    leaf++;
  snprintf(out, out_len, "/dev/shm/%s", leaf);
}

static int file_exists(const char *path) {
  return access(path, F_OK) == 0;
}

static void remove_if_exists(const char *name) {
  char path[256];
  shm_path(name, path, sizeof(path));
  if (file_exists(path))
    unlink(path);
}

static void send_command(int sock, const char *message) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(NQPTP_CONTROL_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  size_t len = strlen(message) + 1; // include terminating NUL
  ssize_t sent = sendto(sock, message, len, 0, (struct sockaddr *)&addr, sizeof(addr));
  assert_true(sent == (ssize_t)len);
  usleep(250000);
}

static uint16_t read_version(const char *name) {
  char path[256];
  shm_path(name, path, sizeof(path));

  int fd = open(path, O_RDONLY);
  assert_true(fd >= 0);

  struct shm_structure s;
  ssize_t n = read(fd, &s, sizeof(s));
  close(fd);
  assert_true(n == (ssize_t)sizeof(s));
  return s.version;
}

static uint64_t read_master_clock(const char *name, bool secondary) {
  char path[256];
  shm_path(name, path, sizeof(path));

  int fd = open(path, O_RDONLY);
  assert_true(fd >= 0);

  struct shm_structure s;
  ssize_t n = read(fd, &s, sizeof(s));
  close(fd);
  assert_true(n == (ssize_t)sizeof(s));
  return secondary ? s.secondary.master_clock_id : s.main.master_clock_id;
}

static void poison_master_clock(const char *name, uint64_t value) {
  char path[256];
  shm_path(name, path, sizeof(path));

  int fd = open(path, O_RDWR);
  assert_true(fd >= 0);

  struct shm_structure *s = mmap(NULL, sizeof(*s), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  assert_ptr_not_equal(s, MAP_FAILED);

  s->main.master_clock_id = value;
  s->secondary.master_clock_id = value;
  msync(s, sizeof(*s), MS_SYNC);
  munmap(s, sizeof(*s));
}

static pid_t start_nqptp_daemon(const char *binary) {
  pid_t pid = fork();
  assert_true(pid >= 0);
  if (pid == 0) {
    execl(binary, binary, "-vv", (char *)NULL);
    _exit(127);
  }
  return pid;
}

static void stop_nqptp_daemon(pid_t pid) {
  if (pid <= 0)
    return;
  kill(pid, SIGTERM);
  for (int i = 0; i < 50; i++) {
    int status = 0;
    pid_t w = waitpid(pid, &status, WNOHANG);
    if (w == pid)
      return;
    usleep(100000);
  }
  kill(pid, SIGKILL);
  waitpid(pid, NULL, 0);
}

static void wait_for_daemon_start(pid_t pid) {
  usleep(600000);
  int status = 0;
  pid_t w = waitpid(pid, &status, WNOHANG);
  assert_true(w == 0);
}

static void test_multi_client_isolation(__attribute__((unused)) void **state) {
  if (geteuid() != 0)
    skip();

  const char *binary = getenv("NQPTP_BINARY");
  if (binary == NULL || binary[0] == '\0')
    binary = "../src/nqptp";

  remove_if_exists(kKitchen);
  remove_if_exists(kFoyer);

  pid_t daemon_pid = start_nqptp_daemon(binary);
  wait_for_daemon_start(daemon_pid);

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  assert_true(sock >= 0);

  char cmd[256];
  snprintf(cmd, sizeof(cmd), "%s T %s", kKitchen, kPeerIP);
  send_command(sock, cmd);
  snprintf(cmd, sizeof(cmd), "%s B", kKitchen);
  send_command(sock, cmd);

  snprintf(cmd, sizeof(cmd), "%s T %s", kFoyer, kPeerIP);
  send_command(sock, cmd);
  snprintf(cmd, sizeof(cmd), "%s B", kFoyer);
  send_command(sock, cmd);

  char kitchen_path[256];
  char foyer_path[256];
  shm_path(kKitchen, kitchen_path, sizeof(kitchen_path));
  shm_path(kFoyer, foyer_path, sizeof(foyer_path));
  assert_true(file_exists(kitchen_path));
  assert_true(file_exists(foyer_path));

  assert_int_equal(read_version(kKitchen), NQPTP_SHM_STRUCTURES_VERSION);
  assert_int_equal(read_version(kFoyer), NQPTP_SHM_STRUCTURES_VERSION);

  poison_master_clock(kKitchen, UINT64_C(0xAAAAAAAAAAAAAAAA));
  poison_master_clock(kFoyer, UINT64_C(0xBBBBBBBBBBBBBBBB));
  assert_true(read_master_clock(kKitchen, false) == UINT64_C(0xAAAAAAAAAAAAAAAA));
  assert_true(read_master_clock(kFoyer, false) == UINT64_C(0xBBBBBBBBBBBBBBBB));

  snprintf(cmd, sizeof(cmd), "%s T", kFoyer);
  send_command(sock, cmd);

  assert_true(read_master_clock(kFoyer, false) == 0);
  assert_true(read_master_clock(kKitchen, false) == UINT64_C(0xAAAAAAAAAAAAAAAA));
  assert_true(read_master_clock(kKitchen, true) == UINT64_C(0xAAAAAAAAAAAAAAAA));

  poison_master_clock(kFoyer, UINT64_C(0xCCCCCCCCCCCCCCCC));
  snprintf(cmd, sizeof(cmd), "%s E", kKitchen);
  send_command(sock, cmd);
  assert_true(read_master_clock(kFoyer, false) == UINT64_C(0xCCCCCCCCCCCCCCCC));

  close(sock);
  stop_nqptp_daemon(daemon_pid);
  remove_if_exists(kKitchen);
  remove_if_exists(kFoyer);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_multi_client_isolation),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
