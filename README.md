# bbj_string.h

A small, single-header, public-domain, dynamic string type for C,
written in the [stb-style](https://github.com/nothings/stb) single-file
library format.

## Usage

```c
#define BBJ_STRING_IMPLEMENTATION
#include "bbj_string.h"
```

Do this in exactly *one* `.c` file. Every other file that needs the
declarations just includes `bbj_string.h` without the define.

```c
bbj_string s = bbj_make_string("Hello");
bbj_string_append(&s, ", World!");
bbj_print_string(&s); // Hello, World!
bbj_destroy_string(&s);
```

Optionally `#define BBJ_STRING_STATIC` before including to make the
whole API `static` (private to that translation unit), useful if
you're including this in more than one `.c` file in the same project.

## API

| Function | Description |
|---|---|
| `bbj_make_string(data)` | Construct a string, copying `data`. |
| `bbj_destroy_string(s)` | Free the string's buffer. |
| `bbj_print_string(s)` | Print the string followed by a newline. |
| `bbj_string_c_str(s)` | Get a `const char*` view of the string. |
| `bbj_string_length(s)` / `bbj_string_capacity(s)` | Accessors. |
| `bbj_string_empty(s)` | Is the string empty? |
| `bbj_string_at(s, i)` | Bounds-checked pointer to character `i`. |
| `bbj_string_append(s, data)` | Append `data` to the end. |
| `bbj_string_insert(s, i, data)` | Insert `data` at index `i`. |
| `bbj_string_pop(s)` | Remove the last character. |
| `bbj_string_pop_at(s, i)` | Remove the character at index `i`. |
| `bbj_string_clear(s)` | Reset length to 0, keep the buffer. |
| `bbj_string_reserve(s, n)` | Ensure capacity for at least `n` bytes. |
| `bbj_string_cmp(a, b)` | `strcmp`-style comparison. |
| `bbj_string_find(s, needle)` | Index of first match, or `BBJ_STRING_NPOS`. |
| `bbj_string_copy(s)` | Deep clone into a new owned string. |
| `bbj_string_substr(s, start, len)` | Extract a range into a new owned string. |

## Design notes

- **Ownership**: every `bbj_string` owns its buffer. Functions that
  construct a new `bbj_string` (`bbj_make_string`, `bbj_string_copy`,
  `bbj_string_substr`) always allocate and copy, never alias another
  string's buffer.
- **`int` for length/capacity**: kept small and simple, matching the
  signed-index style used elsewhere (`BBJ_STRING_NPOS` is `-1`).
  Strings over `INT_MAX` bytes are not supported.
- **NULL-safety**: every function checks its pointer arguments and
  fails safely (returns `NULL`/`BBJ_STRING_NPOS`/an empty string, or
  no-ops), printing a message to `stderr` rather than crashing.
- **Not thread-safe**: a single `bbj_string` must not be used from
  more than one thread at a time without external synchronization.

## Testing

<<<<<<< Updated upstream
`tests/tests.c` is a small assert-based test suite covering every
function, including bounds/NULL edge cases. Build and run it with:

```sh
cc -std=c99 -Wall -Wextra -o test_stb_string tests.c
./tests
=======
`test_bbj_string.c` is a small assert-based test suite covering every
function, including bounds/NULL edge cases. Build and run it with:

```sh
cc -std=c99 -Wall -Wextra -o test_bbj_string test_bbj_string.c
./test_bbj_string
>>>>>>> Stashed changes
```

## License

Public domain, or MIT-equivalent if your lawyers require it — see the
<<<<<<< Updated upstream
license block at the top of `stb_string.h`.
=======
license block at the top of `bbj_string.h`.
>>>>>>> Stashed changes
