#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node;
typedef struct node* ptr;
typedef ptr Stack;
struct node
{
    char line [100];
    ptr next;
};

//create a stack
Stack createStack()
{
    Stack stk;
    stk=(Stack) malloc (sizeof(struct node));
    if(stk==NULL)
    {
        printf("Couldn't create stack. Out of memory.\n");
        return (Stack) -1;
    }
    stk->next=NULL;
    return stk;
}


//function to push a string into the stack
void push(Stack stk, char lineToAdd [100])
{
    ptr temp;
    temp = (ptr) malloc (sizeof(struct node));
    if(temp != NULL)
    {
        strcpy(temp->line, lineToAdd);
        temp->next=stk->next;
        stk->next=temp;
    }
    else
    {
        printf("Couldn't push the element. Out of memory\n");
    }
}


//function to check if the stack is empty
int isEmpty(Stack stk)
{
    return (stk->next == NULL);
}


//function to pop a string from the stack
void pop(Stack stk)
{
    ptr temp;
    if(isEmpty(stk))
    {
        printf("Stack is empty. Can't pop from empty stack\n");
    }
    else
    {
        temp = stk ->next;
        stk->next = stk ->next ->next;
        free(temp);
    }
}


//function that returns the top of the stack without popping it
char* Top (Stack stk)
{
    if(!isEmpty(stk))
    {
        return stk->next->line;
    }
    else
    {
        return NULL;
    }
}


//function to print the stack to the console
void printStack(Stack mainStack)
{
    Stack tempToPrint = createStack();
    printf("result >\n");
    if(isEmpty(mainStack))
    {
        printf("Stack is Empty !\n");
    }
    //pop all elements from the stack and save them to tempToPrint
    while(!isEmpty(mainStack))
    {
        push(tempToPrint, Top(mainStack));
        pop(mainStack);
    }

    //return all elements to the stack and print them while pushing them
    while(!isEmpty(tempToPrint))
    {
        printf("\t%s\n", Top(tempToPrint));
        push(mainStack, Top(tempToPrint));
        pop(tempToPrint);
    }
}


//function to undo the last operation done
void undo(Stack mainStack, Stack undoStack)
{
    //remove the line from the main stack and push it to the undoStack
    if(!isEmpty(mainStack))
    {
        char topLine [100];
        strcpy(topLine, Top(mainStack));
        push(undoStack, topLine);
        pop(mainStack);
        printStack(mainStack);
    }
    else
    {
        printf("Stack is empty. You can't undo\n");
    }

}


//function to redo the last operation done
void redo(Stack mainStack, Stack undoStack)
{
    //remove the line from the undo stack and push it to the mainStack
    if(!isEmpty(undoStack))
    {
        char topLine [100];
        strcpy(topLine, Top(undoStack));
        push(mainStack, topLine);
        pop(undoStack);
        printStack(mainStack);
    }
    else
    {
        printf("There are no lines to redo.\n");
    }
}


//function that saves
void saveToFile(Stack mainStack)
{
    FILE *fptr;
    fptr = fopen("output.txt", "w");
    fprintf(fptr, "result >\n");
    Stack tempToPrint = createStack();
    while(!isEmpty(mainStack))
    {
        push(tempToPrint, Top(mainStack));
        pop(mainStack);
    }
    while(!isEmpty(tempToPrint))
    {
        fprintf(fptr, "\t%s\n", Top(tempToPrint));
        push(mainStack, Top(tempToPrint));
        pop(tempToPrint);
    }
}




void menu()
{
    printf("You can enter any string to add it to the stack or you can enter one of the following commands:\n\n");
    printf("undo: this will undo (i.e., remove) that last entered statement\n\n");
    printf("redo: this will redo (i.e., re-add) that last removed statement\n\n");
    printf("print: this command will print the entire stored input text\n\n");
    printf("save: will save the text to a file called (output.txt)\n\n");
    printf("quit: Exit the program after saving the stack\n\n");
}


int main()
{
    Stack mainStack = createStack();
    Stack undoStack = createStack();
    printf("--------------------- Welcome ---------------------\n");
    menu();

    char enteredLine [100];
    char lowerEnteredLine [100];

    while(1)
    {
        printf("My Command > ");
        gets(enteredLine);



        strcpy(lowerEnteredLine, enteredLine);
        //to make the command PRINT for example valid
        for(int i=0; i<strlen(lowerEnteredLine); i++)
        {
            if(lowerEnteredLine[i]>=65 && lowerEnteredLine[i]<=90)
            {
                lowerEnteredLine[i]+=32;
            }
        }

        if(strcmp(lowerEnteredLine, "undo")==0)
        {
            undo(mainStack, undoStack);
        }
        else if (strcmp(lowerEnteredLine, "redo")==0)
        {
            redo(mainStack, undoStack);
        }
        else if(strcmp(lowerEnteredLine, "print")==0)
        {
            printStack(mainStack);
        }
        else if(strcmp(lowerEnteredLine, "save")==0)
        {
            saveToFile(mainStack);
        }
        else if(strcmp(lowerEnteredLine, "clear")==0)
        {
            saveToFile(mainStack);
        }
        else if(strcmp(lowerEnteredLine, "quit")==0)
        {
            saveToFile(mainStack);
            printf("result > ");
            printf("Thank you and good bye!\n");
            return 0;
        }
        else
        {
            push(mainStack, enteredLine);
        }
    }
    return 0;
}
