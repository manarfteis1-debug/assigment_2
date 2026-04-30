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

// تشوف خدي العملة او لا 
void collectCoin(State &s) {
    if (s.x == coinX[0] && s.y == coinY[0]) s.c1 = true;
    if (s.x == coinX[1] && s.y == coinY[1]) s.c2 = true;
    if (s.x == coinX[2] && s.y == coinY[2]) s.c3 = true;
    if (s.x == coinX[3] && s.y == coinY[3]) s.c4 = true;
}

// يشوف هل قام يتعبئة البنزين ام لا 
void refillFuel(State &s) {
    if (s.x == fuelX && s.y == fuelY)
        s.fuel = MAX_FUEL;
}

// يشوف وصلنا للهدف ام لا
bool goal(State s) {
    return s.x == 1 && s.y == 1 &&
           s.c1 && s.c2 && s.c3 && s.c4;
}

// تحسب في  (h) على حسب عدد العملات 
int h1(State s) {
    int count = 0;
    if (!s.c1) count++;
    if (!s.c2) count++;
    if (!s.c3) count++;
    if (!s.c4) count++;
    return count;
}

// تحسب مجموع المسافة من مكان الاعب الى العمل يلي مش واخدينها
int h2(State s) {
    int maxD = 0;
    int d;

    if (!s.c1) {
        d = manhattan(s.x, s.y, coinX[0], coinY[0]);
        if (d > maxD) maxD = d;
    }

    if (!s.c2) {
        d = manhattan(s.x, s.y, coinX[1], coinY[1]);
        if (d > maxD) maxD = d;
    }

    if (!s.c3) {
        d = manhattan(s.x, s.y, coinX[2], coinY[2]);
        if (d > maxD) maxD = d;
    }

    if (!s.c4) {
        d = manhattan(s.x, s.y, coinX[3], coinY[3]);
        if (d > maxD) maxD = d;
    }

    return maxD;
}

// تمنع الكود ان يخش في حلقة لا نهائية 
bool same(State a, State b) {
    return a.x == b.x &&
           a.y == b.y &&
           a.fuel == b.fuel &&
           a.c1 == b.c1 &&
           a.c2 == b.c2 &&
           a.c3 == b.c3 &&
           a.c4 == b.c4;
}

// تشوف الحالة الحالية مع الحالات يلي زارتهم من قبل هل موجوده ام لا  
bool visitedBefore(State visited[], int count, State s) {
    for (int i = 0; i < count; i++) {
        if (same(visited[i], s))
            return true;
    }
    return false;
}

//تطبع الحالات بالمعلومات يلي فيهم 
void printState(State s) {
    cout << "(" << s.x << "," << s.y << ") ";
    cout << "Fuel=" << s.fuel << " ";
    cout << "Coins=[" << s.c1 << s.c2 << s.c3 << s.c4 << "] ";
    cout << "g=" << s.g << " ";
    cout << "h=" << s.h << " ";
    cout << "f=" << s.f << endl;
}

//نطبع المسار النهائي من البداية للهدف 
void printPath(State allStates[], int index) {
    if (index == -1)
        return;

    printPath(allStates, allStates[index].parent);

    cout << "(" << allStates[index].x << "," << allStates[index].y << ")";

    if (!goal(allStates[index]))
        cout << " -> ";
}

//تنشئ حالة جديدة بعد كل حركة 
void makeChild(State current, State &child, int newX, int newY, int type, int parentIndex) {
    child = current;

    child.x = newX;
    child.y = newY;
    child.fuel = current.fuel - 1;
    child.g = current.g + 1;
    child.parent = parentIndex;

    collectCoin(child);
    refillFuel(child);

    if (type == 1)
        child.h = h1(child);
    else
        child.h = h2(child);

    child.f = child.g + child.h;
}

