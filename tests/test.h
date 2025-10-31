#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>
#include <string.h>
#include <memory.h>

char fill = '.';
int width = 50;
char buf[BUFSIZ];

#define TEST(name)                                      \
    do {                                                \
        int pad_len = width - strlen(name);             \
        memset(buf, fill, pad_len);                     \
        buf[pad_len] = '\0';                            \
        printf("Running test: %s%s", name, buf);        \
        test_count++;                                   \
    } while(0)

#define PASS()                                          \
    do {                                                \
        printf("\e[1;32mPASSED\e[0m\n");                \
        test_passed++;                                  \
    } while(0)

#define ASSERT_TEST(cond, msg)                          \
    do {                                                \
        if (!(cond)) {                                  \
            printf("\e[1;31mFAILED \n\t      -> %s\n\e[0m", msg);  \
            return;                                     \
        }                                               \
    } while(0)


#define PRINT_TITLE(title)                              \
    do {                                                \
        printf("\e[7m\e[1m%-70s\e[22m\e[27m\n", title);     \
    } while(0)
#endif // __TEST_H__
