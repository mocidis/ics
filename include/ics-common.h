#ifndef __ICS_COMMON_H__
#define __ICS_COMMON_H__
#include <errno.h>
#include <string.h>
#include "ansi-utils.h"

#define ICS_EXIT_IF_TRUE(clause, message) EXIT_IF_TRUE(clause, message)
#define ICS_RETURN_IF_TRUE(clause, message) RETURN_IF_TRUE(clause, message)

#endif
