/*  Double Linked List
 *  container class
 *  templated.
 *
 *  Author: Gavin Atkin
 */

#ifndef __DLINKLIST_H__
#define __DLINKLIST_H__

#include <iostream>

using namespace std;

#define NO_INDEX_ERROR -1

template <typename T>
struct Node {
    Node<T>* next;
    Node<T>* prev;
    T value;
};

typedef unsigned int indextype;

class EXOutOfBounds {
public:
    EXOutOfBounds(indextype size, indextype input) : __size(size), __input(input) {};
    friend ostream& operator<<(ostream& os, EXOutOfBounds ex);
    
protected:
    indextype __size;
    indextype __input;
};

ostream& operator<<(ostream& os, EXOutOfBounds ex) {
    os << "\n\nError: An out of bounds exception has occurred.\n";
    os << "Size available: " << ex.__size << " input value: " << ex.__input << "\n\n";
    return os;
}

template <typename T>
class DLinkedList {
public:
    // Constructors and assignments.
	DLinkedList( );
    DLinkedList<T>& operator=(const DLinkedList<T>& copyfrom);
    DLinkedList(const DLinkedList<T>& copyfrom);
    
    // Destructor. Clean up list.
    ~DLinkedList( );
    
    // Operations on list.
    // Clear entire list.
    void clear( );
    
    // Insertions and add.
    void insert(indextype position, T value);
    void insertFront(T value);
    void addBack(T value);
    void insertCompare(T value, bool (*compare_function)(const T& left, const T& right));
    
    // Deletion
    void remove(indextype position);
    void removeFront( );
    void removeBack( );
    
    // Accessors and search.
    
    // Non-constant.
    T& getValue(indextype position);
    T& operator[](indextype position);
    T& getFront( );
    T& getBack( );
    
    // Sorting the list.
    indextype sort(bool (*less_function)(const T& left, const T& right));
    
    // Constant.
    // Get the value at a certain position index.
    const T& getValue(indextype position) const;
    // Get the value at a certain position index.
    const T& operator[](indextype position) const;
    // Get the value at the front of the list.
    const T& getFront( ) const;
    // Get the value at the back of the list.
    const T& getBack( ) const;
    // Find returns the index or NO_INDEX_ERROR (-1)
    long int findFirst(const T&  value) const;
    // Find the nth value in the list or NO_INDEX_ERROR if not found.
    long int findNth(const T&  value, indextype n) const;
    // Get the size of the list.
    indextype getSize( ) const;
    
    // Printing out contents.
    template <typename S>
    friend ostream& operator<<(ostream &os, const DLinkedList<S>& list);
    
protected:
    Node<T>* headptr;
    Node<T>* tailptr;
    indextype size;
    
    void swap(T* left, T* right) {
        T temp = *left;
        *left = *right;
        *right = temp;
    }
};

/*---------------------------------------------
 *              IMPLEMENTATIONS
 *---------------------------------------------*/

template <typename T>
DLinkedList<T>::DLinkedList( ) {
    headptr = nullptr;
    tailptr = nullptr;
    size = 0;
}

template <typename T>
DLinkedList<T>::~DLinkedList( ) {
    clear( );
}

template <typename T>
DLinkedList<T>& DLinkedList<T>::operator=(const DLinkedList& copyfrom) {
    // Clear any previous items in list before copy
    clear( );
    // Copy all elements
    for (indextype i = 0; i < copyfrom.size; ++i) {
        addBack(copyfrom[i]);
    }
    return *this;
}
 
template <typename T>
DLinkedList<T>::DLinkedList(const DLinkedList<T>& copyfrom) : DLinkedList( ) {
   operator=(copyfrom);
}

template <typename T>
void DLinkedList<T>::clear( ) {
    while (headptr != nullptr) {
        removeFront( );
    }
}

// Insertion of elements
// Inserts at position, or end of list. Whichever comes first.
template <typename T>
void DLinkedList<T>::insert(indextype position, T value) {
    // Is this the first value of the list?
    if (headptr == nullptr) {
        headptr = new Node<T>;
        tailptr = headptr;
        headptr->prev = nullptr;
        headptr->next = nullptr;
        headptr->value = value;
    }
    else if (position == 0) {
        // Special case for position in front
        Node<T>* nn = new Node<T>;
        nn->value = value;
        nn->next = headptr;
        headptr->prev = nn;
        nn->prev = nullptr;
        headptr = nn;
    }
    else {
        // Not first in list
        // start at head
        Node<T>* conductor = headptr;
        // go through to one less than index or back of list
        indextype i = 0;
        for (; i < position - 1; ++i) {
            if (conductor->next != nullptr) {
                conductor = conductor->next;
            }
            else {
                break;
            }
        }
        // place in front of current conductor location
        Node<T>* prev = conductor;
        Node<T>* next = conductor->next;
        conductor = new Node<T>;
        conductor->prev = prev;
        conductor->next = next;
        conductor->prev->next = conductor;
        conductor->next->prev = conductor;
        conductor->value = value;
        if (next == nullptr) {
            tailptr = conductor;
        }
    }
    ++size;
}

template <typename T>
void DLinkedList<T>::insertFront(T value) {
    insert(0, value);
}

