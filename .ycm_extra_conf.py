import os.path as p

# CPPFLAGS=-g $(shell root-config --cflags)

# LDFLAGS=-g $(shell root-config --ldflags)
# LDLIBS=$(shell root-config --libs)

_FLAGS = [
    "-x",
    "c++",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-Wno-long-long",
    "-Wno-variadic-macros",
    "-fexceptions",
    "-DNDEBUG",
    # Just so we don't get warnings about unused variables
    "-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE",
    # "-I/snap/root-framework/880/usr/local/include",
    "-I" + p.abspath("."),
    "-I/usr/include/c++/9/",
    "-I/usr/include/x86_64-linux-gnu/c++/9/",
    "-I" + p.join(p.abspath("."), "third_party", "spdlog", "include"),
    "-I" + p.join(p.abspath("."), "third_party", "cppzmq"),
    "-std=c++17",
]


def Settings(**kwargs):
    language = kwargs["language"]

    if language == "cfamily":
        return {
            "flags": _FLAGS,
        }
