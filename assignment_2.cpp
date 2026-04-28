#include <iostream>
using namespace std;

const int SIZE = 5; 
const int MAX_FUEL = 20; 
const int MAX_STATES = 20000; //اقصى حد للحالات 

struct State {   
    int x , y; 
    int fuel;
    bool c1 , c2 , c3 , c4;
    int g, h, f;
    int parent; // يخزن كل حالة من وين جت 
};

// الخدمات التابة في اللعبة 
int walls[5][5] = { 
    {0,0,0,0,0},
    {0,1,1,1,0},
    {0,0,0,1,0},
    {0,1,0,0,0},
    {0,0,0,1,0}
};

int coinX[4] = {1, 5, 1, 5};
int coinY[4] = {1, 1, 5, 5};

int fuelX = 3;
int fuelY = 3;

//  قيمة مطلقة 
int myAbs(int n) { 
    if (n < 0) return -n;
    return n;
}

// تحسب ال h2 
int manhattan(int x1, int y1, int x2, int y2) {
    return myAbs(x1 - x2) + myAbs(y1 - y2);
}

// تحقق من خروج من الشبكة و برا جدار ام لا
bool valid(int x, int y) {
    if (x < 1 || x > SIZE || y < 1 || y > SIZE)
        return false;

    if (walls[y - 1][x - 1] == 1)
        return false;

    return true;
}