#!/usr/bin/env python3
"""Unit checks for critical nqptp-utilities.c invariants."""

import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT / "nqptp-utilities.c"


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    text = SOURCE.read_text(encoding="utf-8")

    require("getaddrinfo: %s" in text, "getaddrinfo error text should name getaddrinfo")
    require("getifaddrs: %s" not in text, "stale getifaddrs error text should not remain")

    require(
        "if (sockets_open_stuff->sockets_open >= MAX_OPEN_SOCKETS)" in text,
        "socket table writes must be bounded",
    )

    failure_path_close = re.compile(r"if\s*\(\s*ret\s*==\s*0\s*\)\s*\{.*?\}\s*else\s*\{\s*close\(fd\);\s*\}", re.S)
    require(
        failure_path_close.search(text) is not None,
        "failed bind/setup socket must be closed",
    )

    require(
        "void debug_print_buffer(int level, char *sender, char *buf, size_t buf_len)" in text,
        "debug_print_buffer signature spacing should match header style",
    )

    print("ALL NQPTP UTILITIES SOURCE INVARIANT CHECKS PASSED")


if __name__ == "__main__":
    try:
        main()
    except AssertionError as e:
        print(f"FAILED: {e}")
        sys.exit(1)
