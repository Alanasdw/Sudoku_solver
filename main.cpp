//#include "Sudoku.hpp"
#include "Sudoku_omp.hpp" //# OpenMP version
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <omp.h>
#include <string.h>

using namespace std;

int main(){
    // bool runOMP = true;
    bool runOMP = false;
    
    //## OpenMP
    int num_threads = omp_get_max_threads();
    if (runOMP) cout << "num_threads= " << num_threads << endl;
    omp_set_num_threads(num_threads);
    
    //## File
    // string filename = "data/input_example";
    string filename = "data/ans0_kaggle";
    fstream file;
    file.open(filename, ios::in);
    cout<<"running file: "<<filename<<endl;

    //## Run
    double time_start = clock();
    string str;
    while(getline(file, str)){
        Sudoku<9> question(str.substr(0, 81));
        Sudoku<9> answer(str.substr(84, 81));
        // cout<<question.asString()<<endl;
        // cout<<question.asSudoku();
        if (runOMP) question.omp_solver(num_threads, answer); //##
        else {
            double time_solve_start = clock();
            if(question.solve()){
                // cout<<"Solved!!!"<<endl;
                // cout<<question.asSudoku();
                // if(question == answer){
                //     cout<<"same as answer"<<endl;
                // }
                // else{
                if(!(question == answer)){
                    cout<<"not same as answer"<<endl;
                    cout<<answer.asSudoku();
                }
            }
            else{
                cout<<"Error: question \""<<question.asString()<<"\" not been solved!!"<<endl;
                break;
            }
            // printf("Solving with: %.5lf sec\n", ( clock() - time_solve_start ) / CLOCKS_PER_SEC);
        }
        // break; //##
    }
    printf("Total running time: %.5lf sec\n", ( clock() - time_start ) / CLOCKS_PER_SEC);
}
/*
test data: data/input_example (first 5 puzzles)
    serial: 913 sec (15.2 min)
    omp: 171 sec (~3 min)
test data: data/input_example (all 10 puzzles)
    serial: 1426 sec (~24 min)
    omp: 265 sec (~5 min)
test data: data/ans0_kaggle
    serial: 4.8 sec
    omp: 8 sec
*/