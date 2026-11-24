#!/usr/bin/env python3
"""Unit checks for critical nqptp-clock-sources.c invariants."""

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT / "nqptp-clock-sources.c"


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    text = SOURCE.read_text(encoding="utf-8")

    require("if (strlen(client_shared_memory_interface_name) >=" in text,
            "client interface names should be length-checked")
    require('client_shared_memory_interface_name != NULL ? client_shared_memory_interface_name'
            in text, "debug logging should guard NULL client names")
    require("if ((client_id < 0) || (client_id >= MAX_CLIENTS))" in text,
            "delete_client should reject out-of-range ids")
    require('debug(1, "cannot getaddrinfo for ip: %s.", sender_string);' in text,
            "getaddrinfo failure should log the sender string")
    require("// first, see if you can find it anywhere" in text,
            "spelling consistency expected in client lookup comment")

    print("ALL NQPTP CLOCK SOURCES SOURCE INVARIANT CHECKS PASSED")


if __name__ == "__main__":
    try:
        main()
    except AssertionError as e:
        print(f"FAILED: {e}")
        sys.exit(1)
