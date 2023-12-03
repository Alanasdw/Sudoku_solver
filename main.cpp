#include "Sudoku.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(){
    fstream file;
    file.open("data/input_example", ios::in);
    string str;
    while(getline(file, str)){
        Sudoku<9> question(str.substr(0, 81));
        Sudoku<9> answer(str.substr(84, 81));
        cout<<question.asString()<<endl;
        cout<<question.asSudoku();
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
    }
}