//تطبيق خورزمية ال greedy
void greedy(State start, int type) {
    State allStates[MAX_STATES];
    State visited[MAX_STATES];
    int frontier[MAX_STATES];

    int stateCount = 0;
    int frontierCount = 0;
    int visitedCount = 0;

    if (type == 1)
        start.h = h1(start);
    else
        start.h = h2(start);

    start.f = start.h;   
    start.parent = -1;

    allStates[stateCount] = start;
    frontier[frontierCount++] = stateCount;
    stateCount++;

    while (frontierCount > 0) {
        int best = 0;
//باش نختار الحالة الجاية
        for (int i = 1; i < frontierCount; i++) {
            if (allStates[frontier[i]].h < allStates[frontier[best]].h)
                best = i;
        }

        int currentIndex = frontier[best];
        State current = allStates[currentIndex];

        for (int i = best; i < frontierCount - 1; i++)
            frontier[i] = frontier[i + 1];

        frontierCount--;
// نتحقق من التكرار
        if (visitedBefore(visited, visitedCount, current))
            continue;

        visited[visitedCount++] = current;

        printState(current);

        if (goal(current)) {
            cout << "Goal Found (Greedy)" << endl;
            cout << "Cost = " << current.g << endl;
            cout << "Visited States = " << visitedCount << endl;
            cout << "Path Solution: ";
            printPath(allStates, currentIndex);
            cout << endl;
            return;
        }
//باش تتحددله الحركة انها فوق وتحت ويمين ويسار فقط
        int dx[4] = {0, 0, -1, 1};
        int dy[4] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (valid(nx, ny) && current.fuel > 0) {
                State child;
                makeChild(current, child, nx, ny, type, currentIndex);

                child.f = child.h;  // التصحيح المهم للـ Greedy

                if (!visitedBefore(visited, visitedCount, child)) {
                    allStates[stateCount] = child;
                    frontier[frontierCount++] = stateCount;
                    stateCount++;
                }
            }
        }
    }

    cout << "No solution found by Greedy" << endl;
}

//تطبيق A*
void a_star(State start, int type) {
    State allStates[MAX_STATES];
    State visited[MAX_STATES];
    int frontier[MAX_STATES];

    int stateCount = 0;
    int frontierCount = 0;
    int visitedCount = 0;

    if (type == 1)
        start.h = h1(start);
    else
        start.h = h2(start);

    start.f = start.g + start.h;  
    start.parent = -1;

    allStates[stateCount] = start;
    frontier[frontierCount++] = stateCount;
    stateCount++;

    while (frontierCount > 0) {
        int best = 0;

        for (int i = 1; i < frontierCount; i++) {
            if (allStates[frontier[i]].f < allStates[frontier[best]].f)
                best = i;
        }

        int currentIndex = frontier[best];
        State current = allStates[currentIndex];

        for (int i = best; i < frontierCount - 1; i++)
            frontier[i] = frontier[i + 1];

        frontierCount--;

        if (visitedBefore(visited, visitedCount, current))
            continue;

        visited[visitedCount++] = current;

        printState(current);

        if (goal(current)) {
            cout << "Goal Found (A*)" << endl;
            cout << "Cost = " << current.g << endl;
            cout << "Visited States = " << visitedCount << endl;
            cout << "Path Solution: ";
            printPath(allStates, currentIndex);
            cout << endl;
            return;
        }

        int dx[4] = {0, 0, -1, 1};
        int dy[4] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (valid(nx, ny) && current.fuel > 0) {
                State child;
                makeChild(current, child, nx, ny, type, currentIndex);

                child.f = child.g + child.h; // A*

                if (!visitedBefore(visited, visitedCount, child)) {
                    allStates[stateCount] = child;
                    frontier[frontierCount++] = stateCount;
                    stateCount++;
                }
            }
        }
    }

    cout << "No solution found by A*" << endl;
}

int main() {
    int x, y;

    cout << "Enter start position x (1-5): ";
    cin >> x;

    cout << "Enter start position y (1-5): ";
    cin >> y;

    if (!valid(x, y)) {
        cout << "Invalid position" << endl;
        return 0;
    }

    State start;

    start.x = x;
    start.y = y;
    start.fuel = MAX_FUEL;

    start.c1 = false;
    start.c2 = false;
    start.c3 = false;
    start.c4 = false;

    start.g = 0;
    start.h = 0;
    start.f = 0;
    start.parent = -1;

    collectCoin(start);
    refillFuel(start);

    cout << "\nGreedy h1" << endl;
    greedy(start, 1);

    cout << "\nGreedy h2" << endl;
    greedy(start, 2);

    cout << "\nA* h1" << endl;
    a_star(start, 1);

    cout << "\nA* h2" << endl;
    a_star(start, 2);

    return 0;
}