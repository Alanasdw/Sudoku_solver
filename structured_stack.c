#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>    // for bool
#include <pthread.h>    // for multi threading

#define N 9
#define SUB_N 3
#define STACK_MAX 100000000 /* 10**8 */
// #define thread_count thread_count

int thread_count = 1;

typedef struct _sSudoku
{
    char puzzle[ N * N + 1];
    int16_t row[ N];
    int16_t col[ N];
    int16_t block[ N];
}sSudoku;

typedef struct _sSudoku_stack
{
    int len;
    sSudoku *base;
} sSudoku_stack;

sSudoku_stack global_stack;
pthread_mutex_t mux_global;
pthread_cond_t cv_mux;
pthread_barrier_t barrier;
bool end;
bool has_answer;
sSudoku solution;
FILE *f_in;
int guess;

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
    size_t len = 200;
    char input[ 200] = { 0};
    bool success = true;

    do
    {
        // if ( getline( &input, &len, stdin) == -1)
        // if ( getline( &input, &len, f_in) == -1)
        if ( fgets( input, len, f_in) == NULL)
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
                    else if ( target != '.')
                    {
                        // repeat again
                        printf("error input character [%c]\n", target);
                        success = false;
                    }// else
                }// for j
            }// for i
        }// if
        if ( !success)
        {
            break;
        }// if
    } while ( input[ 0] == '#');

    // free( input);
    // input = NULL;
    return success;
}

