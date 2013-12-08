#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

struct dllist
{
    int length;
    struct node* head;
    struct node* tail;
};

struct node
{
    int data;
    struct node* next;
    struct node* prev;
};

struct args
{
    struct dllist newList;
    struct dllist list;
};

struct funcs
{
    struct node* (*getn)(struct node*);
    void (*setn)(struct node*, struct node*);
    void (*setp)(struct node*, struct node*);
    int (*cmp)(struct node*, struct node*);
};

void printList(struct dllist list)
{
    struct node* current = list.head;
    if(current == NULL)
    {
        printf("Empty list\n");
    }
    else
    {
        while(current->next != NULL)
        {
            printf("%d, ", current->data);
            current = current->next;
        }
        printf("%d\n", current->data);
    }
}

void printListB(struct dllist list)
{
    struct node* current = list.tail;
    if(current == NULL)
    {
        printf("Empty list\n");
    }
    else
    {
        while(current->prev != NULL)
        {
            printf("%d, ", current->data);
            current = current->prev;
        }
        printf("%d\n", current->data);
    }
}

struct dllist empty()
{
    struct dllist newList;
    newList.length = 0;
    newList.head = NULL;
    newList.tail = NULL;

    return newList;
}

void prepend(struct dllist* list, int data)
{
    struct node* newNode = malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = list->head;
    newNode->prev = NULL;

    if(list->head)
        list->tail = newNode;
    else
        list->head->prev = newNode;

    list->head = newNode;
    list->length++;
}

void append(struct dllist* list, int data)
{
    struct node* newNode = malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = list->tail;

    if(list->tail)
        list->head = newNode;
    else
        list->tail->next = newNode;

    list->tail = newNode;
    list->length++;
}

void prependNode(struct dllist* list, struct node* n)
{
    n->next = list->head;
    n->prev = NULL;

    if(list->head)
        list->tail = n;
    else
        list->head->prev = n;

    list->head = n;
    list->length++;
}

void appendNode(struct dllist* list, struct node* n)
{
    n->next = NULL:
    n->prev = list->tail;

    if(list->tail)
        list->head = n;
    else
        list->tail->next = n;

    list->tail = n;
    list->length++;
}

void* copyListHead(void* arg)
{
    struct args* headLists = (struct args*)arg;
    struct dllist list = headLists->list;

    struct dllist headList;
    headList.length = 0;
    headList.head = NULL;
    headList.tail = NULL;

    struct node* current = list.head;
    int i = 1;
    int end = list.length/2;

    append(&headList, current->data);
    while(i < end)
    {
        current = current->next;
        append(&headList, current->data);
        i++;
    }

    headLists->newList = headList;
}

void* copyListTail(void* arg)
{
    struct args* tailLists = (struct args*)arg;
    struct dllist list = tailLists->list;

    struct dllist tailList;
    tailList.length = 0;
    tailList.head = NULL;
    tailList.tail = NULL;

    struct node* current = list.tail;
    int i = 1;
    int end = (list.length + 1)/2;

    prepend(&tailList, current->data);
    while(i < end)
    {
        current = current->prev;
        prepend(&tailList, current->data);
        i++;
    }

    tailLists->newList = tailList;
}

struct dllist copyList(struct dllist list)
{
    struct dllist newList;

    if(list.length == 0)
    {
        newList.length = 0;
        newList.head = NULL;
        newList.tail = NULL;
    }
    else if(list.length == 1)
    {
        newList.length = 1;
        newList.head = NULL;
        newList.tail = NULL;
        append(&newList, list.head->data);
    }
    else
    {
        pthread_t pth;

        struct args headLists;
        headLists.list = list;
        struct args tailLists;
        tailLists.list = list;

        pthread_create(&pth, NULL, copyListHead, &headLists);
        copyListTail(&tailLists);

        pthread_join(pth, NULL);

        struct dllist headList = headLists.newList;
        struct dllist tailList = tailLists.newList;
        headList.tail->next = tailList.head;
        tailList.head->prev = headList.tail;

        newList.head = headList.head;
        newList.tail = tailList.tail;
        newList.length = list.length;
    }

    return newList;
}

struct dllist buildList(int length)
{
    struct dllist list = empty();

