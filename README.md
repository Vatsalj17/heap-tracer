# Heap Tracer

It is a lightweight runtime memory allocation tracker for C programs, built using the `LD_PRELOAD` mechanism.
It intercepts standard allocation functions like `malloc`, `calloc`, `realloc`, and `free` to monitor heap activity and spot memory leaks in real time.

---

## How It Works

The magic happens in **`shim.c`**, which overrides the standard memory functions provided by `libc`.
Every allocation is recorded in a linked list that stores each pointer and its size.
When your program exits, a destructor function automatically prints a **Heap Summary** showing:

* Total bytes allocated and freed
* Number of allocations and frees
* Any memory still not released (leaks)

In short — it keeps tabs on your heap so you can catch leaks before they catch you.

---

## Building

```bash
make
```

This will build all the components — the shared library (`shim.so`) and the loader (`loader.out`).

---

## Usage

To trace any executable, simply run it through the loader:

```bash
./loader.out ./your_program
```

This sets `LD_PRELOAD` automatically and injects the tracer into your target program.

---

## Components Overview

| File             | Description                                                       |
| ---------------- | ----------------------------------------------------------------- |
| **`loader.c`**   | Wrapper that sets up `LD_PRELOAD` and executes the target binary. |
| **`shim.c`**     | Core tracer implementation with a linked-list memory tracker.     |
| **`shim-old.c`** | Legacy version using a static array instead of a linked list.     |
| **`test.c`**     | Sample program to test allocation, freeing, and leak detection.   |

---

## Cleaning Up

```bash
make clean
```

---

## Why This Exists

Because debugging memory leaks manually is a pain —
this project gives you a simple, transparent way to *see* your heap in action, without needing Valgrind or heavyweight tools.
