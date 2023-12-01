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
        if ( input[ 0] != '#')
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
                    // repeat again
                    printf("error input character [%c]\n", input[ i]);
                }// else
            }// for i
        }// if
    } while ( input[ 0] == '#');

    free( input);
    input = NULL;
    return;
}

bool is_valid( const int *puzzle)
{
    int row[ N][ N] = { 0}, col[ N][ N] = { 0}, block[ N][ N] = { 0};

    for ( int i = 0; i < N; i += 1)
    {
        for ( int j = 0; j < N; j += 1)
        {
            if ( puzzle[ i * N + j] != 0)
            {
                row[ i][ puzzle[ i * N + j]] += 1;
                col[ j][ puzzle[ i * N + j]] += 1;
                block[ i / SUB_N * SUB_N + j / SUB_N][ puzzle[ i * N + j]] += 1;
            }// if
        }// for j
    }// for i

    // check duplicate
    for ( int i = 0; i < N; i += 1)
    {
        for ( int j = 0; j < N; j += 1)
        {
            if ( row[ i][ j] > 1 || col[ i][ j] > 1 || block[ i][ j] > 1)
            {
                return false;
            }// if
        }// for j
    }// for i

    return true;
}

bool solve( const int *puzzle, int *solution)
{
    if ( !is_valid( puzzle))
    {
        return false;
    }// if

    int temp_sol[ N * N];
    memcpy( temp_sol, puzzle, sizeof( int) * N * N);

    // find first empty
    int target = -1;
    for ( int i = 0; i < N * N; i += 1)
    {
        if ( puzzle[ i] == 0)
        {
            target = i;
        }// if
    }// for i

    // no empty spaces => solved
    if ( target == -1)
    {
        memcpy( solution, puzzle, sizeof( int) * N * N);
        return true;
    }// if
    
    // guess all possible numbers
    for ( int i = 0; i < N; i += 1)
    {
        temp_sol[ target] = i + 1;
        // printf("guess %d\n", i);
        if ( solve( temp_sol, solution))
        {
            return true;
        }// if
    }// for i
    
    return false;
}


int main( void)
{
    int *puzzle = malloc( sizeof( int) * N * N);
    int *sol = malloc( sizeof( int) * N * N);

    // get 1 puzzle and solve
    input( puzzle);
    print_sudoku( puzzle);
    bool solution = solve( puzzle, sol);
    if ( solution)
    {
        // solved
        printf("solution found\n");
        print_sudoku( sol);
    }// if
    else
    {
        printf("no answer\n");
    }// else

    free( puzzle);
    puzzle = NULL;
    free( sol);
    sol = NULL;

    return 0;
}
