/*
 * CS 261 Assignment 5
 * Name: Sean Solberg
 * Date: 8-3-2017
 */

#include "dynamicArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TESTING

#ifndef TESTING
static void adjustHeap(DynamicArray* heap, int last, int position, compareFunction compare);
static void buildHeap(DynamicArray* heap, compareFunction compare);
#endif

struct DynamicArray
{
    TYPE* data;
    int size;
    int capacity;
};

// --- Dynamic array ---

static void setCapacity(DynamicArray* array, int capacity)
{
    TYPE* data = malloc(sizeof(TYPE) * capacity);
    for (int i = 0; i < array->size; i++)
    {
        data[i] = array->data[i];
    }
    free(array->data);
    array->data = data;
    array->capacity = capacity;
}

static void init(DynamicArray* array, int capacity)
{
    assert(capacity > 0);
    array->data = NULL;
    array->size = 0;
    setCapacity(array, capacity);
}

DynamicArray* dyNew(int capacity)
{
    DynamicArray* array = malloc(sizeof(DynamicArray));
    init(array, capacity);
    return array;
}

void dyDelete(DynamicArray* array)
{
    free(array->data);
    free(array);
}

void dyAdd(DynamicArray* array, TYPE value)
{
    if (array->size >= array->capacity)
    {
        setCapacity(array, 2 * array->capacity);
    }
    array->data[array->size] = value;
    array->size++;
}

void dyAddAt(DynamicArray* array, TYPE value, int position)
{
    assert(position <= array->size);
    dyAdd(array, value);
    for (int i = array->size - 1; i > position; i--)
    {
        dySwap(array, i, i - 1);
    }
}

void dyPut(DynamicArray* array, TYPE value, int position)
{
    assert(position < array->size);
    array->data[position] = value;
}

void dyRemoveAt(DynamicArray* array, int position)
{
    assert(position < array->size);
    for (int i = position; i < array->size - 1; i++)
    {
        array->data[position] = array->data[position + 1];
    }
    array->size--;
}

TYPE dyGet(DynamicArray* array, int position)
{
    assert(position < array->size);
    return array->data[position];
}

int dySize(DynamicArray* array)
{
    return array->size;
}

void dySwap(DynamicArray* array, int position1, int position2)
{
    assert(position1 < array->size);
    assert(position2 < array->size);
    TYPE temp = array->data[position1];
    array->data[position1] = array->data[position2];
    array->data[position2] = temp;
}

// --- Stack ---

void dyStackPush(DynamicArray* stack, TYPE value)
{
    dyAdd(stack, value);
}

void dyStackPop(DynamicArray* stack)
{
    dyRemoveAt(stack, stack->size - 1);
}

TYPE dyStackTop(DynamicArray* stack)
{
    return dyGet(stack, stack->size - 1);
}

int dyStackIsEmpty(DynamicArray* stack)
{
    return stack->size == 0;
}

// --- Bag ---

