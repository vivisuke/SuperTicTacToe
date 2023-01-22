#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
    Board bd;
    bd.print();
    bd.put(0, 0, MARU);
    //bd.print();
    bd.put(1, 1, MARU);
    //bd.print();
    bd.put(2, 2, MARU);
    bd.print();
    bd.put(3, 3, BATSU);
    bd.print();
    bd.put(4, 3, BATSU);
    bd.put(5, 3, BATSU);
    bd.print();
    bd.undo_put();
    bd.print();
    //
    std::cout << "\nOK.\n";
}
