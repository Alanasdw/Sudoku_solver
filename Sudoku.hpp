#include <bitset>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <iostream>

template <int T>
class Sudoku{
public:
    Sudoku(std::string str);
    // point 0 based
    bool checkValid(int idx, int val);
    // first empty cell
    int firstEmpty();
    // -1 = empty, return true = modify successfully
    bool modify(int idx, int val);
    std::string asString();
    std::string asSudoku();
    bool solve();
    bool operator == (const Sudoku& rhs) const {
        return s == rhs.s;
    }
private:
    std::bitset<T*T> row;
    std::bitset<T*T> col;
    std::bitset<T*T> block;
    std::string s = ""; // sudoku puzzle
};

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
            int c = idx % T;
            cout<<idx<<" "<<r<<" "<<c<<" "<<val<<" "<<r * T + val<<" "<<c * T + val<<endl;
            row.set(r * T + val);
            col.set(c * T + val);
            int block_size = sqrt(T);
            r = idx / T / block_size;
            c = idx % T / block_size;
            cout<<idx<<" "<<r<<" "<<c<<" "<<val<<" "<<r * block_size * T + c * T + val<<endl;
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
    int c = idx % T;
    // cout<<r<<" "<<c<<" "<<val<<endl;
    if(row.test(r * T + val)){
        return false;
    }
    if(col.test(c * T + val)){
        return false;
    }
    int block_size = sqrt(T);
    r = idx / T / block_size;
    c = idx % T / block_size;
    // cout<<r<<" "<<c<<" "<<block_size<<" "<<val<<endl;
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
        int c = idx % T;
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
        int c = idx % T;
        row.set(r * T + val);
        col.set(c * T + val);
        int block_size = sqrt(T);
        r = idx / T / block_size;
        c = idx % T / block_size;
        block.set(r * block_size * T + c * T + val);
        s[idx] = '1' + val;
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
        if(i && i % block_size == 0){
            for(int k = 0; k < T * 2 - 1 + block_size - 1; ++k){
                ret += "--";
            }
            ret += "\n";
        }
        for(int j = 0; j < T; ++j){
            if(j && j % block_size == 0){
                ret += "| ";
            }
            ret += (char)(s[i * T + j]);
            ret += " ";
        }
        ret += "\n";
    }

    return ret;
}
template<int T>
bool Sudoku<T>::solve(){
    stack<Sudoku> st;
    st.push((*this));
    string str = "";
    while(!st.empty()){
        // cout<<"st size: "<<st.size()<<endl;
        // lock
        Sudoku now = st.top();
        st.pop();
        // cout<<now.s<<endl;
        // unlock
        int idx = now.firstEmpty();
        // cout<<"Idx: "<<idx<<endl;
        if(idx == s.size()){
            str = now.s;
            // 不確定可不可以break或return所以我把改值放到最後面了
            // 必須要break不然會跑太久;
            (*this) = now;
            return true;
        }
        vector<Sudoku> next;
        for(int i = 0; i < T; ++i){
            if(now.checkValid(idx, i)){
                Sudoku tmp = now;
                tmp.modify(idx, i);
                // cout<<idx<<" "<<i<<endl;
                // cout<<"tmp: "<<tmp.s<<endl;
                next.push_back(tmp);
            }
        }
        // lock
        for(auto &s : next){
            st.push(s);
        }
        // unlock
    }
    if(str != ""){
        (*this) = Sudoku(str);
        return true;
    }
    return false;
}