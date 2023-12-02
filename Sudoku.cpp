
#include "Sudoku.h"
#include <bitset>
#include <string>
#include <vector>
#include <cmath>
#include <stack>

using namespace std;

template<int T>
Sudoku<T>::Sudoku(string str){
    s = str;
    row.reset();
    col.reset();
    block.reset();
    for(int idx = 0; idx < s.size(); ++idx){
        if(s[idx] != '.'){
            int val = s[idx] - '1';
            int r = idx / T;
            int c = idx % t;
            row.set(r * T + val);
            col.set(c * T + val);
            int block_size = sqrt(T);
            r = idx / block_size;
            c = idx % T / block_size;
            block.set(r * block_size * T + c * T + val);
        }
    }
}
// // full puzzle
// bool checkValid(){
//     if(std::any_of(row.begin(), row.end(), [](int k){return k > 1}) != row.end())
//         return false;
//     if(std::any_of(col.begin(), col.end(), [](int k){return k > 1}) != col.end())
//         return false;
//     if(std::any_of(block.begin(), block.end(), [](int k){return k > 1}) != block.end())
//         return false;
//     return 
// }
// point 0 based
template<int T>
bool Sudoku<T>::checkValid(int idx, int val){
    // for(int i = 0; i )
    // return checkValid();
    int r = idx / T;
    int c = idx % t;
    if(row.test(r * T + val)){
        return false;
    }
    if(col.test(c * T + val)){
        return false;
    }
    int block_size = sqrt(T);
    r = idx / block_size;
    c = idx % T / block_size;
    if(block.test(r * block_size * T + c * T + val)){
        return false;
    }
    return true;
}
template<int T>
int Sudoku<T>::firstEmpty(){
    for(int i = 0; i < s.size(); ++i){
        if(s[i] == '.')
            return i;
    }
    return s.size();
}

template<int T>
bool Sudoku<T>::modify(int idx, int val){
    char origin_val = s[idx];
    if(s[idx] != '.'){
        int val = s[idx] - '1';
        int r = idx / T;
        int c = idx % t;
        row.reset(r * T + val);
        col.reset(c * T + val);
        int block_size = sqrt(T);
        r = idx / block_size;
        c = idx % T / block_size;
        block.reset(r * block_size * T + c * T + val);
    }
    if(!checkValid(idx, val)){
        s[idx] = origin_val;
        return false;
    }
    if(val != '.'){
        int r = idx / T;
        int c = idx % t;
        row.set(r * T + val);
        col.set(c * T + val);
        int block_size = sqrt(T);
        r = idx / block_size;
        c = idx % T / block_size;
        block.set(r * block_size * T + c * T + val);
    }
    return true;
}
template<int T>
string Sudoku<T>::asString(){
    return s;
}
template<int T>
string Sudoku<T>::asSudoku(){
    int cntx = 0, cnty = 0;
    string ret = "";
    int block_size = sqrt(T);
    for(int i = 0; i < T; ++i){
        for(int j = 0; j < T; ++j){
            ret += s[i * T + j] + " "
            if(i && i % block_size == 0){
                ret += "| "
            }
        }
        ret += "\n";
        if(j && j % block_size == 0){
            for(int k = 0; k < T * 2 - 1 + block_size - 1; ++k){
                ret += "--"
            }
            ret += "\n";
        }
    }

    return ret;
}
template<int T>
bool Sudoku<T>::solve(){
    stack<Sudoku> st;
    while(!st.empty()){
        // lock
        Sudoku now = st.top();
        st.pop();
        // unlock
        vector<Sudoku> next;
        for(int i = 0; i < N; ++i){
            if(checkValid(idx, i)){
                Sudoku tmp = now;
                tmp.modify(idx, val);
                next.push_back(tmp);
            }
        }
        // lock
        for(auto &s : next){
            st.push(s);
        }
        // unlock
    }
}