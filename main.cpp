#include "Sudoku.hpp"
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>

using namespace std;

int main(){
    fstream file;
    file.open("data/input_example", ios::in);
    // file.open("data/puzzle2_17_clue", ios::in);
    double time_start = clock();
    string str;
    while(getline(file, str)){
        Sudoku<9> question(str.substr(0, 81));
        Sudoku<9> answer(str.substr(84, 81));
        cout<<question.asString()<<endl;
        cout<<question.asSudoku();
        double time_solve_start = clock();
        if(question.solve()){
            cout<<"Solved!!!"<<endl;
            cout<<question.asSudoku();
            if(question == answer){
                cout<<"same as answer"<<endl;
            }
            else{
                cout<<"not same as answer"<<endl;
                cout<<answer.asSudoku();
            }
        }
        printf("Solving with: %.5lf sec\n", ( clock() - time_solve_start ) / CLOCKS_PER_SEC);
    }
    printf("Total running time: %.5lf sec\n", ( clock() - time_start ) / CLOCKS_PER_SEC);
}
//test test