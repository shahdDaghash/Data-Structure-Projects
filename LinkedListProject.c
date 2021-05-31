/*
/ Shahd Abu-Daghash _ 1191448 _ 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX 300

struct Node;
typedef struct Node* ptr;
typedef ptr List;
typedef ptr position;


struct Node
{
    double coefficient;
    int exponent;
    ptr next;
    ptr prev;
};

//global variable that are used throughout the project
List allEquations [100];
int AllIndex=0;
List sumOfEquations;
List subOfEquations;
List mulEq;
List mulOfEquations;



//insert a node after position P in List L
void insert(List L, position P, double coeff, int power)
{
    position insertTemp;
    insertTemp = (position) malloc(sizeof(struct Node));
    if(insertTemp!=NULL)
    {
        insertTemp -> coefficient = coeff;
        insertTemp -> exponent = power;
        position after = P->next;
        if(after!=NULL) //if node is not then it has a previous and it is assigned to insertTemp
        {
            after->prev = insertTemp;
        }
        insertTemp -> next = after;
        P -> next = insertTemp;
        insertTemp -> prev = P;
    }
    else
    {
        printf("could not initiate Position. Out of memory\n");
        exit (1);
    }
}



//delete node at position P in list L
void deleteNode (List L, position P)
{
    if(P->prev !=NULL)
    {
        (P->prev)->next=P->next; //connect the previous node to the next node
    }
    if(P->next !=NULL)
    {
        (P->next)->prev=P->prev; //connect the next node to the previous node
    }
    else
    {
        (P->next)->prev = NULL;
    }
    free(P);
}



//Find the node that holds the specified power
ptr Find(int power, List L)
{

    position P;
    P = L->next;

    //loop until end of list
    while(P != NULL && P->exponent != power)
    {
        P = P->next;
    }

    return P;

}



//function to print the entered list to the console
void printList (List L)
{
    position P = L;
    P=P->next;
    printf("%.3f x^%d ", P->coefficient, P->exponent); //print the first node
    //the if statements are for the appearance of the term when power and coefficient equal 1 and/or 0
    while(P->next!=NULL)
    {
        P=P->next;
        if(P->coefficient>0)
        {
            if(P->exponent==0)
            {
                if(P->coefficient==1)
                {
                    printf("+ 1"); //1x^0
                }
                else
                {
                    printf("+ %.3f", P->coefficient);
                }
            }
            else if(P->exponent==1)
            {
                if(P->coefficient==1)
                {
                    printf("+ x ");
                }
                else
                {
                    printf("+ %.3fx ", P->coefficient);
                }
            }
            else
            {
                if(P->coefficient==1)
                {
                    printf("+ x^%d ", P->exponent);
                }
                else
                {
                    printf("+ %.3fx^%d ", P->coefficient, P->exponent);
                }
            }
        }
        else if (P->coefficient<0)
        {
            double coo= P->coefficient * -1;
            if(P->exponent==0)
            {
                if(coo==1)
                {
                    printf("- 1 ");
                }
                else
                {
                    printf("- %.3f ", coo);
                }
            }
            else if(P->exponent==1)
            {
                if(P->coefficient==1)
                {
                    printf("- x ");
                }
                else
                {
                    printf("- %.3fx ", P->coefficient);
                }
            }
            else
            {
                if(P->coefficient==1)
                {
                    printf("- x^%d ", P->exponent);
                }
                else
                {
                    printf("- %.3fx^%d ", coo, P->exponent);
                }
            }
        }
    }
    printf("\n\n");
}



//function that returns the first node that has a power greater than the existing one
ptr Findbigger(int power, List L)
{
    position P;
    P = L->next;
    position lastBigger;
    lastBigger = L->next;

    while(P != NULL && P->exponent > power)
    {
        lastBigger=P;
        P = P->next;
    }

    return lastBigger;

}



//function that deletes the whole list
void DeleteList(List L)
{
    position P, temp;
    P = L->next;
    L->next = NULL;

    while(P != NULL)
    {
        temp = P->next;
        free(P);
        P=temp;
    }
}



//function that empties the list by deleting it and creating it again
List MakeEmpty(List L)
{
    if(L != NULL)
        DeleteList( L );
    L=(List) malloc(sizeof(struct Node));
    if(L==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    L->next = NULL;
    L->prev = NULL;
    return L;

}



//read the equation from the file and save it to the linked lists
void Read()
{
    AllIndex=0;
    FILE *fptr;
    fptr = fopen("equations.txt", "r");

    char currentEq [MAX];

    //this loop is to read through the file line by line
    while (fgets(currentEq, MAX, fptr)!=NULL)
    {
        List L;
        L=(List) malloc(sizeof(struct Node));
        if(L==NULL)
        {
            printf("could not initiate list. Out of memory\n");
            exit (1);
        }
        L -> next = NULL;
        L->prev=NULL;
        position tail_L = L;

        allEquations[AllIndex]=L; //Saving L in the array
        AllIndex++;

        int i=0; //
        int isnegative=0; //if 1 it means that the operation is - not +


        if(strncmp(&(currentEq[i]),"-",1)==0) //check if the first index is negative sign
        {
            isnegative=1;
            i++;
        }


        while(strncmp(&(currentEq[i-1]),"\n",1)!=0 && strncmp(&(currentEq[i]),"n",1)!=0 && strncmp(&(currentEq[i-1]),"\0",1)!=0 && strncmp(&(currentEq[i]),"\0",1)!=0) //loop through the line
        {
            double coeff=0;
            int power=0;

            int ok0=0; //this variable is 1 when the entered coefficient is zero

            while(strncmp(&(currentEq[i])," ",1)==0) //skip the spaces
                i++;


            if(strncmp(&(currentEq[i]),"0",1)>=0 && strncmp(&(currentEq[i]),"9",1)<=0)//read the coefficient
            {
                coeff = atoi(&(currentEq[i]));
                int x=coeff;
                if(x==0)
                {
                    ok0=1; //number is entered as zero (this is to prevent it from handling the number and make it 1)
                    i++;
                }
                else
                {
                    while(x!=0) //skip until end of number (if number is 1111, 4 skips are made here)
                    {
                        i++;
                        x/=10;
                    }
                }
            }


            if(strncmp(&(currentEq[i]),".",1)==0)//if there is a fractional part
            {
                double rslen=0;
                i++;
                int rs=atoi(&(currentEq[i])); //right hand side number
                int x=rs; //copy the value into x
                while(x!=0) //skip the characters for the length of the number
                {
                    i++;
                    rslen++;
                    x/=10;
                }

                //add the number to the coefficient
                coeff*= pow(10,rslen);
                coeff+=(rs);
                coeff/=pow(10.0,rslen);
            }


            while(strncmp(&(currentEq[i])," ",1)==0) //skip the spaces
                i++;


            if(coeff==0 && ok0==0)//if the number is not essentially zero and still zero make the coefficient 1
            {
                coeff=1;
            }

            //if the number is negative multiply it by -1
            if(isnegative==1)
            {
                coeff*=-1;
                isnegative=0;
            }

            //if no x after the coefficient, then the power is zero
            if(strncmp(&(currentEq[i]),"\n",1)==0 || strncmp(&(currentEq[i]),"+",1)==0 || strncmp(&(currentEq[i]),"-",1)==0)
            {
                power=0;
            }

            else //there is a power
            {
                i++; //skip the x

                while(strncmp(&(currentEq[i])," ",1)==0) //skip the spaces
                    i++;

                if(strncmp(&(currentEq[i]),"^",1)==0) //skip the ^
                    i++;

                while(strncmp(&(currentEq[i])," ",1)==0)// skip the spaces
                    i++;

                //if there is no number between ^ and +-\n, then the power is 1
                if(strncmp(&(currentEq[i]),"+",1)==0 || strncmp(&(currentEq[i]),"-",1)==0 || strncmp(&(currentEq[i]),"\n",1)==0)
                {
                    power=1;
                }

                else //there is a power != 0
                {
                    while(strncmp(&(currentEq[i])," ",1)==0) //skip the spaces
                        i++;

                    if(strncmp(&(currentEq[i]),"1",1)>=0 && strncmp(&(currentEq[i]),"9",1)<=0) //the power
                    {
                        power = atoi(&(currentEq[i]));
                        int x=power;
                        while(x!=0) //skip for the length of the power
                        {
                            i++;
                            x/=10;
                        }
                    }

                    while(strncmp(&(currentEq[i])," ",1)==0) //skip the spaces
                        i++;
                }
            }

            if(strncmp(&(currentEq[i]),"-",1)==0)//if the next operation is - save the state for the next loop
            {
                isnegative=1;
            }

            insert(L,tail_L,coeff,power); //insert the node with the coeff and power that were read
            tail_L = tail_L -> next;

            i++; //skip to read the next node

        }
    }
    fclose(fptr); //close the file after reading all the lines
}



//perform the addition operation on all of the equations
void addition ()
{
    sumOfEquations =  (List) malloc(sizeof(struct Node));
    if(sumOfEquations==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    sumOfEquations -> next= NULL;
    sumOfEquations -> prev= NULL;

    position P = allEquations[0];
    position tailsum = sumOfEquations;
    while(P->next!=NULL)
    {
        P=P->next;
        position same = Find(P->exponent, sumOfEquations);
        if(same != NULL)
        {
            same->coefficient+=P->coefficient;
        }
        else
        {
            insert(sumOfEquations,tailsum, P->coefficient, P->exponent);
        }
        tailsum=tailsum->next;
    }


    for(int i=1; i< AllIndex; i++)
    {
        position atEq = allEquations[i];
        while(atEq -> next != NULL)
        {
            atEq = atEq -> next;
            position same = Find(atEq->exponent, sumOfEquations);
            if(same != NULL)
            {
                same->coefficient+=atEq->coefficient;
            }
            else
            {
                position bigger = Findbigger(atEq->exponent, sumOfEquations);
                insert(sumOfEquations, bigger, atEq->coefficient, atEq->exponent);
            }
        }
    }

    position sumpos = sumOfEquations;
    while(sumpos->next!=NULL)
    {
        sumpos=sumpos->next;
        if(sumpos->coefficient==0)
        {
            deleteNode(sumOfEquations, sumpos);
        }
    }



}



//perform the subtraction operation on all of the equations
void subtraction ()
{
    subOfEquations =  (List) malloc(sizeof(struct Node));
    if(subOfEquations==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    subOfEquations -> next= NULL;
    subOfEquations -> prev= NULL;
    position P = allEquations[0];
    position tailsub = subOfEquations;
    while(P->next!=NULL)
    {
        P=P->next;
        position same = Find(P->exponent, subOfEquations);
        if(same != NULL)
        {
            same->coefficient+=P->coefficient;
        }
        else
        {
            insert(subOfEquations,tailsub, P->coefficient, P->exponent);
        }
        tailsub=tailsub->next;
    }

    for(int i=1; i< AllIndex; i++)
    {
        position at = allEquations[i];
        while(at->next != NULL)
        {
            at = at -> next;
            position same = Find(at->exponent, subOfEquations);
            if(same != NULL)
            {
                same->coefficient-=at->coefficient;
            }
            else
            {
                position bigger = Findbigger(at->exponent, subOfEquations);
                double co = at->coefficient;
                co*=-1;
                insert(subOfEquations, bigger, co, at->exponent);
            }
        }
    }

    position subpos = subOfEquations;
    while(subpos->next!=NULL)
    {
        subpos=subpos->next;
        if(subpos->coefficient==0)
        {
            deleteNode(subOfEquations, subpos);
        }
    }

}



//perform the multiplication operation on all of the equations
void multiplication ()
{
    mulEq =  (List) malloc(sizeof(struct Node));
    if(mulEq==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    mulEq -> next= NULL;
    mulEq -> prev= NULL;
    List temp =  (List) malloc(sizeof(struct Node));
    if(temp==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    temp -> next= NULL;
    temp -> prev= NULL;

    position PAll = allEquations[0];
    position tailmul = mulEq;

    //moved the contents of the first equation into the result of multiplication
    while(PAll->next!=NULL)
    {
        PAll=PAll->next;
        position samemul = Find(PAll->exponent, mulEq);
        if(samemul != NULL)
        {
            samemul->coefficient+=PAll->coefficient;
        }
        else
        {
            insert(mulEq, tailmul, PAll->coefficient, PAll->exponent);
        }
        tailmul=tailmul->next;
    }


    for(int i=1; i< AllIndex; i++)
    {
        position at = allEquations[i];
        temp=MakeEmpty(temp);
        while(at->next != NULL)
        {
            at = at -> next;
            position where = mulEq;
            while(where->next != NULL)
            {
                where=where->next;
                double coeffmul = at->coefficient * where->coefficient;
                int powermul = at->exponent + where->exponent;
                position bigger = Findbigger(powermul, temp);
                if(bigger!=NULL)
                {
                    insert(temp, bigger, coeffmul, powermul);
                }
                else
                {
                    position posss = temp;
                    insert(temp, posss, coeffmul, powermul);
                }
            }
        }

        MakeEmpty(mulEq);
        tailmul = mulEq;
        position temoptr = temp;
        while(temoptr->next!=NULL)
        {
            temoptr = temoptr->next;
            insert(mulEq, tailmul, temoptr->coefficient, temoptr->exponent);
            tailmul=tailmul->next;
        }
    }

    mulOfEquations =  (List) malloc(sizeof(struct Node));
    if(mulOfEquations==NULL)
    {
        printf("could not initiate list. Out of memory\n");
        exit (1);
    }
    mulOfEquations -> next= NULL;
    mulOfEquations ->prev = NULL;
    position att = mulEq;
    while(att->next != NULL)
    {
        att = att -> next;
        position samee = Find(att->exponent, mulOfEquations);
        if(samee != NULL)
        {
            samee->coefficient+=att->coefficient;
        }
        else
        {
            position bigger = Findbigger(att->exponent, mulOfEquations);
            if(bigger==NULL)
            {
                position first = mulOfEquations;
                insert(mulOfEquations, first, att->coefficient, att->exponent);
            }
            else
            {
                if(att->exponent < bigger->exponent)
                    insert(mulOfEquations, bigger, att->coefficient, att->exponent);
                else
                {
                    position firstt = mulOfEquations;
                    insert(mulOfEquations, firstt, att->coefficient, att->exponent);
                }
            }
        }
    }

    position mulpos = mulOfEquations;
    while(mulpos->next!=NULL)
    {
        mulpos=mulpos->next;
        if(mulpos->coefficient==0)
        {
            deleteNode(mulOfEquations, mulpos);
        }
    }

}



//print all equations
void printAll()
{
    for(int i=0; i< AllIndex; i++)
    {
        printf("Equation %d: ", i+1);
        printList(allEquations[i]);;
    }
}



//substitute the value x in the list L
double Substitute (List L, double x)
{
    position P= L;
    double result=0;
    while(P->next!=NULL)
    {
        P=P->next;
        result+=P->coefficient*pow(x,P->exponent);
    }
    return result;
}



//works the same as printList but it prints to the file
void fprintList (List L, FILE* resultsptr)
{
    position P = L;
    P=P->next;
    fprintf(resultsptr, "%.3f x^%d ", P->coefficient, P->exponent);
    while(P->next!=NULL)
    {
        P=P->next;
        if(P->coefficient>0)
        {
            if(P->exponent==0)
            {
                if(P->coefficient==1)
                {
                    fprintf(resultsptr, "+ 1");
                }
                else
                {
                    fprintf(resultsptr, "+ %.3f", P->coefficient);
                }
            }
            else if(P->exponent==1)
            {
                if(P->coefficient==1)
                {
                    fprintf(resultsptr, "+ x ");
                }
                else
                {
                    fprintf(resultsptr, "+ %.3fx ", P->coefficient);
                }
            }
            else
            {
                if(P->coefficient==1)
                {
                    fprintf(resultsptr, "+ x^%d ", P->exponent);
                }
                else
                {
                    fprintf(resultsptr, "+ %.3fx^%d ", P->coefficient, P->exponent);
                }
            }
        }
        else if (P->coefficient<0)
        {
            double coo= P->coefficient * -1;
            if(P->exponent==0)
            {
                if(coo==1)
                {
                    fprintf(resultsptr, "- 1 ");
                }
                else
                {
                    fprintf(resultsptr, "- %.3f ", coo);
                }
            }
            else if(P->exponent==1)
            {
                if(P->coefficient==1)
                {
                    fprintf(resultsptr, "- x ");
                }
                else
                {
                    fprintf(resultsptr, "- %.3fx ", P->coefficient);
                }
            }
            else
            {
                if(P->coefficient==1)
                {
                    fprintf(resultsptr, "- x^%d ", P->exponent);
                }
                else
                {
                    fprintf(resultsptr, "- %.3fx^%d ", coo, P->exponent);
                }
            }
        }
    }
    fprintf(resultsptr, "\n\n");
}



//print all operation results to the file
void SaveToFile()
{
    FILE *fptr;
    fptr = fopen("results.txt", "w");
    addition();
    subtraction();
    multiplication();
    fprintf(fptr, "Result of Addition operation:\n");
    fprintList(sumOfEquations, fptr);
    fprintf(fptr, "Result of Subtraction operation:\n");
    fprintList(subOfEquations, fptr);
    fprintf(fptr, "Result of Multiplication operation:\n");
    fprintList(mulOfEquations, fptr);

    fclose(fptr);
}



//view the menu for the user to choose from
void Menu()
{
    printf("----------------------------------------Menu------------------------------------------\n");
    printf("\t.................................................................\n");
    printf("\t:\t(1) Read from the file equations.txt\t\t\t:\n");
    printf("\t:\t(2) Perform Addition\t\t\t\t\t:\n");
    printf("\t:\t(3) Perform Subtraction\t\t\t\t\t:\n");
    printf("\t:\t(4) Perform Multiplication\t\t\t\t:\n");
    printf("\t:\t(5) Save all operations to the file results.txt\t\t:\n");
    printf("\t:\t(6) Exit\t\t\t\t\t\t:\n");
    printf("\t.................................................................\n");
    printf("\n");

    printf("What is your Choice?\n");

}



int main()
{
    printf("\t\t\t\tWelcome to my program\n");
    printf("\tThis program reads equation and perform many operations on them. Check it out!\n");
    int isRead=0; //1 if the file is read
    char select;
    int selection=0;
    while(selection!=6)
    {
        Menu();

        scanf("%s", &select);
        if(strncmp(&(select),"0",1)>=0 && strncmp(&(select),"6",1)<=0)
        {
            selection = atoi(&(select));
        }
        else
        {
            selection = 0;
            printf("Invalid choice. You can only enter digits from 1 to 6\n");
            continue;
        }

        //switch to choose the option
        switch(selection)
        {
        case 1:
            printf("\n...........You have chosen option 1: Read from the file equations.txt..........\n\n");

            Read();
            printf("The following equation were read from the file:\n\n");
            printAll();
            isRead=1;
            printf("...........................................\n");
            printf("Reading is Complete\n");
            printf("\n\n");
            break;

        case 2:
            printf("\n..........You have chosen option 2: Perform addition..........\n\n");

            if(isRead)
            {
                printf("Addition is to be performed on the following equations:\n\n");
                printAll();
                addition();
                printf("Result of Addition operation is:\n");
                printf("\t");
                printList(sumOfEquations);
                printf("...........................................\n");
                printf("Addition is complete\n");
                printf("...........................................\n\n");
                int yes;
                printf("Enter (1) if you want to substitute a value in the result of Addition:\n");
                scanf("%d", &yes);
                if(yes==1)
                {
                    double x;
                    printf("Enter the value you want to substitute: ");
                    scanf("%lf", &x);
                    double sumSubstitute = Substitute(sumOfEquations, x);
                    printf("Substituting %.3f in the equation:\n", x);
                    printList(sumOfEquations);
                    printf("Result is: %f\n\n", sumSubstitute);
                }
                printf("...........................................\n");
                printf("Operation is complete\n");
            }

            else
            {
                printf("\n...........................................\n");
                printf("Addition can not be done. You need to read the file first\n");
            }
            printf("\n\n");
            break;

        case 3:
            printf("\n..........You have chosen option 3: Perform Subtraction..........\n\n");

            if(isRead)
            {
                printf("Subtraction is to be performed on the following equations:\n\n");
                printAll();

                subtraction();
                printf("Result of Subtraction operation is:\n");
                printf("\t");
                printList(subOfEquations);
                printf("...........................................\n");
                printf("Subtraction is complete\n");
                printf("...........................................\n\n");
                int yes;
                printf("Enter (1) if you want to substitute a value in the result of subtraction: \n");
                scanf("%d", &yes);
                if(yes==1)
                {
                    double x;
                    printf("Enter the value you want to substitute: ");
                    scanf("%lf", &x);
                    double subSubstitute = Substitute(subOfEquations, x);
                    printf("\nSubstituting %.3f in the equation:\n", x);
                    printList(subOfEquations);
                    printf("Result is: %f\n\n", subSubstitute);
                }
                printf("...........................................\n");
                printf("Operation is complete\n");
            }
            else
            {
                printf("...........................................\n");
                printf("Subtraction can not be done. You need to read the file first\n");
            }
            printf("\n\n");
            break;

        case 4:
            printf("\n..........You have chosen option 4: Perform Multiplication..........\n\n");

            if(isRead)
            {
                printf("Multiplication is to be performed on the following equations:\n\n");
                printAll();
                multiplication();
                printf("Result of Multiplication operation is:\n");
                printf("\t");
                printList(mulOfEquations);
                printf("Multiplication is complete");
                int yes;
                printf("Enter (1) if you want to substitute a value in the result of multiplication: \n");
                scanf("%d", &yes);
                if(yes==1)
                {
                    double x;
                    printf("Enter the value you want to substitute: ");
                    scanf("%lf", &x);
                    double mulSubstitute = Substitute(mulOfEquations, x);
                    printf("Substituting %.3f in the equation:\n", x);
                    printList(mulOfEquations);
                    printf("Result is: %f\n\n", mulSubstitute);
                }
                printf("...........................................\n");
                printf("Operation is complete\n");
            }
            else
            {
                printf("...........................................\n");
                printf("Multiplication can not be done. You need to read the file first\n");
            }
            printf("\n\n");
            break;

        case 5:
            printf("\n..........You have chosen option 5: Save all operations to the file results.txt..........\n\n");

            if(isRead)
            {
                SaveToFile();
                printf("Saving to file operation is done\n");
                printf("Results will be found in the file results.txt");
            }
            else
            {
                printf("Operation can not be done. You need to read the file first\n");
                printf("...........................................\n");
            }
            printf("\n\n");
            break;

        case 6:
            printf("..........Thank you for using the program..........\n");
            break;

        default:
            printf("Wrong selection. Try again Please.\n");
            break;

        }
    }

    return 0;
}
