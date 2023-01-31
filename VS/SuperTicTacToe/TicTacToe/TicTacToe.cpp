#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
	Board bd;
	bd.print();
	bd.put(1, 2, WHITE);
	bd.put(2, 1, BLACK);
	bd.print();
	//
    std::cout << "\nOK.\n";
}
