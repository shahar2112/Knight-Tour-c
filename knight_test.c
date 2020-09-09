#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "knight.h"

#define TEST(real,expected) (real == expected ? printf("%d, Pass\n", expected) : printf("%d, Fail\n",expected))

#define RED	printf("33[1;31m")
#define BLUE printf("33[1;36m")
#define MAGENTA	printf("33[1;35m")
#define YELLOW printf("33[1;33m")
#define GREEN printf("33[1;32m")
#define DEFAULT	printf("33[0m")

int main()
{
    size_t start_point = 0;
    size_t path[64] = {0};
    size_t i = 0;
    int status = 0;

    srand(time(0)); 
    start_point = rand()%64;

    printf("rand is %lu\n", start_point);

    for(i=0; i<64; i++)
    {
        status = KnightTourWarnsdorf(path, i);
        if(status == 2)
        {
            printf("TIME OUT\n");
        }
        else if(status == 0)
        {
            printf("success in startpoint %lu\n", i);
        }
        else
        {
            printf("FAIL in startpoint %lu\n", i);
        }
        
    }
    

    for(i=0; i<64; i++)
    {
       printf("path[%lu] is %lu\n", i, path[i]);
    }



    return 0;
}

