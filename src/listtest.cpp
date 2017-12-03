#include <iostream>
#include "dlinklist.h"
#include "queue.h"

using namespace std;

int main() {
	Queue<int> a;
    Queue<int> b;
    Queue<int> c;
    
    for (int i = 0; i < 10; i++)
        a.enqueue(i);
    
    b = a;
    
    c = b;
    c.enqueue(20);
    
    cout << "a:\n";
    while (a.getSize() > 0) {
        cout << a.dequeue() << endl;
    }
    cout << "b:\n";
    while (b.getSize() > 0) {
        cout << b.dequeue() << endl;
    }
    cout << "c:\n";
    while (c.getSize() > 0) {
        cout << c.dequeue() << endl;
    }
    
    return 0;
}

