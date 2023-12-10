#define _GNU_SOURCE     // for getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>    // for bool
#include <pthread.h>    // for multi threading

#define N 9
#define SUB_N 3
#define STACK_MAX 100000000 /* 10**8 */
#define THREAD_COUNT 4

typedef struct _sSudoku
{
    char puzzle[ N * N + 1];
    int row[ N];
    int col[ N];
    int block[ N];
}sSudoku;

FILE *f_in;

inline int set( int number, int offset)
{
    number |= (1 << offset);
    return number;
}

inline int unset( int number, int offset)
{
    number &= ~(1 << offset);
    return number;
}

bool input( sSudoku *puzzle)
{
    size_t len = 0;
    char *input = NULL;
    bool success = true;

    do
    {
        // if ( getline( &input, &len, stdin) == -1)
        if ( getline( &input, &len, f_in) == -1)
        {
            if ( !feof( f_in))
            {
                printf("getline error\n");
            }// if
            success = false;
            break;
        }// if

        len = strlen( input);
        // printf("%s] %ld\n", input, len);
        if ( input[ 0] != '#')
        {
            // the line we want
            memcpy( puzzle -> puzzle, input, sizeof(char) * 81);
            puzzle -> puzzle[ 81] = '\0';
            for ( int i = 0; i < N; i += 1)
            {
                puzzle->row[ i] = 0;
                puzzle->col[ i] = 0;
                puzzle->block[ i] = 0;
            }// for i
            
            for (int i = 0; i < N; i += 1)
            {
                for ( int j = 0; j < N; j += 1)
                {
                    char target = input[ i * N + j];
                    if ( target >= '0' && target <= '9')
                    {
                        puzzle->row[ i] = set( puzzle->row[ i], target - '1');
                        puzzle->col[ j] = set( puzzle->col[ j], target - '1');
                        puzzle->block[ i / SUB_N * SUB_N + j / SUB_N] = set( puzzle->block[ i / SUB_N * SUB_N + j / SUB_N], target - '1');
                    }// else if
                    else if ( input[ i] != '.')
                    {
                        // repeat again
                        printf("error input character [%c]\n", input[ i]);
                        success = false;
                    }// else
                }// for j
            }// for i
            

            // for ( int i = 0; i < N * N; i += 1)
            // {
            //     if ( input[ i] >= '0' && input[ i] <= '9')
            //     {
            //         puzzle->row[ i] = set( puzzle->row[ i], input[ i] - '0');
            //         puzzle->col[ i] = set( puzzle->row[ i], input[ i] - '0');
            //         puzzle->block[ i] = set( puzzle->row[ i], input[ i] - '0');
            //     }// else if
            //     else if ( input[ i] != '.')
            //     {
            //         // repeat again
            //         printf("error input character [%c]\n", input[ i]);
            //         success = false;
            //     }// else
            // }// for i
        }// if
        if ( !success)
        {
            break;
        }// if
    } while ( input[ 0] == '#');

    free( input);
    input = NULL;
    return success;
}

void print_sudoku( const sSudoku puzzle)
{
    printf("%s\n", puzzle.puzzle);
    // printf("%s",);

    printf("row: ");
    for ( int i = 0; i < N; i += 1)
    {
        printf("%d ", puzzle.row[ i]);
    }// for i
    printf("\n");
    printf("col: ");
    for ( int i = 0; i < N; i += 1)
    {
        printf("%d ", puzzle.col[ i]);
    }// for i
    printf("\n");
    printf("block: ");
    for ( int i = 0; i < N; i += 1)
    {
        printf("%d ", puzzle.block[ i]);
    }// for i
    printf("\n");
    
    return;
}

void pretty_print_sudoku( const sSudoku puzzle)
{
    int cntx = 0, cnty = 0;
    for ( int i = 0; i < N; i += 1)
    {
        for ( int j = 0; j < N; j += 1)
        {
            if ( puzzle.puzzle[ i * N + j] == '0')
            {
                printf("  ");
            }// if
            else
            {
                printf("%c ", puzzle.puzzle[ i * N + j]);
            }// else
            // printf("%d ", puzzle[ i * N + j]);
            cntx += 1;
            if ( cntx == SUB_N)
            {
                printf("| ");
                cntx = 0;
            }// if
        }// for j
        cnty += 1;
        if ( cnty == SUB_N)
        {
            printf("\n");
            for ( int k = 0; k < 2 * N + SUB_N + (SUB_N - 1); k += 1)
            {
                printf("-");
            }// for i
            cnty = 0;
        }// if
        printf("\n");
    }// for i

    return;
}

int main( void)
{
    f_in = fopen("data/input_example", "r");

    sSudoku puzzle;
    input( &puzzle);

    pretty_print_sudoku( puzzle);
    print_sudoku( puzzle);

    // cleanup
    fclose( f_in);
    f_in = NULL;
    return 0;
}
