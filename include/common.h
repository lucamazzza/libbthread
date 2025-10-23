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
#define ASSERT(cond, msg) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "ASSERTION FAILED in %s:%d: %s\n", \
                __FILE__, __LINE__, message); \
            abort(); \
        } \
    } while (0)
#define DEBUG(fmt, ...)      fprintf(stderr, "DEBUG %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define ASSERT(cond, msg)   ((void)0)
#define DEBUG(fmt, ...)     ((void)0)
#endif
#define INFO(fmt, ...)      fprintf(stdout, "INFO: " fmt "\n", ##__VA_ARGS__);
#define WARN(fmt, ...)      fprintf(stderr, "WARNING %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define ERROR(fmt, ...)     fprintf(stderr, "ERROR %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(x)           __builtin_expect(!!(x), 1)
#define UNLIKELY(x)         __builtin_expect(!!(x), 0)
#define INLINE              __attribute__((always_inline)) inline
#else
#define LIKELY(x)           (void)(x)
#define UNLIKELY(x)         (void)(x)
#define INLINE              inline
#endif

#ifdef __cplusplus
}
#endif

#endif // __COMMON_H__
