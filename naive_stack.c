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

int *stack_base;
int stack_len;
int max_used = 0;
int thread_compute;

bool has_answer;
bool end;
int *solution;

// mutex for solution and stack
pthread_mutex_t mux;
// pthread_mutex_t mux_stack;
pthread_cond_t cv_mux;

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

void *solve( void *data)
{
    // int thread_num = *(int *)data;

    int puzzle[ N * N];

    int candidates[ N * N * N];
    int valids;

    while ( 1)
    {
        pthread_mutex_lock( &mux);
        while ( stack_len == 0 && thread_compute >= 0)
        {
            if ( end)
            {
                break;
            }// if
            pthread_cond_wait( &cv_mux, &mux);
        }// while
        if ( end)
        {
            pthread_cond_signal( &cv_mux);
            pthread_mutex_unlock( &mux);
            break;
        }// if
        
        pop_stack( puzzle);
        thread_compute += 1;
        // printf("in compute %d\n", thread_compute);
        pthread_cond_signal( &cv_mux);
        pthread_mutex_unlock( &mux);

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
            pthread_mutex_lock( &mux);
            memcpy( solution, puzzle, sizeof( int) * N * N);
            end = true;
            has_answer = true;
            pthread_cond_signal( &cv_mux);
            pthread_mutex_unlock( &mux);
            break;
        }// if

        valids = 0;

        // guess all possible numbers
        for ( int i = 0; i < N; i += 1)
        {
            puzzle[ target] = i + 1;
            // printf("guess %d\n", i);
            if ( is_valid( puzzle))
            {
                memcpy( candidates + valids * N * N, puzzle, sizeof( int) * N * N);
                valids += 1;
            }// if
        }// for i

        pthread_mutex_lock( &mux);
        for ( int i = 0; i < valids; i += 1)
        {
            push_stack( candidates + i * N * N);
        }// for i
        thread_compute -= 1;
        pthread_cond_signal( &cv_mux);
        pthread_mutex_unlock( &mux);
    }// while

    pthread_exit( NULL);
}

int main( void)
{
    int *puzzle = malloc( sizeof( int) * N * N);
    int *sol = malloc( sizeof( int) * N * N);
    solution = sol;

    f_in = fopen("data/input_example", "r");
    // f_in = fopen("data/puzzle2_17_clue", "r");

    bool error = false;

    // get 1 puzzle and solve
    while ( input( puzzle))
    {
        pthread_t threads[ THREAD_COUNT];
        pthread_mutex_init( &mux, NULL);
        // pthread_mutex_init( &mux_stack, NULL);
        pthread_cond_init( &cv_mux, NULL);
        init_stack();

        push_stack( puzzle);

        has_answer = false;
        end = false;
        // spawn threads
        for ( int i = 0; i < THREAD_COUNT; i += 1)
        {
            thread_compute = 0;
            if ( pthread_create( &threads[ i], NULL, solve, NULL))
            {
                printf("pthread create error\n");
                error = true;
            }// if
        }// for i

        if ( error)
        {
            break;
        }// if

        // join threads
        for ( int i = 0; i < THREAD_COUNT; i += 1)
        {
            pthread_join( threads[ i], NULL);
        }// for i

        if ( has_answer)
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

        pthread_cond_destroy( &cv_mux);
        pthread_mutex_destroy( &mux);
        // pthread_mutex_destroy( &mux_stack);
        free_stack();
        // break;
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
