#define _GNU_SOURCE     // for getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>    // for bool

#define N 9
#define SUB_N 3
#define STACK_MAX 100000000 /* 10**8 */

int *stack_base;
int stack_len;
int max_used = 0;

FILE *f_in = NULL;

void print_sudoku( const int *puzzle)
{
    for ( int i = 0; i < N * N; i += 1)
    {
        if ( puzzle[ i] == 0)
        {
            printf(".");
        }// if
        else
        {
            printf("%d", puzzle[ i]);
        }// else
    }// for i
    
    return;
}

void pretty_print_sudoku( const int *puzzle)
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

bool input( int *buffer)
{
    char *input = NULL;
    size_t len = 0;
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
                    success = false;
                }// else
            }// for i
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

void init_stack( void)
{
    // stack stores the full puzzle
    stack_base = malloc( sizeof(int) * N * N * STACK_MAX);
    stack_len = 0;
    return;
}

void free_stack( void)
{
    free( stack_base);
    stack_base = NULL;
    stack_len = 0;
    return;
}

int push_stack( const int* puzzle)
{
    if ( stack_len == STACK_MAX)
    {
        // stack full
        printf("stack full error %d\n", stack_len);
        return 1;
    }// if

    max_used = max_used > stack_len? max_used: stack_len;    

    memcpy( stack_base + stack_len * N * N, puzzle, sizeof( int) * N * N);
    stack_len += 1;
    
    return 0;
}

int pop_stack( int *puzzle)
{
    if ( stack_len == 0)
    {
        // stack full
        printf("stack empty error %d\n", stack_len);
        return 1;
    }// if

    stack_len -= 1;
    memcpy( puzzle, stack_base + stack_len * N * N, sizeof( int) * N * N);
    return 0;
}

void print_stack( void)
{
    for ( int i = 0; i < stack_len; i += 1)
    {
        printf("[");
        for ( int j = 0; j < N * N; j += 1)
        {
            printf("%d", stack_base[ i * N * N + j]);
        }// for j
        printf("]\n");
    }// for i
    
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
                row[ i][ puzzle[ i * N + j] - 1] += 1;
                col[ j][ puzzle[ i * N + j] - 1] += 1;
                block[ (i / SUB_N) * SUB_N + j / SUB_N][ puzzle[ i * N + j] - 1] += 1;
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

bool solve( int *solution)
{
    int answer = false;
    int puzzle[ N * N];

    while ( stack_len != 0)
    {   
        pop_stack( puzzle);

        if ( !is_valid( puzzle))
        {
            continue;
        }// if

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
            answer = true;
            break;
        }// if

        // guess all possible numbers
        for ( int i = 0; i < N; i += 1)
        {
            puzzle[ target] = i + 1;
            // printf("guess %d\n", i);
            push_stack( puzzle);
        }// for i
    }// while

    return answer;
}

int main( void)
{
    int *puzzle = malloc( sizeof( int) * N * N);
    int *sol = malloc( sizeof( int) * N * N);


    // f_in = fopen("data/input_example", "r");
    f_in = fopen("data/puzzle2_17_clue", "r");

    // get 1 puzzle and solve
    while ( input( puzzle))
    {
        init_stack();

        push_stack( puzzle);
        bool solution = solve( sol);
        if ( solution)
        {
            // solved
            // printf("solution found\n");
            // pretty_print_sudoku( sol);
            print_sudoku( puzzle);
            printf(":1:");
            print_sudoku( sol);
            printf("\n");
        }// if
        else
        {
            printf("no answer\n");
        }// else

        free_stack();
    }// while

    printf("max stack used: %d\n", max_used);

    fclose( f_in);
    f_in = NULL;

    free( puzzle);
    puzzle = NULL;
    free( sol);
    sol = NULL;

    return 0;
}
