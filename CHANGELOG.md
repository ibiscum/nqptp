# Changelog

## Unreleased

### Fixed
- Guard wake-up announce message allocation in `send_awakening_announcement_sequence` by checking `malloc` for `NULL` before use.
- Make wake-up destination resolution family-consistent by setting `getaddrinfo` hints to the selected `ip_family`.
- Remove stale, unused `epoll_fd` state from `nqptp.c`.
- Align the real-time scheduling comment with the actual configured `SCHED_FIFO` priority (`5`).
- Fix mislabelled `getaddrinfo` failure text in `nqptp-utilities.c`.
- Prevent socket table overflow in `open_sockets_at_port` by enforcing `MAX_OPEN_SOCKETS` bounds.
- Close socket descriptors on bind/setup failure paths in `open_sockets_at_port` to prevent descriptor leaks.
- Update `nqptp-utilities.c` metadata/signature consistency (copyright year and `debug_print_buffer` spacing).
- Update `nqptp-ptp-definitions.h` metadata and comment clarity.
- Replace zero-length tail arrays in `nqptp-ptp-definitions.h` with C99 flexible array members for better standards conformance.
- Make control-port parsing in `nqptp-message-handlers.c` robust for packets that are not NUL-terminated by copying into a bounded temporary buffer when needed.
- Remove unreachable `command == NULL` check from the stop-timing control-flow branch in `nqptp-message-handlers.c`.
- Remove brittle fixed Follow_Up TLV offset usage and derive debug output from parsed TLV fields instead.
- Replace non-reentrant global hex formatting buffer in `nqptp-message-handlers.c` with a local, caller-supplied buffer helper.
- Remove stale undeclared API surface from `nqptp-clock-sources.h` by dropping obsolete function declarations that no longer have implementations.
- Update `nqptp-clock-sources.h` metadata/comment consistency (copyright year and wording fix).
- Harden `get_client_id` in `nqptp-clock-sources.c` with client-name length validation, deterministic slot use, and null-safe debug logging.
- Improve `nqptp-clock-sources.c` robustness by bounds-checking `delete_client` input IDs.
- Fix `create_clock_source_record` getaddrinfo failure logging to report the requested sender IP.
- Update `nqptp-clock-sources.c` metadata/comment consistency.
- Update `general-utilities` metadata consistency and const-correctness for `timespec_to_ns`.
- Correct misleading inline comment on `nctoh64` byte handling in `general-utilities.c`.
- Handle `clock_gettime` failures explicitly in `get_time_now` with fatal diagnostics.
- Remove unused exported `timespec_to_ns` helper and stale commented legacy implementation.
- Correct `nctohl`/`nctohs` inline comments from “characters” to “bytes”.
- Add an include guard to `debug.h` and normalize `debug.h`/`debug.c` metadata years.
- Add `--with-journald-logging=auto|yes|no` configure support and conditional `libsystemd` integration.
- Add native journald logging backend in `debug.c` with automatic fallback to stderr if journald is unavailable or send fails.

### Tests
- Update `test-multi-client-isolation.py` to expect shared-memory interface version `11`.
- Add `test-nqptp-source-invariants.py` unit checks to guard critical `nqptp.c` invariants for allocation safety and address-family consistency.
- Add `test-nqptp-utilities-source-invariants.py` unit checks to guard critical `nqptp-utilities.c` socket setup invariants.
- Add `test-nqptp-message-handlers-source-invariants.py` unit checks to guard control-message parsing and Follow_Up TLV parsing invariants.
- Add `test-nqptp-clock-sources-source-invariants.py` unit checks to guard `nqptp-clock-sources.c` client/lookup safety invariants.

### Documentation
- Update `README.md` to describe per-client timing peer groups and named shared-memory interfaces.
- Update `README.md` control-port protocol description to include `<smi-name> <command> ...` message format.
- Update `README.md` shared-memory struct example to include `clock_change_offset`.
- Update `nqptp-shm-structures.h` control-port protocol comments to match the current client-prefixed message format.
- Update `nqptp.8` FILES section to document per-client shared-memory segments (`/dev/shm/<interface-name>`).
- Update `README.md` build documentation to describe `--with-journald-logging` behavior and fallback.