    int i;
    for(i = 1; i <= length; i++)
    {
        append(&list, 1000*sin(i*i));
    }

    return list;
}

int compare(struct node* a, struct node* b)
{
    return a->data - b->data;
}

int compareNeg(struct node* a, struct node* b)
{
    return b->data - a->data;
}

struct node* getNext(struct node* current)
{
    return current->next;
}

struct node* getPrev(struct node* current)
{
    return current->prev;
}

void setNext(struct node* current, struct node* next)
{
    current->next = next;
}

void setPrev(struct node* current, struct node* prev)
{
    current->prev = prev;
}

struct funcs fwdFuncs()
{
    struct funcs f;
    f.getn = getNext;
    f.setn = setNext;
    f.setp = setPrev;
    f.cmp = compare;

    return f;
}

struct funcs bwdFuncs()
{
    struct funcs f;
    f.getn = getPrev;
    f.setn = setPrev;
    f.setp = setNext;
    f.cmp = compareNeg;

    return f;
}

int merge(struct dllist* list, struct node* start, int length, int partSize,  struct funcs f)
{
    int qpos, qsize, psize;
    qpos = 0;

    struct node *head, *tail, *p, *q, *toAdd;
    p = start;
    head = NULL;
    tail = NULL;

    while(qpos < length)
    {
        q = p;
        psize = 0;
        while(psize < partSize && qpos < length)
        {
            q = f.getn(q);
            qpos++;
            psize++;
        }

        qsize = partSize;     // although it might be smaller
        while(psize > 0 || (qsize > 0 && qpos < length))
        {
            if(psize == 0)
            {
                toAdd = q;
                q = f.getn(q);
                qpos++;
                qsize--;
            } else if(qsize == 0 || qpos >= length || f.cmp(p, q) <= 0)
            {
                toAdd = p;
                p = f.getn(p);
                psize--;
            } else
            {
                toAdd = q;
                q = f.getn(q);
                qpos++;
                qsize--;
            }

            if(tail)
            {
                f.setn(tail, toAdd);
                f.setp(toAdd, tail);
            } else
            {
                head = toAdd;
                f.setp(head, NULL);
            }
            tail = toAdd;
        }
        p = q;
    }
    f.setn(tail, NULL);
    l->head = head;
    l->tail = tail;
}

void mergesort(struct dllist* list, int sectionLength, int partSize)
{
    do
    {
        partSize *= 2;
        merge(list, list->head, sectionLength, partSize, fwdFuncs());
    } while(partSize < sectionLength);
}

void addReveredAndFlip(struct dllist* list, struct dllist* reversed)
{
    // Add reversed
    list->tail->next = reversed->tail;
    reversed->tail->prev = list->tail;
    list->tail = reversed->head;

    // Flip
    reversed->head = list->tail->next;
    reversed->tail = list->tail;
}

void add(struct dllist* front, struct dllist back)
{
    front->tail->next = back.head;
    back.head->prev = front->tail;
    front->tail = back.tail;
}

void sortRecur(struct dllist* list, int sectionLength, int partSize)
{
    if(sectionLength < 5 && -5 < sectionLength)
    {
        mergesort(list, sectionLength, partSize);
    }

    struct dllist front, back;

    front.head = list->head;
    front.length = sectionLength/2;
    back.tail = list->tail;
    back.length = (sectionLength + 1)/2;

    merge(&front, front.head,  front.length, partSize*2, fwdFuncs());
    merge(&back, back.tail, back.length, partSize*2, bwdFuncs());
    addReversedAndFlip(&front, &back);

    sortRecur(&front, sectionLength/2, partSize*2);
    sortRecur(&back, sectionLength/2, partSize*2);

    add(&front, back);
    *list = front;
}

void sort(struct dllist* list)
{
    sortRecur(list, list->length, 1);

    struct dllist listB = *list;

    sortHalf(list, list->length/2);
    sortHalf(&listB, -(list->length + 1)/2);

    list->tail->next = listB.tail;
    listB.tail->prev = list->tail;
    list->tail = listB.head;

    merge(list, list->head, list->length, list->length/2, fwdFuncs());
}

int main(void)
{
    struct dllist list = buildList(8);

    sort(&list);
    printList(list);
    printListB(list);

    return 0;
}
