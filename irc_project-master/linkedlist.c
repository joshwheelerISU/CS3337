#include <stdio.h>
#include <stdlib.h>

//Linked List for use in the IRC project. Implemented using help from geeksforgeeks.org/generic-linked-list-in-c-2/.
//Joshua Wheeler, 2020. 



struct Node
{

	//void pointer to whatever we want to store in this node
	void *data;
	
	//next node in the list
	struct Node* next;

};

void push(struct Node** listHead, void *data, size_t size)
{


	struct Node *newNode;
	newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->next = NULL;
	newNode->data = malloc(size);



	//byte-for-byte copy the data 
	
	int i;
	for(i=0; i<size; i++)
	{
		*(char *)(newNode->data + i) = *(char *)(data + i);
	}

	//put the node at the end of the list
	
	struct Node *checkNode = listHead;
	
	while(checkNode->next != NULL)
	{
	
		checkNode = checkNode->next;
	
	}
	
	checkNode->next = newNode;
	
	
}

void delete(struct Node** listHead, struct Node *mFD)
{


	struct Node *checkNode = listHead;
	
	while(checkNode->next != mFD || checkNode->next == NULL)
	{
	
		checkNode = checkNode->next;
	
	}
	
	struct Node *previousNode = checkNode;
	
	checkNode = checkNode->next;
	
	if (checkNode != NULL)
	{
	
		previousNode->next = checkNode->next;
		checkNode->next = NULL;
		free(checkNode->data);
	
	}


}