static int findFirst(DynamicArray* array, TYPE value, compareFunction compare)
{
    for (int i = 0; i < array->size; i++)
    {
        if (compare(value, array->data[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void dyBagAdd(DynamicArray* bag, TYPE value)
{
    dyAdd(bag, value);
}

void dyBagRemove(DynamicArray* bag, TYPE value, compareFunction compare)
{
    int position = findFirst(bag, value, compare);
    if (position != -1)
    {
        dyRemoveAt(bag, position);
    }
}

int dyBagContains(DynamicArray* bag, TYPE value, compareFunction compare)
{
    return findFirst(bag, value, compare) != -1;
}

// --- Ordered bag ---

static int binarySearch(DynamicArray* array, TYPE value, compareFunction compare)
{
    int low = 0;
    int high = array->size - 1;
    while (low <= high)
    {
        int middle = (low + high) / 2;
        if (compare(value, array->data[middle]) < 0)
        {
            high = middle - 1;
        }
        else if (compare(value, array->data[middle]) > 0)
        {
            low = middle + 1;
        }
        else
        {
            return middle;
        }
    }
    return low;
}

void dyOrderedAdd(DynamicArray* bag, TYPE value, compareFunction compare)
{
    int position = binarySearch(bag, value, compare);
    dyAddAt(bag,value, position);
}

void dyOrderedRemove(DynamicArray* bag, TYPE value, compareFunction compare)
{
    int position = binarySearch(bag, value, compare);
    if (compare(value, bag->data[position]) == 0)
    {
        dyRemoveAt(bag, position);
    }
}

int dyOrderedContains(DynamicArray* bag, TYPE value, compareFunction compare)
{
    int position = binarySearch(bag, value, compare);
    return compare(value, dyGet(bag, position)) == 0;
}

// --- Heap ---

/**
 * Adjusts heap to maintain the heap property.
 * @param heap
 * @param last  index to adjust up to.
 * @param position  index where adjustment starts.
 * @param compare  pointer to compare function.
 */
void adjustHeap(DynamicArray* heap, int last, int position, compareFunction compare)
{
	int rtIdx;
	int lfIdx;
	lfIdx = position * 2 + 1;
	rtIdx = position * 2 + 2;

	//have 2 child nodes
	if (rtIdx < last) 
	{
		//get index of smallest child
		int small;
		int smallIndex;
		small = (*compare)(heap->data[lfIdx], heap->data[rtIdx]);
		if (small == -1) //left is smaller
		{
			smallIndex = lfIdx;
		}
		else smallIndex = rtIdx;

		//compare smallest child to current position and swap if 
		if (heap->data[position] > heap->data[smallIndex]) 
		{
			//swap value of parent for value of child
			dySwap(heap, position, smallIndex);

			//set new position as starting point for next iteration
			adjustHeap(heap, last, smallIndex, compare);
		}
	}
	//only have 1 child node
	else if (lfIdx < last) 
	{
		//compare smallest child to current position and swap if 
		if (heap->data[position] > heap->data[lfIdx])
		{
			//swap value of parent for value of child
			dySwap(heap, position, lfIdx);

			//set new position as starting point for next iteration
			adjustHeap(heap, last, lfIdx, compare);
		}
	}
	
	//none. at bottom of tree
	else return;
}

/**
 * Builds a valid heap from an arbitrary array.
 * @param heap  array with elements in any order.
 * @param compare  pointer to compare function.
 */
void buildHeap(DynamicArray* heap, compareFunction compare)
{
	//find last non-leaf node
	int max;
	max = dySize(heap);
	int i;
	i = max / 2 - 1;

	//loop from last non-leaf node to root calling adjust heap 
	int index;
	for (index = i; index >= 0; index--) 
	{
		adjustHeap(heap, max, index, compare);
	}

}

/**
 * Adds an element to the heap.
 * @param heap
 * @param value  value to be added to heap.
 * @param compare  pointer to compare function.
 */
void dyHeapAdd(DynamicArray* heap, TYPE value, compareFunction compare)
{
	//function adds new value to heap and then percolates up to find ultimate position
	//code adopted from lecture notes

	int parent;
	int pos = dySize(heap);
	dyAdd(heap, value); //add to array and resize if needed

	while (pos != 0) 
	{
		parent = (pos - 1) / 2;
		//compare current position to parent and swap if needed
		if ( (*compare)(dyGet(heap, pos), dyGet(heap, parent) ) == -1)
		{
			//swap and re-set pos
			dySwap(heap, parent, pos);
			pos = parent;
		}
		else return;
	}
}

/**
 * Removes the first element, which has the min priority, form the heap.
 * @param heap
 * @param compare  pointer to the compare function.
 */
void dyHeapRemoveMin(DynamicArray* heap, compareFunction compare)
{
	//remove last element and put in first position, then adjust heap
	int last;
	assert(dySize(heap) != 0);
	last = dySize(heap) - 1;

	//put last element into first element
	dyPut(heap, dyGet(heap, last), 0);

	//remove last element from heap and adjust
	dyRemoveAt(heap, last);
	adjustHeap(heap, last, 0, compare);
}

/**
 * Returns the first element, which has the min priority, from the heap.
 * @param heap
 * @return  Element at the top of the heap.
 */
TYPE dyHeapGetMin(DynamicArray* heap)
{
	assert(dySize(heap) != 0);
	return  dyGet(heap, 0);
}

/**
 * Sorts arbitrary array in-place.
 * @param heap  array with elements in arbitrary order.
 * @param compare  pointer to the compare function.
 */
void dyHeapSort(DynamicArray* heap, compareFunction compare)
{	
	//assert
	assert(dySize(heap) != 0);
	//build heap
	buildHeap(heap, compare);
	
	int i;
	int index;

	//find last node
	i = dySize(heap)-1;

	for (index = i; index > 0; index--)
	{
		//swap with min node
		dySwap(heap, 0, index);
		adjustHeap(heap, index, 0, compare);
	}

}

// --- Iterator ---

DynamicArrayIterator* dyIteratorNew(DynamicArray* array)
{
    DynamicArrayIterator* iterator = malloc(sizeof(DynamicArrayIterator));
    iterator->array = array;
    iterator->current = 0;
    return iterator;
}

void dyIteratorDelete(DynamicArrayIterator* iterator)
{
    free(iterator);
}

int dyIteratorHasNext(DynamicArrayIterator* iterator)
{
    return iterator->current < iterator->array->size;
}

TYPE dyIteratorNext(DynamicArrayIterator* iterator)
{
    TYPE value = dyGet(iterator->array, iterator->current);
    iterator->current++;
    return value;
}

void dyIteratorRemove(DynamicArrayIterator* iterator)
{
    iterator->current--;
    dyRemoveAt(iterator->array, iterator->current);
}

// --- Utility ---

void dyPrint(DynamicArray* array, printFunction print)
{
    printf("\nsize: %d\ncapacity: %d\n[\n", array->size, array->capacity);
    for (int i = 0; i < array->size; i++)
    {
        printf("%d : ", i);
        print(array->data[i]);
        printf("\n");
    }
    printf("]\n");
}

void dyCopy(DynamicArray* source, DynamicArray* destination)
{
    free(destination->data);
    init(destination, source->capacity);
    for (int i = 0; i < source->size; i++)
    {
        destination->data[i] = source->data[i];
    }
    destination->size = source->size;
}