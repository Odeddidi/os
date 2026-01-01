# Cache demo (`cache-demo`)

A small C program that simulates a set-associative cache and prints the cache contents after a sequence of reads.

## Files

- `cache.c`: cache implementation + interactive `main`
- `cache.h`: types and function declarations
- `Makefile`: build rules (`make`, `make clean`)

## Build

```sh
cd cache-demo
make
```

This produces:
- `cache` (executable)
- `cache.o` (object file)

## Run

```sh
cd cache-demo
./cache
```

### Program input format (interactive)

1) Enter memory size:

```text
Size of data: <n>
```

2) Enter `n` bytes (as integers) that represent the “memory” contents:

```text
Input data >> <byte0> <byte1> ... <byte(n-1)>
```

3) Enter cache parameters:

```text
s t b E: <s> <t> <b> <E>
```

- Number of sets: $S = 2^s$
- Block size: $B = 2^b$
- Associativity: `E` lines per set
- Tag bits printed using width `t`

4) Enter read offsets (integers). Each non-negative value triggers `read_byte(cache, mem, offset)`. Enter a negative number to stop and print the cache:

```text
0
7
12
-1
```

## Clean

```sh
cd cache-demo
make clean
```

## Which files are safe to delete?

Generated (safe to delete; will be recreated by `make`):
- `cache-demo/cache`
- `cache-demo/cache.o`

Local/editor-only (safe to delete if you don’t need it):
- `cache-demo/.vscode/`