template <typename T>
void DLinkedList<T>::addBack(T value) {
    if (tailptr == nullptr) {
        // first value in list
        tailptr = new Node<T>;
        tailptr->next = nullptr;
        tailptr->prev = nullptr;
        tailptr->value = value;
        headptr = tailptr;
    }
    else {
        // Not first, put at end
        Node<T>* nn = new Node<T>;
        nn->prev = tailptr;
        nn->next = nullptr;
        nn->value = value;
        tailptr->next = nn;
        tailptr = nn;
    }
    ++size;
}

template <typename T>
void DLinkedList<T>::insertCompare(T value, bool (*compare_function)(const T& left, const T& right)) {
    indextype i;
    for (i = 0; i < getSize(); ++i) {
        if ((*compare_function)(getValue(i), value)) {
            insert(i, value);
            break;
        }
    }
    if (i == getSize()) {
        addBack(value);
    }
}

// Deletion of elements
// Deletes index or does nothing if index is out of bounds.
template <typename T>
void DLinkedList<T>::remove(indextype position) {
    // Exit if nothing to do.
    if (position >= size)
        return;
    
    indextype i = 0;
    Node<T>* conductor = headptr;
    while (i != position) {
        // Run off train if index out of bounds
        // for saftey.
        if (conductor != nullptr) {
            conductor = conductor->next;
        }
        ++i;
    }

    if (conductor != nullptr) {
        // conductor is still in bounds
        if (conductor->prev != nullptr) {
            conductor->prev->next = conductor->next;
        }
        if (conductor->next != nullptr) {
            conductor->next->prev = conductor->prev;
        }
        if (headptr == conductor) {
            headptr = conductor->next;
        }
        delete conductor;
        --size;
        if (size == 0) {
            headptr = nullptr;
            tailptr = nullptr;
        }
    }
}

template <typename T>
void DLinkedList<T>::removeFront( ) {
    remove(0);
}

template <typename T>
void DLinkedList<T>::removeBack( ) {
    remove(size - 1);
}

// Accessors and search.
// non-constant
template <typename T>
T& DLinkedList<T>::getValue(indextype position) {
    if (position >= size) {
        cout << "error: position: " << position << " size: " << size << endl;
        throw EXOutOfBounds(size, position);
    }
    
    // Shortest from front
    if (position < size / 2) {
        Node<T>* conductor = headptr;
        for (indextype i = 0; i < position; ++i) {
            conductor = conductor->next;
        }
        return conductor->value;
    }
    else {
        // Shortest from back
        Node<T>* conductor = tailptr;
        for (indextype i = size - 1; i > position; --i) {
            conductor = conductor->prev;
        }
        return conductor->value;
    }
}


// Sort using a bubblesort algorithm (since speed here isn't really an issue).
template <typename T>
indextype DLinkedList<T>::sort(bool (*greater_function)(const T& left, const T& right)) {
    int i, j;
    indextype num_swaps = 0;
    for (i = 0; i < getSize() - 1; ++i) {
        for (j = 0; j < getSize() - i - 1; ++j) {
            if ((*greater_function)(getValue(j), getValue(j + 1))) {
                swap(&(getValue(j)), &(getValue(j + 1)));
                num_swaps++;
            }
        }
    }
    //cout << "list sort, num swaps: " << num_swaps << endl;
    return num_swaps;
}

template <typename T>
T& DLinkedList<T>::operator[](indextype position) {
    return getValue(position);
}

template <typename T>
T& DLinkedList<T>::getFront( ) {
    return getValue(0);
}

template <typename T>
T& DLinkedList<T>::getBack( ) {
    return getValue(size - 1);
}

// constant
template <typename T>
const T& DLinkedList<T>::getValue(indextype position) const {
    if (position >= size) throw EXOutOfBounds(size, position);
    
    // Shortest from front
    if (position < size / 2) {
        Node<T>* conductor = headptr;
        for (indextype i = 0; i < position; ++i) {
            conductor = conductor->next;
        }
        return conductor->value;
    }
    else {
        // Shortest from back
        Node<T>* conductor = tailptr;
        for (indextype i = size - 1; i > position; --i) {
            conductor = conductor->prev;
        }
        return conductor->value;
    }
}

template <typename T>
const T& DLinkedList<T>::operator[](indextype position) const {
    return getValue(position);
}

template <typename T>
const T& DLinkedList<T>::getFront( ) const {
    return getValue(0);
}

template <typename T>
const T& DLinkedList<T>::getBack( ) const {
    return getValue(size - 1);
}

// Find returns the index or NO_INDEX_ERROR (-1)
// If n > the number of occurances, returns the greatest occurance.
// Will return NO_INDEX_ERROR if n == 0;
template <typename T>
long int DLinkedList<T>::findNth(const T& value, indextype n) const {
    indextype in = 0;
    indextype i = NO_INDEX_ERROR;
    Node<T>* conductor = headptr;
    while (conductor != nullptr && i < n) {
        if (conductor->value == value) {
            ++in;
        }
        conductor = conductor->next;
        ++i;
    }
    
    return i;
}

template <typename T>
long int DLinkedList<T>::findFirst(const T& value) const {
    return findNth(value, 1);
}

template <typename T>
indextype DLinkedList<T>::getSize( ) const {
    return size;
}


// printing
template <typename T>
ostream& operator<<(ostream& os, const DLinkedList<T>& list) {
    Node<T>* conductor = list.headptr;
    while (conductor != nullptr) {
        os << conductor->value << ", ";
        conductor = conductor->next;
    }
    return os;
}

#endif
