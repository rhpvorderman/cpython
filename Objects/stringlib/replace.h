/* stringlib: replace implementation */

#ifndef STRINGLIB_FASTSEARCH_H
#error must include "stringlib/fastsearch.h" before including this module
#endif

Py_LOCAL_INLINE(void)
STRINGLIB(replace_1char_inplace)(STRINGLIB_CHAR* s, STRINGLIB_CHAR* end,
                                 Py_UCS4 u1, Py_UCS4 u2, Py_ssize_t maxcount)
{
    *s = u2;
    while (--maxcount && ++s != end) {
        /* Find the next character to be replaced.

           If it occurs often, it is faster to scan for it using an inline
           loop.  If it occurs seldom, it is faster to scan for it using a
           function call; the overhead of the function call is amortized
           across the many characters that call covers.  We start with an
           inline loop and use a heuristic to determine whether to fall back
           to a function call. */
        if (*s != u1) {
            int attempts = 10;
            /* search u1 in a dummy loop */
            while (1) {
                if (++s == end)
                    return;
                if (*s == u1)
                    break;
                if (!--attempts) {
                    /* if u1 was not found for attempts iterations,
                       use FASTSEARCH() or memchr() */
#ifdef STRINGLIB_FAST_MEMCHR
                    s++;
                    s = STRINGLIB_FAST_MEMCHR(s, u1, end - s);
                    if (s == NULL)
                        return;
#else
                    Py_ssize_t i;
                    STRINGLIB_CHAR ch1 = (STRINGLIB_CHAR) u1;
                    s++;
                    i = FASTSEARCH(s, end - s, &ch1, 1, 0, FAST_SEARCH);
                    if (i < 0)
                        return;
                    s += i;
#endif
                    /* restart the dummy loop */
                    break;
                }
            }
        }
        *s = u2;
    }
}

Py_LOCAL_INLINE(void)
STRINGLIB(replace_1char_copy_no_maxcount)(
    const STRINGLIB_CHAR *restrict source,
    STRINGLIB_CHAR *restrict dest,
    Py_ssize_t n,
    const STRINGLIB_CHAR to_replace,
    const STRINGLIB_CHAR replacement)
{
    /* Code below is very easy to vectorize using vector compare functions
       and the compiler will do so. */
    for (Py_ssize_t i=0; i<n; i++) {
        STRINGLIB_CHAR current_char = source[i];
        if (current_char == to_replace) {
            dest[i] = replacement;
        } else {
            dest[i] = current_char;
        }
    }
}
