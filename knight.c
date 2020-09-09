/**************************************/
/* author: Shahar maimon              */
/* date: 03.25.20                     */
/**************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "knight.h"
#include "bitarr.h"
#include "heap_PQ.h"

#define TABLE_SIZE (64)
#define COLUMN (8)
#define TIMER (300)

typedef enum status
{
    SUCCESS,
    FAIL,
    TIMEOUT,
    MEMORY_ALLOCATION_FAILED
}status_t;

typedef struct options
{
    size_t index;
    size_t count_opt;
}options_t;

static int isValid(int final_x, int final_y);
static void initLutMoves(size_t LutMoves[]);
 static status_t knightRec(size_t path[], size_t LutMoves[], size_t point, size_t board, time_t start_time); 
static size_t CountOpt(size_t LutMoves[], bit_arr_t board, size_t i);
static status_t knightRecWarnsdorf(size_t path[], size_t LutMoves[], size_t point, bit_arr_t board);
static int CmpFunc(void *data1, void *data2);
static pr_queue_t *PQ_createAndCmp(size_t LutMoves[], bit_arr_t board, size_t point);
static void Destroy(pr_queue_t *pr_queue);

 int KnightTour(size_t path[64], size_t start_point)
{
    size_t LutMoves[TABLE_SIZE] = {0};
    size_t board = 0;

    initLutMoves(LutMoves);

    board = BitArrSetOn(board, start_point);

    path[0] = start_point;

   return knightRec(path + 1, LutMoves, start_point, board, time(0));
} 


int KnightTourWarnsdorf(size_t path[], size_t start_point)
{
    size_t LutMoves[TABLE_SIZE] = {0};
    size_t board = 0;

    initLutMoves(LutMoves);

    board = BitArrSetOn(board, start_point);

    path[0] = start_point;

   return knightRecWarnsdorf(path + 1, LutMoves, start_point, board);
}


 static status_t knightRecWarnsdorf(size_t path[], size_t LutMoves[], size_t point, bit_arr_t board)
 {
    size_t min_index;
    status_t status = FAIL;
    pr_queue_t *pr_queue = NULL;
    void *options_struct = NULL;

    if(board == ~0)
    {

         return SUCCESS;
    }

    pr_queue = PQ_createAndCmp(LutMoves, board, point);
    if (NULL == pr_queue)
    {
        fprintf(stderr, "allocation failed\n");
        return MEMORY_ALLOCATION_FAILED;
    }

    
    while(status == FAIL && !PQIsEmpty(pr_queue))
    {
        options_struct = PQPeek(pr_queue);
        min_index = ((options_t*)options_struct)->index;
        PQDequeue(pr_queue);
        free(options_struct);

        *path = min_index;
        status = knightRecWarnsdorf(path + 1, LutMoves, min_index, BitArrSetOn(board, min_index));
    }  
     
    Destroy(pr_queue);
    return status;
 }




 

 static status_t knightRec(size_t path[], size_t LutMoves[], size_t point, size_t board, time_t start_time)
 {
     size_t i =0;
     status_t status = FAIL;

     if(board == ~0)
     {
         return SUCCESS;
     }

  
     for(i = 0; (i < TABLE_SIZE)  && (status == FAIL); i++ )
     {
         if(time(0) - start_time >= TIMER)
         {
             return TIMEOUT;
         }
         if(BitArrGetBit(LutMoves[point], i))
         {
             if(BitArrGetBit(board, i) != 1)
             {
                *path = i;
                status = knightRec(path + 1, LutMoves, i, BitArrSetOn(board, i), start_time);
             }        
         } 
     }

     return status;
 } 
 


static size_t CountOpt(size_t LutMoves[], bit_arr_t board, size_t i)
{
    size_t count = 0;
    size_t j =0;

    for(j=0; j<TABLE_SIZE; j++)
    {
        if((BitArrGetBit(LutMoves[i], j) == 1) && BitArrGetBit(board, j) == 0)
        {
            count++;
        }
    }
    return count;
}





static void initLutMoves(size_t LutMoves[])
{
    int difx[] = {-2, -1, 1, 2, -2, -1, 1, 2}; 
    int dify[] = {-1, -2, -2, -1, 1, 2, 2, 1}; 

    size_t i = 0, j = 0;
    int y_index = 0, x_index = 0;
    int final_y = 0, final_x = 0;
    int move = 0;

    for(i = 0; i < TABLE_SIZE; i++)
    {
        y_index = i/COLUMN;
        x_index = i%COLUMN;

        for(j = 0; j < COLUMN; j++)
        {
            final_x = difx[j] + x_index;
            final_y = dify[j] + y_index;
            if(isValid(final_x, final_y))
            {
                move = final_y*COLUMN + final_x;
                LutMoves[i] = BitArrSetOn(LutMoves[i], move);
            }
        }
    }
}



static int isValid(int final_x, int final_y)
{
    return ((final_x >= 0) && (final_x < COLUMN)) && ((final_y >= 0) && (final_y < COLUMN));
}




static pr_queue_t *PQ_createAndCmp(size_t LutMoves[], bit_arr_t board, size_t point)
{
    pr_queue_t *pr_queue = NULL;
    size_t i = 0;

    pr_queue = PQCreate(CmpFunc);
    if(NULL == pr_queue)
    {
        printf("Error. Allocation was unsuccessful. \n");
        return NULL;
    }

    for(i = 0; i < TABLE_SIZE ; i++ )
    {
        if(BitArrGetBit(LutMoves[point], i) == 1  && (BitArrGetBit(board, i) != 1) )
        {
            options_t *option = NULL;

            option = (options_t*)malloc(sizeof(options_t));
            if(NULL == option)
            {
                fprintf(stderr, "allocation failed\n");
                return NULL;
            }

            option->index = i;
            option->count_opt = CountOpt(LutMoves, board,  option->index);
          
            if(PQEnqueue(pr_queue, option) != 1)
            {
                return NULL;
            }
        }
    }
    return pr_queue;
}



static int CmpFunc(void *data1, void *data2)
{
    options_t *options1 = (void*)data1;
    options_t *options2 = (void*)data2;

    return options1->count_opt - options2->count_opt;
}

static void Destroy(pr_queue_t *pr_queue)
{
    void *options_struct = NULL;

    while(!PQIsEmpty(pr_queue))
    {
        options_struct = PQPeek(pr_queue);
        PQDequeue(pr_queue);
        free(options_struct);
    }
    PQDestroy(pr_queue);
}
