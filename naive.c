#define _GNU_SOURCE     // for getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>    // for bool

#define N 9
#define SUB_N 3

struct sSudoku
{
    int puzzle[ N * N];
    int candidate[ N * N][ N]; // possibility of all positions
    int position_left;
};


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

void print_candidate( const int candidate[ N * N][N])
{
    for ( int i = 0; i < N * N; i += 1)
    {
        printf("%d:", i);
        for ( int j = 0; j < N; j += 1)
        {
            if ( candidate[ i][ j])
            {
                printf(" %d", j + 1);
            }// if
        }// for j
        printf("\n");
    }// for i
    
    return;
}

void update_candidate( struct sSudoku *sudoku)
{
    sudoku->position_left = 0;
    for ( int i = 0; i < N * N; i += 1)
    {
        // for every point not figured out
        if ( sudoku->puzzle[ i] == 0)
        {
            sudoku->position_left += 1;
            // add all
            for ( int j = 0; j < N; j += 1)
            {
                sudoku->candidate[ i][ j] = 1;
            }// for j
            
            int start = ( i / N) * N;
            // printf("%d start %d\n", i, start);
            // check row
            for ( int j = start; j < start + N; j += 1)
            {
                if ( sudoku->puzzle[ j] == 0)
                {
                    continue;
                }// if
                // remove from candicate
                sudoku->candidate[ i][ sudoku->puzzle[ j] - 1] = 0;
            }// for j
            
            start = i % N;
            // check col
            for ( int j = 0; j < N; j += 1)
            {
                if ( sudoku->puzzle[ start + j * N] == 0)
                {
                    continue;
                }// if
                // remove from candicate
                sudoku->candidate[ i][ sudoku->puzzle[ start + j * N] - 1] = 0;
            }// for j

            // check block
            start = i / ( N * N) * ( N * N);
            for ( int row = 0; row < SUB_N; row += 1)
            {
                for ( int col = 0; col < SUB_N; col += 1)
                {
                    if ( sudoku->puzzle[ start + row * N + col] == 0)
                    {
                        continue;
                    }// if
                    // remove from candicate
                    sudoku->candidate[ i][ sudoku->puzzle[ start + row * N + col] - 1] = 0;
                }// for col
            }// for row
        }// if
    }// for i
    
    return;
}

bool solve( struct sSudoku *sudoku)
{
    update_candidate( sudoku);
    // print_candidate( sudoku->candidate);
    // printf("position left %d\n", sudoku->position_left);

    if ( sudoku->position_left == 0)
    {
        return true;
    }// if
    
    
    return false;
}

int main()
{
    struct sSudoku sudoku = { 0};

    // get 1 puzzle and solve
    input( sudoku.puzzle);
    print_sudoku( sudoku.puzzle);
    bool solution = solve( &sudoku);
    if ( solution)
    {
        // solved
        print_sudoku( sudoku.puzzle);
    }// if
    else
    {
        printf("no answer\n");
    }// else

    return 0;
}
