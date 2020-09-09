#ifndef __KNIGHT_H__ 
#define __KNIGHT_H__

#include <stddef.h> /* size_t */

int KnightTour(size_t path[64], size_t start_point);
int KnightTourWarnsdorf(size_t path[], size_t start_point);

#endif