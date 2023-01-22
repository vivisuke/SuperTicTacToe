#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
    Board bd;
    bd.print();
    bd.put(0, 0, MARU);
    bd.print();
    bd.put(1, 1, MARU);
    bd.print();
    bd.put(2, 2, MARU);
    bd.print();
    //
    std::cout << "\nOK.\n";
}