void print_sudoku( const sSudoku puzzle)
{
    printf("%s", puzzle.puzzle);
    // printf("%s",);

    // printf("row: ");
    // for ( int i = 0; i < N; i += 1)
    // {
    //     printf("%d ", puzzle.row[ i]);
    // }// for i
    // printf("\n");
    // printf("col: ");
    // for ( int i = 0; i < N; i += 1)
    // {
    //     printf("%d ", puzzle.col[ i]);
    // }// for i
    // printf("\n");
    // printf("block: ");
    // for ( int i = 0; i < N; i += 1)
    // {
    //     printf("%d ", puzzle.block[ i]);
    // }// for i
    // printf("\n");
    
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

void init_stack( sSudoku_stack *stack)
{
    // stack stores the full puzzle
    stack -> base = malloc( sizeof(sSudoku) * STACK_MAX);
    stack -> len = 0;
    return;
}

void free_stack( sSudoku_stack *stack)
{
    free( stack -> base);
    stack -> base = NULL;
    stack -> len = 0;
    return;
}

int push_stack( sSudoku_stack *stack, const sSudoku *puzzle)
{
    if ( stack -> len == STACK_MAX)
    {
        // stack full
        printf("stack full error %d\n", stack -> len);
        return 1;
    }// if

    // max_used = max_used > stack_len? max_used: stack_len;

    memcpy( stack -> base + stack -> len, puzzle, sizeof(sSudoku));
    stack -> len += 1;
    
    return 0;
}

int pop_stack( sSudoku_stack *stack, sSudoku *puzzle)
{
    if ( stack -> len == 0)
    {
        // stack full
        printf("stack empty error %d\n", stack -> len);
        return 1;
    }// if

    stack -> len -= 1;
    memcpy( puzzle, stack -> base + stack -> len, sizeof(sSudoku));
    return 0;
}

int16_t valids( sSudoku puzzle, int location)
{
    int16_t answer = 0;

    answer = puzzle.row[ location / N]
            | puzzle.col[ location % N]
            | puzzle.block[( location / N) / SUB_N * SUB_N + ( location % N) / SUB_N];

    answer = ~answer;
    // printf("%d: possible %X\n", location, answer);
    return answer;
}

void sudoku_set( sSudoku *puzzle, int location, int value)
{
    puzzle -> puzzle[ location] = '1' + value;

    // update row, col, block
    puzzle -> row[ location / N] = set( puzzle -> row[ location / N], value);
    puzzle -> col[ location % N] = set( puzzle -> col[ location % N], value);
    puzzle -> block[( location / N) / SUB_N * SUB_N + ( location % N) / SUB_N] = set( puzzle -> block[( location / N) / SUB_N * SUB_N + ( location % N) / SUB_N], value);

    return;
}

void sudoku_unset( sSudoku *puzzle, int location)
{
    char value = puzzle -> puzzle[ location];
    puzzle -> puzzle[ location] = '.';

    // update row, col, block
    puzzle -> row[ location / N] = unset( puzzle -> row[ location / N], value - '1');
    puzzle -> col[ location % N] = unset( puzzle -> col[ location % N], value - '1');
    puzzle -> block[( location / N) / SUB_N * SUB_N + ( location % N) / SUB_N] = unset( puzzle -> block[( location / N) / SUB_N * SUB_N + ( location % N) / SUB_N], value - '1');

    return;
}

int get_stack_pop_num( sSudoku_stack *stack)
{
    if ( stack -> len == 0)
    {
        printf("stack empty get_pop error\n");
        return -1;
    }// if
    
    stack -> len -= 1;
    return stack -> len;
}

void pop_stack_num( sSudoku_stack *stack, sSudoku *puzzle, int offset)
{
    memcpy( puzzle, stack -> base + offset, sizeof(sSudoku));
    return;
}

void *solve( void *arg)
{
    sSudoku local_puzzle;
    sSudoku local_candidates[ N];
    int cand_len;

    pthread_barrier_wait( &barrier);

    while ( 1)
    {
        // check finished
        pthread_mutex_lock( &mux_global);

        while ( global_stack.len == 0)
        {
            if ( end)
            {
                break;
            }// if
            pthread_cond_wait( &cv_mux, &mux_global);
        }// while

        if ( end)
        {
            // pthread_cond_signal( &cv_get);
            pthread_cond_broadcast( &cv_mux);
            pthread_mutex_unlock( &mux_global);
            break;
        }// if
        pop_stack( &global_stack, &local_puzzle);
        pthread_cond_broadcast( &cv_mux);
        pthread_mutex_unlock( &mux_global);

        int16_t candidate;
        int empty_pos = -1;
        int possibles = 10;
        int16_t temp;
        // find empty
        for ( int i = 0; i < N * N; i += 1)
        {
            if ( local_puzzle.puzzle[ i] == '.')
            {
                temp = __builtin_popcount( valids( local_puzzle, i) & 0x01ff);
                // printf("temp %d\n", temp);
                // printf("valids %X\n", valids( local_puzzle, i));
                if ( possibles > temp)
                {
                    // a better choice
                    possibles = temp;
                    empty_pos = i;
                }// if
                
                // naive find
                // empty_pos = i;
                // break;
            }// if
        }// for i

        // no empty spots
        if ( empty_pos == -1)
        {
            pthread_mutex_lock( &mux_global);
            has_answer = true;
            end = true;
            memcpy( &solution, &local_puzzle, sizeof(sSudoku));
            pthread_cond_broadcast( &cv_mux);
            pthread_mutex_unlock( &mux_global);
            break;
        }// if
        // printf("poss %d\n", possibles);

        candidate = valids( local_puzzle, empty_pos);
        cand_len = 0;
        // printf("candidate %X\n", candidate);
        for ( int i = 0; i < N; i += 1)
        {
            if ( candidate & 0x01)
            {
                // printf("guessing %d\n", i + 1);
                // modify
                sudoku_set( &local_puzzle, empty_pos, i);
                // add
                memcpy( &local_candidates[ cand_len], &local_puzzle, sizeof(sSudoku));
                cand_len += 1;
                // revert
                sudoku_unset( &local_puzzle, empty_pos);
            }// if
            candidate = candidate >> 1;
        }// for i

        pthread_mutex_lock( &mux_global);
        for ( int i = 0; i < cand_len; i += 1)
        {
            push_stack( &global_stack, &local_candidates[ i]);
        }// for i
        // guess += cand_len;
        pthread_cond_broadcast( &cv_mux);
        pthread_mutex_unlock( &mux_global);
    }// while
    
    pthread_exit( NULL);
}

int main( int argc, char *argv[])
{
    thread_count = atoi( argv[ 1]);
    f_in = fopen( argv[ 2], "r");

    sSudoku puzzle;

    while ( input( &puzzle))
    {
        pthread_t threads[ thread_count];
        pthread_mutex_init( &mux_global, NULL);
        pthread_cond_init( &cv_mux, NULL);

        pthread_barrier_init( &barrier, NULL, thread_count);

        guess = 0;
        has_answer = false;
        end = false;
        init_stack( &global_stack);
        push_stack( &global_stack, &puzzle);

        for ( int i = 0; i < thread_count; i += 1)
        {
            if ( pthread_create( &threads[ i], NULL, solve, NULL))
            {
                printf("pthread create error\n");
                return 1;
            }// if
        }// for i

        for ( int i = 0; i < thread_count; i += 1)
        {
            pthread_join( threads[ i], NULL);
        }// for i

        if ( has_answer)
        {
            // pretty_print_sudoku( sol);
            print_sudoku( puzzle);
            printf(":1:");
            print_sudoku( solution);
            printf("\n");
            // printf("%d,\n", guess);
        }// if
        else
        {
            printf("no answer\n");
        }// else

        free_stack( &global_stack);

        pthread_barrier_destroy( &barrier);
        pthread_cond_destroy( &cv_mux);
        pthread_mutex_destroy( &mux_global);
        // break;
    }// while

    // cleanup
    fclose( f_in);
    f_in = NULL;
    return 0;
}
