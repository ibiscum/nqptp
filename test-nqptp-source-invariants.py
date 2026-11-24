#!/usr/bin/env python3
"""Unit checks for critical nqptp.c invariants.

These checks guard against regressions in recently fixed issues:
* wake-up announce message allocation must be null-checked before use
* wake-up destination resolution must match the selected socket family
* stale epoll state must not reappear
* scheduling comment and configured priority must agree
"""

import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT / "nqptp.c"


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    text = SOURCE.read_text(encoding="utf-8")

    require("int epoll_fd;" not in text, "stale epoll_fd global should not exist")

    require(
        "// try to set a real time scheduling policy with a priority of 5" in text,
        "scheduling comment must reflect configured priority",
    )
    require("param.sched_priority = 5;" in text, "expected sched_priority=5")

    malloc_guard = re.compile(
        r"msg\s*=\s*malloc\s*\(\s*msg_length\s*\)\s*;\s*"
        r"if\s*\(\s*msg\s*==\s*NULL\s*\)\s*\{\s*"
        r'warn\("Could not allocate %zu bytes for announce message\.",\s*msg_length\);\s*'
        r"return;\s*\}\s*"
        r"memset\s*\(\s*\(void \*\)msg\s*,\s*0\s*,\s*msg_length\s*\)\s*;",
        re.S,
    )
    require(
        malloc_guard.search(text) is not None,
        "announce message must be null-checked before memset/use",
    )

    require("hints.ai_family = ip_family;" in text, "getaddrinfo hints must use ip_family")
    require("hints.ai_flags = 0;" in text, "getaddrinfo hints flags should be 0")
    require(
        "hints.ai_family = AF_UNSPEC;" not in text,
        "AF_UNSPEC must not be used in wake-up address resolution",
    )

    print("ALL NQPTP SOURCE INVARIANT CHECKS PASSED")


if __name__ == "__main__":
    try:
        main()
    except AssertionError as e:
        print(f"FAILED: {e}")
        sys.exit(1)
