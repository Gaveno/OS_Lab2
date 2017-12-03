/*
 * Queue Class
 * Uses double linked list
 * Author: Gavin Atkin
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__
 
#include "dlinklist.h"

template <typename T>
class Queue {
public:
    Queue( );
    Queue(const Queue<T>& copyfrom);
    Queue<T>& operator=(const Queue<T>& copyfrom);
    Queue<T>& operator=(const DLinkedList<T>& copyfrom);
    
    // Add value to end of list (or insert based on compare function).
    void enqueue(const T& value, bool (*compare_function)(const T& left, const T& right) = nullptr);
    // Return front value and remove it from queue.
    T dequeue( );
    // Get front value without removing from queue.
    T& getFront( );
    
    // Get the size of the queue.
    unsigned int getSize( ) {
        return myList.getSize( );
    }
    
    // Get the length (same as size) of queue.
    unsigned int getLength( ) {
        return getSize( );
    }
    
    // Sort the queue based on a comparison function.
    unsigned int sort(bool (*greater_function)(const T& left, const T& right)) {
        return myList.sort(greater_function);
    }
    
    // Double linked list for holding queue values.
    DLinkedList<T> myList;
};

template <typename T>
Queue<T>::Queue( ) {
    
}

template <typename T>
Queue<T>::Queue(const Queue<T>& copyfrom) {
    myList = copyfrom.myList;
}

template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& copyfrom) {
    myList = copyfrom.myList;
    return *this;
}

template <typename T>
Queue<T>& Queue<T>::operator=(const DLinkedList<T>& copyfrom) {
    myList = copyfrom;
    return *this;
}

template <typename T>
void Queue<T>::enqueue(const T& value, bool (*compare_function)(const T& left, const T& right)) {
    if (compare_function == nullptr)
        myList.addBack(value);
    else
        myList.insertCompare(value, compare_function);
}

template <typename T>
T Queue<T>::dequeue( ) {
    T value = myList.getFront( );
    myList.removeFront( );
    return value;
}

template <typename T>
T& Queue<T>::getFront( ) {
    return myList.getFront( );
}

#endif