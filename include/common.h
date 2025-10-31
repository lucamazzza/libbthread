/*
 * File:        common.h
 * Author:      Luca Mazza
 * Description: Common definitions and macros used across the project.
 * License:     MIT
 *
 * Copyright (c) 2025 Luca Mazza
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char bool;
#define true                ((bool)1)
#define false               ((bool)0)
#define TO_BOOL(x)          ((bool))!!(x))

#define ARRAY_SIZE(arr)     (sizeof(arr)/sizeof((arr)[0]))
#define MIN(a,b)            (((a)<(b))?(a):(b)
#define MAX(a,b)            (((a)>(b))?(a):(b))
#define SWAP(a,b)           do { a ^= b; b ^= a; a ^= b; } while (0)
#define UNUSED(x)           (void)(x)

#ifdef NDEBUG
#define __RELEASE
#else
#define __DEBUG
#endif

#ifdef __DEBUG
#define ASSERT(cond, msg)                                       \
    do {                                                        \
        if (!(condition)) {                                     \
            fprintf(stderr, "ASSERTION FAILED in %s:%d: %s\n",  \
                __FILE__, __LINE__, message);                   \
            abort();                                            \
        }                                                       \
    } while (0)
#define DEBUG(fmt, ...)      fprintf(stderr, "DEBUG %s:%d:      " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define ASSERT(cond, msg)   ((void)0)
#define DEBUG(fmt, ...)     ((void)0)
#endif
#define INFO(fmt, ...)      fprintf(stdout, "INFO:              " fmt "\n",                     ##__VA_ARGS__);
#define WARN(fmt, ...)      fprintf(stderr, "WARNING %s:%d:     " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define ERROR(fmt, ...)     fprintf(stderr, "ERROR %s:%d:       " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(x)           __builtin_expect(!!(x), 1)
#define UNLIKELY(x)         __builtin_expect(!!(x), 0)
#define INLINE              __attribute__((always_inline)) inline
#define UNREF               __attribute__((unused))
#else
#define LIKELY(x)           (void)(x)
#define UNLIKELY(x)         (void)(x)
#define INLINE              inline
#define UNREF
#endif

#ifdef __cplusplus
}
#endif

#endif // __COMMON_H__
