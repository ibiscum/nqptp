#!/usr/bin/env python3
"""Unit checks for critical nqptp-message-handlers.c invariants."""

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT / "nqptp-message-handlers.c"


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    text = SOURCE.read_text(encoding="utf-8")

    require("char hexcharbuffer[16384];" not in text, "global hex buffer should not exist")
    require("static void hex_string(" in text, "hex formatter should be local helper")
    require("buf + 86" not in text, "magic fixed packet offset should not be used")

    require(
        'else if ((strcmp(command, "T") == 0) && (ip_list == NULL))' in text,
        "stop-timing branch should not include unreachable command == NULL case",
    )
    require(
        "(command == NULL) || ((strcmp(command, \"T\") == 0)" not in text,
        "unreachable command == NULL disjunction should not remain",
    )

    require(
        "allocated_working_buf = malloc((size_t)recv_len + 1);" in text
        and "allocated_working_buf[recv_len] = '\\0';" in text,
        "control message parsing should preserve non-null-terminated payloads safely",
    )

    print("ALL NQPTP MESSAGE HANDLERS SOURCE INVARIANT CHECKS PASSED")


if __name__ == "__main__":
    try:
        main()
    except AssertionError as e:
        print(f"FAILED: {e}")
        sys.exit(1)
