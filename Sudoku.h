#include <bitset>
#include <string>

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
    static bool solve();
private:
    std::bitset<T*T> row(0);
    std::bitset<T*T> col(0);
    std::bitset<T*T> block(0);
    std::string s = ""; // sudoku puzzle
};