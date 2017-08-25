/*
 * CS 261 Assignment 5
 * Name: Sean Solberg 
 * Date: 8-5-2017
 */

#include "dynamicArray.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Loads into heap a list from a file with lines formatted like
 * "priority, name".
 * @param heap
 * @param file
 */
void listLoad(DynamicArray* heap, FILE* file)
{
    const int FORMAT_LENGTH = 256;
    char format[256];
    snprintf(format, FORMAT_LENGTH, "%%d, %%%d[^\n]", TASK_NAME_SIZE);
    
    Task temp;
    while (fscanf(file, format, &temp.priority, &temp.name) == 2)
    {
        dyHeapAdd(heap, taskNew(temp.priority, temp.name), taskCompare);
    }
}

/**
 * Writes to a file all tasks in heap with lines formatted like
 * "priority, name".
 * @param heap
 * @param file
 */
void listSave(DynamicArray* heap, FILE* file)
{
    for (int i = 0; i < dySize(heap); i++)
    {
        Task* task = dyGet(heap, i);
        fprintf(file, "%d, %s\n", task->priority, task->name);
    }
}

/**
 * Prints every task in heap.
 * @param heap
 */
void listPrint(DynamicArray* heap)
{
    DynamicArray* temp = dyNew(1);
    dyCopy(heap, temp);
     while (dySize(temp) > 0)
    {
        Task* task = dyHeapGetMin(temp);
        printf("\n");
        taskPrint(task);
        printf("\n");
        dyHeapRemoveMin(temp, taskCompare);
    }
    dyDelete(temp);
}

/**
 * Handles the given command.
 * @param list
 * @param command
 */
void handleCommand(DynamicArray* list, char command)
{
	char taskName[20];
	char fileName[20];
	int prior;
	prior = 0;
	int count;
	count = 0;

	switch (command) {
		case 'a':
			//get task name
			printf("Please enter the task description: ");
			scanf("%[a-zA-Z ]", taskName);

			//get priority
			printf("Please enter the task priority (0-999): ");
			scanf("%d", &prior);
			while (getchar() != '\n'); //gets rid of 'enter' in buffer

			//create task, add to heap, and sort
			count = dySize(list);
			
			struct Task *tempTask;
			tempTask = taskNew(prior, taskName);
			dyHeapAdd(list, tempTask, taskCompare);

			//print message if added successfully
			if (dySize(list) == count + 1) 
			{
				printf("The task '%s' has been added to your to - do list.", taskName);
				printf("\n");
			}

			break;
		case 'g':
			//get min task; check first to see if list is empty
			
			if (dySize(list) == 0) 
			{
				printf("Your to-do list is empty!\n");
			}
			else
			{
				struct Task *getMin = dyHeapGetMin(list);
				printf("Your first task is: %s \n", getMin->name);
			}
			break;
		case 'p':
			//print list; check first to see if list is empty
			if (dySize(list) == 0)
			{
				printf("Your to-do list is empty!\n");
			}
			else
			{
				listPrint(list);
			}
			break;
		case 'l': //load from file

			printf("Please enter the filename: ");
			scanf("%s", fileName);
			while (getchar() != '\n'); //gets rid of 'enter' in buffer

			FILE *fp =  fopen(fileName, "r");
			if (fp == 0) 
			{
				printf("Error loading file\n");
			}
			else
			{
				printf("File loaded sucessfully.\n");
				listLoad(list, fp);
				fclose(fp);
			}

			break;
		case 's': //save to file
			printf("Please enter the filename: ");
			scanf("%s", fileName);
			while (getchar() != '\n'); //gets rid of 'enter' in buffer
			
			//File I/O 
			FILE *fw = fopen(fileName, "w");
			if (fw == 0) {
				printf("Error saving");
			}
			else
			{
				printf("The list has been saved into the file successfully.\n");
				listSave(list, fw);
				fclose(fw);
			}

			break;
		case 'r': //remove min
			if (dySize(list) > 0)
			{
				struct Task *temp = dyHeapGetMin(list);
				dyHeapRemoveMin(list, taskCompare);
				taskDelete(temp);
			}
			else
				printf("List is empty.\n");
				printf("\n");
			break;
		case 'e':
			//end program
			printf("Program Ending\n");
			break;
		default: 
			break;
	}
}

int main()
{
    // Implement
    printf("\n\n** TO-DO LIST APPLICATION **\n\n");
    DynamicArray* list = dyNew(8);
    char command = ' ';
    do
    {
        printf("Press:\n"
               "'l' to load to-do list from a file\n"
               "'s' to save to-do list to a file\n"
               "'a' to add a new task\n"
               "'g' to get the first task\n"
               "'r' to remove the first task\n"
               "'p' to print the list\n"
               "'e' to exit the program\n"
        );
        command = getchar();
        // Eat newlines
        while (getchar() != '\n');
        handleCommand(list, command);
    }
    while (command != 'e');
 /* free dynamically allocated List pointers in array to avoid memory leaks */
         /* Fix it */
	int index;
	for (index = 0; index < dySize(list); index++)
	{
		struct Task *delTask = dyGet(list, index);
		taskDelete(delTask);
	}

    dyDelete(list);
    return 0;
}
