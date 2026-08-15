# stb_string.h

A small, single-header, public-domain, dynamic string type for C,
written in the [stb-style](https://github.com/nothings/stb) single-file
library format.

## Usage

```c
#define STB_STRING_IMPLEMENTATION
#include "stb_string.h"
```

Do this in exactly *one* `.c` file. Every other file that needs the
declarations just includes `stb_string.h` without the define.

```c
stb_string s = stb_make_string("Hello");
stb_string_append(&s, ", World!");
stb_print_string(&s); // Hello, World!
stb_destroy_string(&s);
```

Optionally `#define STB_STRING_STATIC` before including to make the
whole API `static` (private to that translation unit), useful if
you're including this in more than one `.c` file in the same project.

## API

| Function | Description |
|---|---|
| `stb_make_string(data)` | Construct a string, copying `data`. |
| `stb_destroy_string(s)` | Free the string's buffer. |
| `stb_print_string(s)` | Print the string followed by a newline. |
| `stb_string_c_str(s)` | Get a `const char*` view of the string. |
| `stb_string_length(s)` / `stb_string_capacity(s)` | Accessors. |
| `stb_string_empty(s)` | Is the string empty? |
| `stb_string_at(s, i)` | Bounds-checked pointer to character `i`. |
| `stb_string_append(s, data)` | Append `data` to the end. |
| `stb_string_insert(s, i, data)` | Insert `data` at index `i`. |
| `stb_string_pop(s)` | Remove the last character. |
| `stb_string_pop_at(s, i)` | Remove the character at index `i`. |
| `stb_string_clear(s)` | Reset length to 0, keep the buffer. |
| `stb_string_reserve(s, n)` | Ensure capacity for at least `n` bytes. |
| `stb_string_cmp(a, b)` | `strcmp`-style comparison. |
| `stb_string_find(s, needle)` | Index of first match, or `STB_STRING_NPOS`. |
| `stb_string_copy(s)` | Deep clone into a new owned string. |
| `stb_string_substr(s, start, len)` | Extract a range into a new owned string. |

## Design notes

- **Ownership**: every `stb_string` owns its buffer. Functions that
  construct a new `stb_string` (`stb_make_string`, `stb_string_copy`,
  `stb_string_substr`) always allocate and copy, never alias another
  string's buffer.
- **`int` for length/capacity**: kept small and simple, matching the
  signed-index style used elsewhere (`STB_STRING_NPOS` is `-1`).
  Strings over `INT_MAX` bytes are not supported.
- **NULL-safety**: every function checks its pointer arguments and
  fails safely (returns `NULL`/`STB_STRING_NPOS`/an empty string, or
  no-ops), printing a message to `stderr` rather than crashing.
- **Not thread-safe**: a single `stb_string` must not be used from
  more than one thread at a time without external synchronization.

## Testing

`tests/tests.c` is a small assert-based test suite covering every
function, including bounds/NULL edge cases. Build and run it with:

```sh
cc -std=c99 -Wall -Wextra -o test_stb_string tests.c
./tests
```

## License

Public domain, or MIT-equivalent if your lawyers require it — see the
license block at the top of `stb_string.h`.
