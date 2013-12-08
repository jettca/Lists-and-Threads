#include <stdio.h>
#include <stdlib.h>

struct node
{
    int data;
    struct node* next;
};

void pnum(int num)
{
    printf("%d\n", num);
}

void printList(struct node* head)
{
    if(head == NULL)
        return;

    struct node* current = head;
    while(current->next != NULL)
    {
        pnum(current->data);
        current = current->next;
    }
    pnum(current->data);
}

int length(struct node* head)
{
    int count = 0;
    struct node* current = head;

    while(current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}

void push(struct node** headRef, int data)
{
    struct node* newNode = malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = *headRef;
    *headRef = newNode;
}

struct node* build(int length)
{
    struct node* head = NULL;
    struct node** currentRef = &head;

    int i;
    for(i = 1; i <= length; i++)
    {
        push(currentRef, i);
        currentRef = &((*currentRef)->next);
    }

    return head;
}

struct node* append(struct node** headRef, int data)
{
    if(*headRef == NULL)
    {
        push(headRef, data);
    }
    else
    {
        struct node* current = *headRef;

        while(current->next != NULL)
        {
            current = current->next;
        }
        push(&(current->next), data);
    }
}

struct node* copyList(struct node* head)
{
    struct node* newHead = NULL;
    struct node** newRef = &newHead;

    struct node* current = head;
    while(current != NULL)
    {
        push(newRef, current->data);

        current = current->next;
        newRef = &((*newRef)->next);
    }

    return newHead;
}

int main(void)
{
    struct node* list = build(1);
    append(&list, -1);

    struct node* list2 = copyList(list);
    append(&list2, -100);

    printList(list);
    printList(list2);

    return 0;
}
