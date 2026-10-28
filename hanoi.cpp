// Tower of Hanoi - DS mini project (C++)
// Data structure : Stack (linked-list implementation), one stack per rod
// Technique      : Recursion
// Build          : g++ -std=c++17 -o hanoi hanoi.cpp
// Run            : ./hanoi            (menu)
//                  ./hanoi auto 4     (solve 4 disks automatically, no menu)

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// ---------------------------------------------------------------
// Stack implemented with a singly linked list (LIFO)
// ---------------------------------------------------------------
struct Node {
    int data;
    Node* next;
    Node(int d, Node* n) : data(d), next(n) {}
};

class Stack {
    Node* topNode;
    int count;

public:
    Stack() : topNode(nullptr), count(0) {}
    ~Stack() { clear(); }

    void push(int x) {
        topNode = new Node(x, topNode);
        count++;
    }

    int pop() {
        if (isEmpty()) return -1;
        Node* t = topNode;
        int v = t->data;
        topNode = t->next;
        delete t;
        count--;
        return v;
    }

    int peek() const { return isEmpty() ? -1 : topNode->data; }
    bool isEmpty() const { return topNode == nullptr; }
    int size() const { return count; }

    void clear() {
        while (!isEmpty()) pop();
    }

    // copy contents bottom -> top into arr, returns how many
    int toArray(int arr[]) const {
        int i = count - 1;
        for (Node* p = topNode; p != nullptr; p = p->next) arr[i--] = p->data;
        return count;
    }
};

// ---------------------------------------------------------------
// Game state
// ---------------------------------------------------------------
const int MAX_DISKS = 10;
Stack rod[3];            // rod[0] = A, rod[1] = B, rod[2] = C
int numDisks = 3;
long long moveCount = 0;
bool printSteps = true;

char rodName(int i) { return char('A' + i); }

void resetGame(int n) {
    numDisks = n;
    moveCount = 0;
    for (int i = 0; i < 3; i++) rod[i].clear();
    for (int d = n; d >= 1; d--) rod[0].push(d);
}

// Draw the three rods sideways, level by level, top to bottom
void drawBoard() {
    int arr[3][MAX_DISKS];
    int sz[3];
    for (int r = 0; r < 3; r++) sz[r] = rod[r].toArray(arr[r]);

    int width = numDisks * 2 + 1;
    cout << "\n";
    for (int level = numDisks - 1; level >= 0; level--) {
        for (int r = 0; r < 3; r++) {
            string cell;
            if (level < sz[r]) {
                int d = arr[r][level];
                int pad = (width - (d * 2 - 1)) / 2;      // keeps every cell the same width
                cell = string(pad, ' ') + string(d * 2 - 1, '=') + string(pad, ' ');
            } else {
                cell = string(numDisks, ' ') + "|" + string(numDisks, ' ');
            }
            cout << cell << "   ";
        }
        cout << "\n";
    }
    for (int r = 0; r < 3; r++) {
        cout << string(numDisks, '-') << rodName(r) << string(numDisks, '-') << "   ";
    }
    cout << "\n";
}

// ---------------------------------------------------------------
// Moving a disk (validates the rules). Returns true if the move was legal.
// ---------------------------------------------------------------
bool moveDisk(int from, int to) {
    if (from == to || rod[from].isEmpty()) return false;
    int d = rod[from].peek();
    if (!rod[to].isEmpty() && rod[to].peek() < d) return false;   // larger on smaller
    rod[from].pop();
    rod[to].push(d);
    moveCount++;
    return true;
}

// ---------------------------------------------------------------
// The recursive solution
//   move n disks from 'from' to 'to' using 'aux'
//   1) move n-1 disks   from -> aux
//   2) move the biggest from -> to
//   3) move n-1 disks   aux  -> to
// Time: O(2^n)   Recursion depth: n
// ---------------------------------------------------------------
void solve(int n, int from, int to, int aux) {
    if (n == 0) return;                       // base case
    solve(n - 1, from, aux, to);
    int d = rod[from].peek();
    moveDisk(from, to);
    if (printSteps) {
        cout << "Move " << moveCount << ": disk " << d << "  " << rodName(from) << " -> "
             << rodName(to) << "\n";
    }
    solve(n - 1, aux, to, from);
}

bool isSolved() { return rod[2].size() == numDisks; }

// ---------------------------------------------------------------
// Menu helpers
// ---------------------------------------------------------------
int readInt(const string& prompt, int lo, int hi) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= lo && x <= hi) return x;
        if (cin.eof()) exit(0);                  // input ended: leave instead of looping forever
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Enter a number from " << lo << " to " << hi << ".\n";
    }
}

void autoSolve(int n, bool showBoard) {
    resetGame(n);
    printSteps = true;
    if (showBoard) drawBoard();
    solve(n, 0, 2, 1);
    if (showBoard) drawBoard();
    cout << "\nSolved " << n << " disks in " << moveCount << " moves (minimum is "
         << ((1LL << n) - 1) << ").\n";
}

void playYourself() {
    int n = readInt("Number of disks (3-10): ", 3, 10);
    resetGame(n);
    cout << "\nMove disks from A to C. Type a move like 'AC' (from A to C), "
            "or 'Q' to quit.\n";
    while (!isSolved()) {
        drawBoard();
        cout << "Moves: " << moveCount << "   Your move: ";
        string s;
        if (!(cin >> s)) return;                 // input ended
        if (s == "q" || s == "Q") return;
        if (s.size() != 2) {
            cout << "Type two letters, e.g. AC.\n";
            continue;
        }
        int f = toupper(s[0]) - 'A', t = toupper(s[1]) - 'A';
        if (f < 0 || f > 2 || t < 0 || t > 2) {
            cout << "Rods are A, B and C.\n";
            continue;
        }
        if (!moveDisk(f, t)) {
            cout << "*** Invalid move: a larger disk cannot go on a smaller one, "
                    "and the source rod must not be empty. ***\n";
        }
    }
    drawBoard();
    cout << "\nYou solved it in " << moveCount << " moves. Minimum possible: "
         << ((1LL << n) - 1) << ".\n";
}

void showFormula() {
    cout << "\nMinimum moves for n disks = 2^n - 1\n";
    cout << " n   moves\n";
    for (int n = 1; n <= 10; n++) cout << (n < 10 ? "  " : " ") << n << "   " << ((1LL << n) - 1) << "\n";
}

int main(int argc, char* argv[]) {
    // quick mode:  ./hanoi auto <n>
    if (argc == 3 && string(argv[1]) == "auto") {
        int n = atoi(argv[2]);
        if (n < 1 || n > MAX_DISKS) {
            cout << "n must be 1 to " << MAX_DISKS << "\n";
            return 1;
        }
        autoSolve(n, n <= 5);
        return 0;
    }

    while (true) {
        cout << "\n===== TOWER OF HANOI =====\n"
                "1. Watch the recursive solution\n"
                "2. Play it yourself\n"
                "3. Minimum-moves table\n"
                "4. Exit\n";
        int choice = readInt("Choose: ", 1, 4);
        if (choice == 1) autoSolve(readInt("Number of disks (1-10): ", 1, 10), true);
        else if (choice == 2) playYourself();
        else if (choice == 3) showFormula();
        else break;
    }
    return 0;
}
