#define _GNU_SOURCE     // for getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>    // for bool

#define N 9
#define SUB_N 3

void print_sudoku( const int *puzzle)
{
    int cntx = 0, cnty = 0;
    for ( int i = 0; i < N; i += 1)
    {
        for ( int j = 0; j < N; j += 1)
        {
            if ( puzzle[ i * N + j] == 0)
            {
                printf("  ");
            }// if
            else
            {
                printf("%d ", puzzle[ i * N + j]);
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

void input( int *buffer)
{
    char *input = NULL;
    size_t len = 0;

    do
    {
        if ( getline( &input, &len, stdin) == -1)
        {
            printf("getline error\n");
            break;
        }// if
        
        len = strlen( input);
        // printf("%s] %ld\n", input, len);
        if ( len == N * N + 1)
        {
            // the line we want
            for ( int i = 0; i < N * N; i += 1)
            {
                if ( input[ i] == '.')
                {
                    buffer[ i] = 0;
                }// if
                else if ( input[ i] >= '0' && input[ i] <= '9')
                {
                    buffer[ i] = input[ i] - '0';
                }// else if
                else
                {
                    printf("error input character [%c]\n", input[ i]);
                }// else
            }// for i
        }// if
    } while ( len != N * N + 1);

    free( input);
    input = NULL;
    return;
}

int find_empty( const int *puzzle)
{
    // returns -1 if finished
    int index = -1;

    for ( int i = 0; i < N * N; i += 1)
    {
        if ( puzzle[ i] == 0)
        {
            index = i;
            break;
        }// if
    }// for i
    
    return index;
}

bool solve( int *puzzle, int next)
{

    return false;
}

int main()
{
    int *puzzle = NULL;
    puzzle = calloc( N * N, sizeof(int));

    // get 1 puzzle and solve
    input( puzzle);
    print_sudoku( puzzle);
    printf("first slot index %d\n", find_empty( puzzle));
    // bool solution = solve( puzzle);
    // print_sudoku( puzzle);

    free( puzzle);
    puzzle = NULL;

    return 0;
}
