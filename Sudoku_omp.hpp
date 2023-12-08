#include <bitset>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <iostream>
#include <omp.h>
#include <time.h>

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
    //## omp function
    void omp_solver(const int num_threads, Sudoku answer);
    bool omp_solve(bool *solved);
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
            // cout<<idx<<" "<<r<<" "<<c<<" "<<val<<" "<<r * T + val<<" "<<c * T + val<<endl;
            row.set(r * T + val);
            col.set(c * T + val);
            int block_size = sqrt(T);
            r = idx / T / block_size;
            c = idx % T / block_size;
            // cout<<idx<<" "<<r<<" "<<c<<" "<<val<<" "<<r * block_size * T + c * T + val<<endl;
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
bool Sudoku<T>::checkValid(int idx, int val){ // val can be used or not
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
            for(int k = 0; k < T * 2 - 1 + 2 * block_size - 2; ++k){
                ret += "-";
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
    int cnt = 0;
    while(!st.empty()){
        ++cnt;
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
            // cout<<"cnt: "<<cnt<<endl;
            return true;
        }
        vector<Sudoku> next[T];
        // Sudoku<9> next[T]; //## using array to avoid rasing on vector
        bool next_exist[T];
        // #pragma omp parallel for //## too slow for hard puzzle
        for(int i = 0; i < T; ++i){
            next_exist[i] = false;
            if(now.checkValid(idx, i)){
                Sudoku tmp = now;
                tmp.modify(idx, i);
                // cout<<idx<<" "<<i<<endl;
                // cout<<"tmp: "<<tmp.s<<endl;
                // next.push_back(tmp);
                next[i].push_back(tmp);
                next_exist[i] = true;
            }
        }
        // lock
        // for(auto &s : next){
        //     st.push(s);
        // }
        for(int i = 0; i < T; ++i){
            if(next_exist[i])
                st.push(next[i][0]);
        }
        // unlock
    }
    if(str != ""){
        (*this) = Sudoku(str);
        return true;
    }
    return false;
}
// write a higher slover that branch at top level
// first find n > thread num of branch
// than do serial solve in different threads
// make a share data that tell all threads return or not
template<int T>
void Sudoku<T>::omp_solver(const int num_threads, Sudoku answer){
    //# first, find branches
    vector<Sudoku> branches;
    branches.push_back((*this));
    int num_branches = 1;
    while (num_branches < num_threads){
        if (branches[0].firstEmpty() >= T * T) break; // in easy puzzles, some branches already filled
        vector<Sudoku> new_branches;
        int new_num_branches = 0;
        for (int i = 0; i < num_branches; ++i){
            Sudoku now = branches[i];
            int idx = now.firstEmpty();
            for(int j = 0; j < T; ++j){
                if(now.checkValid(idx, j)){
                    Sudoku tmp = now;
                    tmp.modify(idx, j);
                    new_branches.push_back(tmp);
                    new_num_branches++;
                }
            }
        }
        branches = new_branches;
        num_branches = new_num_branches;
        // cout << num_branches << endl;
    }
    // cout << "find " << num_branches << " branches" << endl;
    bool solved = false;
    double time_solve_start = clock();
    #pragma omp parallel for schedule(static, 1)
    for (int i = 0; i < num_branches; ++i){
        double time_solve_start = clock();
        Sudoku question = branches[i];
        // cout << question.firstEmpty() << endl;
        if(question.omp_solve(&solved)){
            // printf("Thread %d solved with: %.5lf sec\n", omp_get_thread_num (), ( clock() - time_solve_start ) / CLOCKS_PER_SEC);
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
            // break;
        }
        // else printf("Thread %d solving with: %.5lf sec\n", omp_get_thread_num (), ( clock() - time_solve_start ) / CLOCKS_PER_SEC);
    }
    if (!solved){
        cout<<"Error: question \""<<(*this).asString()<<"\" not been solved!!"<<endl;
    }
    // printf("Solving with: %.5lf sec\n", ( clock() - time_solve_start ) / CLOCKS_PER_SEC);
    // cout<<"end of omp_solver"<<endl;
}
template<int T>
bool Sudoku<T>::omp_solve(bool *solved){
    if (*solved) return false;
    stack<Sudoku> st;
    st.push((*this));
    string str = "";
    int cnt = 0;
    while(!st.empty() && !(*solved)){
        ++cnt;
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
            *solved = true;
            // cout<<"cnt: "<<cnt<<endl;
            return true;
        }
        vector<Sudoku> next[T];
        // Sudoku<9> next[T]; //## using array to avoid rasing on vector
        bool next_exist[T];
        // #pragma omp parallel for //## too slow for hard puzzle
        for(int i = 0; i < T; ++i){
            next_exist[i] = false;
            if(now.checkValid(idx, i)){
                Sudoku tmp = now;
                tmp.modify(idx, i);
                // cout<<idx<<" "<<i<<endl;
                // cout<<"tmp: "<<tmp.s<<endl;
                // next.push_back(tmp);
                next[i].push_back(tmp);
                next_exist[i] = true;
            }
        }
        // lock
        // for(auto &s : next){
        //     st.push(s);
        // }
        for(int i = 0; i < T; ++i){
            if(next_exist[i])
                st.push(next[i][0]);
        }
        // unlock
    }
    // if(str != ""){ // may not able to find the answer that different to std-ans??
    //     (*this) = Sudoku(str);
    //     return true;
    // }
    return false;
}