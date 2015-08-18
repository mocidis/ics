#ifndef __ICS_COMMON_H__
#define __ICS_COMMON_H__
#include <errno.h>
#include <string.h>
#define ICS_EXIT_IF_TRUE(clause, message) if(clause) { \
    fprintf(stderr, message); \
    exit(-1); \
}
#define ICS_RETURN_IF_TRUE(clause, message) if(clause) { \
    fprintf(stderr, message); \
    exit(-1); \
}

#endif
