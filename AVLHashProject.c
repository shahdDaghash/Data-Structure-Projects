#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAXSIZE 400
#define SIZE 100
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
typedef int boolean;

struct AVLNode;
typedef struct AVLNode *AVL;

struct HashNode;
typedef struct HashNode* Hash;

struct patientInfo //This struct contains the information of a patient and is used in both the TreeNode and the HashTable
{
    char PatientName [SIZE];
    char Gender [2];
    char DateOfAdmission [9];
    char DateOfBirth [9];
    char Illness [SIZE];
    char Address [SIZE];
    char BloodType [4];

};

struct AVLNode //Tree node
{
    struct patientInfo info;
    AVL Left;
    AVL Right;
    int Height;
};


struct HashNode //Hash node for the table
{
    struct patientInfo info;
    int status;
};

//Predefining these functions so it will not give implicit declaration warning
void InsertNodeToHash(struct patientInfo t, Hash Table, int sizeOfInsert, int stat);
void HashWork();

//Global variable to be used in the project
AVL Tree = NULL;
int sizeOfTree = 0;
Hash Table;
int TableSize;
int numberOfItems;
int numberOfElementsInTable=0;
FILE *fOutPtr;


///////////////////////////////////////////////////////////////AVL Functions/////////////////////////////////////////////////////////////////////////////////////////

//Return the height of the node
int GetHeight (AVL t)
{
    if(t==NULL)
        return -1;
    else
        return t->Height;
}

//evaluates the balance factor which is the height from the left side - the height from the right side
int GetBalanceFactor(AVL t)
{
    if (t == NULL)
        return 0;
    return GetHeight(t->Left) - GetHeight(t->Right);
}

//Find the smallest node on the left of node t
AVL FindMin( AVL t )
{
    if( t == NULL)
        return NULL;
    else if( t -> Left == NULL)
        return t;
    else
        return FindMin( t->Left );
}

//perform the single rotate left operation and update the heights of the nodes
AVL SingleRotateLeft(AVL x)
{
    AVL y;
    y = x->Right;
    x->Right  = y->Left;
    y->Left = x;

    x->Height = 1 + MAX(GetHeight(x->Left), GetHeight(x->Right));
    y->Height = 1 + MAX(GetHeight(x), GetHeight(y->Right));

    return y;
}

//perform the single rotate right operation and update the heights of the nodes
AVL SingleRotateRight(AVL y)
{
    AVL x;
    x        = y->Left;
    y->Left  = x->Right;
    x->Right = y;

    y->Height = 1 + MAX(GetHeight(y->Right), GetHeight(y->Left));
    x->Height = 1 + MAX(GetHeight(y), GetHeight(x->Left));

    return x;
}

//perform double rotate left (single rotate left then single rotate right)
AVL DoubleRotateLeft(AVL t)
{
    t->Left  = SingleRotateLeft(t->Left);
    return SingleRotateRight(t);
}

//perform double rotate right (single rotate right then single rotate left)
AVL DoubleRotateRight(AVL t)
{
    t->Right  = SingleRotateRight(t->Right);
    return SingleRotateLeft(t);
}

//checks if an entered date is a valid one
//a date is valid if it consists of numerics only and of size 8
boolean IsValidDate(char date[9])
{
    for(int i=0; i<8; i++)
    {
        if(date[i] < 48 || date[i] > 57)
        {
            return 0;
        }
    }
    if(date[8]!='\0')
        return 0;
    return 1;
}

//Used to enter a valid date (used when inserting and modifying in Tree and inserting in Hash)
void EnterValidDate(char date[9])
{
    scanf("\n\t");
    gets(date);

    while(!IsValidDate(date))
    {
        printf("\tWrong entry. Try Again:\n\t");
        scanf("\n\t");
        gets(date);
    }
}

//This function searches for the node with the name (patientNameToFind) and returns the node
AVL Find(AVL t, char patientNameToFind[SIZE])
{
    if(t==NULL)
        return NULL;
    else if(strcmp(patientNameToFind, t->info.PatientName)<0) //search in the left if smaller
        return Find(t->Left, patientNameToFind);
    else if(strcmp(patientNameToFind, t->info.PatientName)>0) //Search in the right if larger
        return Find(t->Right, patientNameToFind);
    else
        return t;
}

//This function inserts a new node to the tree with the information from the array
AVL Insert (AVL t, char patientInfoArr [7][SIZE])
{
    if(t==NULL)
    {
        t=malloc(sizeof (struct AVLNode));
        if(t==NULL)
        {
            printf("\tOut of Memory. Can't Insert node to Tree\n");
            exit(1);
        }
        t->Height=0;
        t->Left= t->Right=NULL;
        strcpy(t->info.PatientName, patientInfoArr[0]);
        strcpy(t->info.Gender, patientInfoArr[1]);
        strncpy(t->info.DateOfAdmission, patientInfoArr[2], 8);
        strncpy(t->info.DateOfBirth, patientInfoArr[3], 8);
        strcpy(t->info.Illness, patientInfoArr[4]);
        strcpy(t->info.Address, patientInfoArr[5]);
        strncpy(t->info.BloodType, patientInfoArr[6],4);
        sizeOfTree++;
    }

    else if(strcasecmp(patientInfoArr[0], t->info.PatientName)<0)
    {
        t->Left=Insert(t->Left,patientInfoArr);
        if(GetHeight(t->Left) - GetHeight(t->Right) >= 2) //balancing the tree
        {
            if(strcmp(patientInfoArr[0], t->Left->info.PatientName)<0)
                t=SingleRotateRight(t);
            else
                t=DoubleRotateLeft(t);
        }
    }
    else if(strcasecmp(patientInfoArr[0], t->info.PatientName)>0)
    {
        t->Right = Insert(t->Right, patientInfoArr);
        if(GetHeight(t->Right) - GetHeight(t->Left) >= 2) //balancing the tree
        {
            if(strcmp(patientInfoArr[0], t->Right->info.PatientName)>0)
                t=SingleRotateLeft(t);
            else
                t=DoubleRotateRight(t);
        }
    }

    t->Height = 1 + MAX(GetHeight(t->Right), GetHeight(t->Left)); //update the height

    return t;
}

//This function deleted the node with the name as in patientNameToDelete
AVL Delete( char patientNameToDelete [SIZE], AVL t )
{
    if( t == NULL )
        return NULL;

    else if( strcmp(patientNameToDelete,t->info.PatientName)<0 )
        t->Left = Delete( patientNameToDelete, t->Left );

    else if( strcmp(patientNameToDelete,t->info.PatientName)>0 )
        t->Right = Delete( patientNameToDelete, t->Right );
    else
    {
        if( t->Left==NULL || t->Right==NULL )
        {
            AVL temp = t->Left ? t->Left : t->Right;
            if (temp==NULL)
            {
                temp = t;
                t=NULL;
            }
            else
            {
                *t = *temp;
                free(temp);
            }
        }
        else
        {
            AVL temp = FindMin(t->Right);
            t->info = temp->info;
            t->Right = Delete(temp->info.PatientName, t->Right);
        }

    }
    if(t==NULL)
    {
        return t;
    }
    t -> Height = MAX(GetHeight(t -> Left), GetHeight(t-> Right));
    //Here the balancing is done
    int balance = GetBalanceFactor(t);
    if (balance > 1 && GetBalanceFactor(t->Left) >= 0)
    {
        return SingleRotateRight(t);
    }
    if (balance > 1 && GetBalanceFactor(t->Left) < 0)
    {
        t->Left = SingleRotateLeft(t->Left);
        return SingleRotateRight(t);
    }
    if (balance < -1 && GetBalanceFactor(t->Right) <= 0)
    {
        return SingleRotateLeft(t);
    }
    if (balance < -1 && GetBalanceFactor(t->Right) > 0)
    {
        t->Right = SingleRotateRight(t->Right);
        return SingleRotateRight(t);
    }
    return t;

}

//this function prints a node to wither the console or the file depending on the variable printToFile
void PrintInfoOfPatient (AVL t, boolean printToFile)
{
    if(printToFile)
    {
        fprintf(fOutPtr, "%s\t",t->info.PatientName);
        fprintf(fOutPtr, "%s\t", strupr(t->info.Gender));
        char date [8];
        strcpy(date, t->info.DateOfAdmission);
        fprintf(fOutPtr, "%c%c/%c%c/%c%c%c%c\t", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
        strcpy(date, t->info.DateOfBirth);
        fprintf(fOutPtr, "%c%c/%c%c/%c%c%c%c\t", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
        fprintf(fOutPtr, "%s\t", t->info.Illness);
        fprintf(fOutPtr, "%s\t\t", t->info.Address);
        fprintf(fOutPtr, "%s\n\n", t->info.BloodType);
    }
    else
    {
        printf("\n\tName: %s\n",t->info.PatientName);
        printf("\tGender: %s\n", strupr(t->info.Gender));
        char date [8];
        strcpy(date, t->info.DateOfAdmission);
        printf("\tDate of admission: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
        strcpy(date, t->info.DateOfBirth);
        printf("\tDate of Birth: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
        printf("\tIllness: %s\n", t->info.Illness);
        printf("\tAddress: %s\n", t->info.Address);
        printf("\tBlood Type: %s\n", strupr(t->info.BloodType));
    }
}

//This function prints all the records to the console lexicographically
//Using in order printing (left node right)
void PrintInOrderToConsole(AVL t)
{
    if(t!= NULL)
    {
        PrintInOrderToConsole(t->Left);
        PrintInfoOfPatient(t,0);
        PrintInOrderToConsole(t->Right);
    }
    else
    {
        return;
    }
}

//This function prints all the records to the file using in order method as well
void PrintInOrderToFile(AVL t)
{
    if(t!= NULL)
    {
        PrintInOrderToFile(t->Left);
        PrintInfoOfPatient(t,1);
        PrintInOrderToFile(t->Right);
    }
    else
    {
        return;
    }
}

//This function prints all the nodes that have the same illness as the entered string
boolean FindIllness(AVL t, char illnessToFind[SIZE])
{
    if(t!=NULL)
    {
        boolean didPrinting = FindIllness(t->Left, illnessToFind);
        if(strcasecmp(t->info.Illness, illnessToFind)==0)
        {
            PrintInfoOfPatient(t,0);
            didPrinting=1;
        }
        didPrinting = MAX(FindIllness(t->Right, illnessToFind), didPrinting );
        return didPrinting;
    }
    return -1;
}


//this function reads the file and inserts the nodes to the tree
void ReadFromFile ()
{
    FILE *fptr;
    fptr = fopen("patients.txt", "r");
    while(!feof(fptr))
    {
        char line[MAXSIZE];
        char patientInfoArr [7][SIZE];
        int segment = 0;

        fgets(line, MAXSIZE, fptr);
        char *token;
        token = strtok(line, "#");

        while(token!=NULL)
        {
            strcpy(patientInfoArr[segment], token);
            segment++;
            token = strtok(NULL, "#\n");
        }
        Tree = Insert(Tree,patientInfoArr);
    }

}

//This function asks the user for the information of the patient they want to add and inserts the record to the tree by using insert function
void InsertNewPatient()
{

    char patientInfoArr [7][SIZE];
    char bloodTypes [][4] = {"A-", "A+", "B-", "B+", "AB-", "AB+", "O-", "O+" };

    char patientName[SIZE];
    printf("\tPlease Enter Patient's name:\n\t");
    scanf("\n\t");
    gets(patientName);

    while(Find(Tree, patientName) != NULL) //if the patient name already exists in the tree then it's invalid to enter it again
    {
        printf("\tPatient Name already exists in the tree. Enter 1 if you want to enter a new name: ");
        char tryAgain;
        scanf("%s", &tryAgain);
        if(strncmp(&(tryAgain),"1",1)!=0) //check if the user wants to enter another name
        {
            printf("\tOperation was not complete\n");
            return;
        }
        printf("\tPlease Enter Patient's name:\n\t");
        scanf("\n\t");
        gets(patientName);
    }
    strcpy(patientInfoArr[0], patientName);

    while(1)
    {
        printf("\tPlease Enter Gender (F/M):\n\t");
        scanf("\n\t");
        gets(patientInfoArr[1]);
        if(strcasecmp(patientInfoArr[1],"F")==0 || strcasecmp(patientInfoArr[1],"M")==0) //check if the input is FfMm
        {
            break;
        }
        else
        {
            printf("\tInvalid Gender. Try Again.\n");
        }
    }

    printf("\tPlease Enter Date of admission:\n\t");

    char dateOfAdmission[9];
    EnterValidDate(dateOfAdmission); //The function makes sure a valid date is entered
    strncpy(patientInfoArr[2], dateOfAdmission, 8);


    printf("\tPlease Enter Date Of Birth:\n\t");
    char dateOfBirth[9];
    EnterValidDate(dateOfBirth);
    strcpy(patientInfoArr[3], dateOfBirth);

    printf("\tPlease Enter The Illness:\n\t");
    scanf("\n\t");
    gets(patientInfoArr[4]);

    printf("\tPlease Enter The Address:\n\t");
    scanf("\n\t");
    gets(patientInfoArr[5]);

    while(1)
    {
        printf("\tPlease Enter The Blood Type:\n\t");
        scanf("\n\t");
        gets(patientInfoArr[6]);
        boolean validBloodType=0;
        for(int i=0; i<sizeof(bloodTypes); i++) //loop through the array in the beginning of this function and check if it's one of the valid blood types
        {
            if(strcasecmp(patientInfoArr[6], bloodTypes[i])==0)
            {
                validBloodType=1;
            }
        }
        if(validBloodType)
            break;
        printf("\tInvalid Blood Type. Please try again.\n");
    }

    printf("\tFollowing is the record you want to add:\n");
    printf("\n\tName: %s\n",patientInfoArr[0]);
    printf("\tGender: %s\n", strupr(patientInfoArr[1]));
    char date [8];
    strcpy(date, patientInfoArr[2]);
    printf("\tDate of admission: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
    strcpy(date, patientInfoArr[3]);
    printf("\tDate of Birth: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
    printf("\tIllness: %s\n", patientInfoArr[4]);
    printf("\tAddress: %s\n", patientInfoArr[5]);
    printf("\tBlood Type: %s\n", strupr(patientInfoArr[6]));

    printf("\tEnter 1 to confirm adding: "); //check if the user is sure to enter the record
    char confirmInsert;
    scanf("%c", &confirmInsert);
    if(strncmp(&(confirmInsert),"1",1)==0)
    {
        Tree = Insert(Tree, patientInfoArr);
        printf("\n\tInserting the patient is complete\n");
    }
    else
    {
        printf("\tThe entered record is not added.\n");
    }

}

//This function searches for a patient by their name that is taken from the user
void FindAndModify()
{
    char bloodTypes [][4] = {"A-", "A+", "B-", "B+", "AB-", "AB+", "O-", "O+" };
    while(1)
    {
        char patientNameToFind[SIZE];
        printf("\n\tEnter the name of the patient you want to find:\n\t");
        scanf("\n\t");
        gets(patientNameToFind);
        AVL patientNode = Find(Tree, patientNameToFind);
        if(patientNode==NULL)
            printf("\tPatient Not Found\t");
        else
        {
            printf("\tPatient was found. Info of this patient:\n");
            PrintInfoOfPatient(patientNode,0);
            printf("\n");
            //give the user the option to modify any of the information in the name
            //when modifying a valid input should be entered
            printf("\n\tEnter 1 if you want to modify the record of this patient: \n\t");
            char ifModify;
            scanf("%s", &ifModify);
            if(strncmp(&(ifModify),"1",1)!=0)
                return;

            printf("\tEnter the choice of the information you want to change\n");
            printf("\t(1) Change Patient name\n");
            printf("\t(2) Change Gender\n");
            printf("\t(3) Change date of admission\n");
            printf("\t(4) Change date of birth\n");
            printf("\t(5) Change Illness\n");
            printf("\t(6) Change Address\n");
            printf("\t(7) Change Blood Type\n");
            int choice;
            printf("\t");
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                printf("\tPlease Enter patient's name:\n\t");
                scanf("\n\t");
                char patientInfoArr [7][SIZE];
                strcpy(patientInfoArr[0], patientNode->info.PatientName);
                strcpy(patientInfoArr[1], patientNode->info.Gender);
                strcpy(patientInfoArr[2], patientNode->info.DateOfAdmission);
                strcpy(patientInfoArr[3], patientNode->info.DateOfBirth);
                strcpy(patientInfoArr[4], patientNode->info.Illness);
                strcpy(patientInfoArr[5], patientNode->info.Address);
                strcpy(patientInfoArr[6], patientNode->info.BloodType);

                Delete(patientNode->info.PatientName, Tree);
                gets(patientInfoArr[0]);
                Tree = Insert(Tree, patientInfoArr);
                patientNode = Find(Tree, patientInfoArr[0]);

                break;
            case 2:
                while(1)
                {
                    printf("\tPlease Enter Gender (F/M):\n\t");
                    scanf("\n\t");
                    char gender [SIZE];
                    gets(gender);
                    if(strcasecmp(gender,"F")==0 || strcasecmp(gender,"M")==0)
                    {
                        strncpy(patientNode->info.Gender, gender, 1);
                        break;
                    }
                    else
                    {
                        printf("\tInvalid Gender. Try Again.\n");
                    }
                }
                break;
            case 3:
                printf("\tPlease Enter Date Of admission:\n\t");
                char dateOfadmission[9];
                EnterValidDate(dateOfadmission);
                strcpy(patientNode->info.DateOfAdmission, dateOfadmission);
                break;
            case 4:
                printf("\tPlease Enter Date Of Birth:\n\t");
                char dateOfBirth[9];
                EnterValidDate(dateOfBirth);
                strcpy(patientNode->info.DateOfBirth, dateOfBirth);
                break;
            case 5:
                printf("\tPlease Enter Illness:\n\t");
                scanf("\n\t");
                gets(patientNode->info.Illness);
                break;
            case 6:
                printf("\tPlease Enter Address:\n\t");
                scanf("\n\t");
                gets(patientNode->info.Address);
                break;
            case 7:
                while(1)
                {
                    printf("\tPlease Enter The Blood Type:\n\t");
                    scanf("\n\t");
                    char blood [SIZE];
                    gets(blood);
                    boolean validBloodType=0;
                    for(int i=0; i<sizeof(bloodTypes); i++)
                    {
                        if(strcasecmp(blood, bloodTypes[i])==0)
                        {
                            validBloodType=1;
                        }
                    }
                    if(validBloodType)
                    {
                        strncpy(patientNode->info.BloodType, blood, 1);
                        break;
                    }
                    printf("\tInvalid Blood Type. Please try again.\n");
                }
                break;
            default:
                printf("\tWrong choice. Try again\n");
            }
            if(ifModify)
            {
                printf("\n\tPatient record was modified.\n");
            }

            printf("\n\tName: %s\n",patientNode->info.PatientName);
            printf("\tGender: %s\n", patientNode->info.Gender);
            char date [8];
            strcpy(date, patientNode->info.DateOfAdmission);
            printf("\tDate of admission: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
            strcpy(date, patientNode->info.DateOfBirth);
            printf("\tDate of Birth: %c%c/%c%c/%c%c%c%c\n", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7] );
            printf("\tIllness: %s\n", patientNode->info.Illness);
            printf("\tAddress: %s\n", patientNode->info.Address);
            printf("\tBlood Type: %s\n", strupr(patientNode->info.BloodType));


        }
        //Give the user the option to search for another patient
        printf("\n\tEnter 1 if you want to search for another patient: ");
        char searchpatient;
        scanf("%s", &searchpatient);
        if(strncmp(&(searchpatient),"1",1)!=0)
            break;

    }
}

//Print all the patients that have the same illness entered by the user (FindIllness is used)
void FindSameIllness ()
{
    printf("\tPlease enter an illness:\n\t");
    char illness [SIZE];
    scanf("\n\t");
    gets(illness);
    boolean didPrinting = FindIllness(Tree, illness);
    //The function will return whether it did any printing or not
    if(didPrinting==-1)
        printf("\tNo Patient has the illness you entered\n");

}

//function that takes form the user the patient whom they want to delete
void DeletePatient ()
{
    printf("\tPlease enter a patient name to delete their record:\n\t");
    char patientNameToDelete [SIZE];
    scanf("\n\t");
    gets(patientNameToDelete);
    while(Find(Tree, patientNameToDelete)==NULL)
    {
        printf("\tPatient not found in the system\n\tEnter Another name\n\t");
        scanf("\n\t");
        gets(patientNameToDelete);
    }
    Tree = Delete(patientNameToDelete, Tree);
    printf("\n\tPatient %s is Deleted From the Tree\n", patientNameToDelete);
    sizeOfTree--;
    PrintInOrderToConsole(Tree);
}

//The menu that shows the user the list of operations
void AVLMenu()
{
    printf("\t----------------------------------------AVL Menu------------------------------------------\n");
    printf("\n\t Choose one of the following.\n\n") ;
    printf("\t (1): Read the file (patients.txt) and save the records in AVL Tree.\n\n") ;
    printf("\t (2): Insert New Patient.\n\n") ;
    printf("\t (3): Find a patient (you can choose to edit the information).\n\n") ;
    printf("\t (4): List All Patients in lexicographic order with all information.\n\n") ;
    printf("\t (5): List All Patients who have the same illness entered.\n\n") ;
    printf("\t (6): Delete a Patient From The System.\n\n") ;
    printf("\t (7): Save the Contents To The File (patients_hash.data). This will take you to the Hash part in the program.\n\n") ;
    printf("\t (8): Exit from the program.\n\n");
    printf("\t--------------------------------------------------------------------------------------\n");
}

//This functions clears the screen for easier using of the program
void ClearScreen()
{
    system("cls");
    printf("\n\t\t_______________________________________________________________________________\n\n");
}

//This function performs the options shown in the menu to the user
void AVLWork()
{
    system("color f0");
    printf("\t\t\t\tWelcome to my program (Project 3)\n");
    printf("\tThis program does operations of patient records using AVL Tree and Hashing. Check it out!\n");
    boolean isRead=0;
    char select;
    int selection;
    while(1)
    {
        AVLMenu();
        printf("\tEnter Your Selection:\n\t");
        scanf("%s", &select);
        if(strncmp(&(select),"0",1)>=0 && strncmp(&(select),"8",1)<=0)
        {
            selection = atoi(&(select));
        }
        else
        {
            ClearScreen();
            printf("\tInvalid choice. You can only enter digits from 1 to 8\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            continue;
        }

        switch(selection)
        {
        case 1:
            ClearScreen();
            printf("\n\t...........You have chosen option 1: Read the file (patients.txt) and save the records in AVL Tree...........\n\n");

            ReadFromFile();
            printf("\tThe following records were read from the file:\n");
            PrintInOrderToConsole(Tree);
            isRead=1;
            printf("\n\tReading is Complete\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 2:
            ClearScreen();
            printf("\n\t...........You have chosen option 2: Insert New Patient...........\n\n");

            if(isRead)
            {
                InsertNewPatient();
                printf("\n\tList of all the records: \n");
                PrintInOrderToConsole(Tree);
            }
            else
            {
                printf("\tYou need to read from the file before adding a patient record\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 3:
            ClearScreen();
            printf("\n\t...........You have chosen option 3: Find a patient (you can choose to edit the information)...........\n\n");

            if(isRead)
            {
                FindAndModify();
            }
            else
            {
                printf("\tYou need to read from the file before searching for a patient\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 4:
            ClearScreen();
            printf("\n\t...........You have chosen option 4: List All Patients in lexicographic order with all information...........\n\n");

            if(isRead)
            {
                PrintInOrderToConsole(Tree);
            }
            else
            {
                printf("\tYou need to read from the file before printing the records\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 5:
            ClearScreen();
            printf("\n\t...........You have chosen option 5: List All Patients who have the same illness entered...........\n\n");

            if(isRead)
            {
                FindSameIllness();
            }
            else
            {
                printf("\tYou need to read from the file before printing the records with the entered Illness\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 6:
            ClearScreen();
            printf("\n\t...........You have chosen option 6: Delete a Patient From The System...........\n\n");

            if(isRead)
            {
                DeletePatient();
            }
            else
            {
                printf("\tYou need to read from the file before deleting a patient\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 7:
            ClearScreen();
            printf("\n\t...........You have chosen option 7: Save the Contents To The File (patients_hash.data)...........\n");
            printf("\tAfter printing to the file the program will move to the hash part\n\n");

            if(isRead)
            {
                fOutPtr = fopen("patients_hash.data.txt", "w");
                PrintInOrderToFile(Tree);
                fclose(fOutPtr);
                printf("\tSaving The Contents To The File Is Complete\n");
                HashWork();
            }
            else
            {
                printf("\tYou need to read from the file before Saving the Contents to the file (patients_hash.data)\n");
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 8:
            printf("\tPress 1 to confirm that you want to exit the program or press any key to get back to the menu: ");
            char confirmExit;
            scanf("%s", &confirmExit);
            if(strncmp(&(confirmExit),"1",1)==0)
            {
                printf("\tThank You For Using My Program\n");
                exit(0);
            }
            break;
        default:
            ClearScreen();
            printf("\tWrong selection. Try again Please.\n");
            printf("\t\t_______________________________________________________________________________\n\n");
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////Hash Functions/////////////////////////////////////////////////////////////////////////////////////////

//Returns the first prime that is either bigger than or bigger than or equal the number passed to it
int GeneratePrime (int num, boolean equal)
{
    boolean isPrime;
    int initialValue = num;
    while(1)
    {
        isPrime=1;
        for(int i=2; i*i<=num; i++)
        {
            if(num%i==0)
            {
                isPrime=0;
                break;
            }
        }
        if(isPrime && num==initialValue)
        {
            if(equal) //if the number passed is prime and the sign is bigger than or equal
                return initialValue;
        }
        else
        {
            if(isPrime)
                return num;
        }
        num++;
    }
}

//This function returns the sum of the ASCII codes of the letters in the string passed to it
int CharSum (char s [SIZE])
{
    int sumOfChars = 0;
    for(int i=0; i<strlen(s); i++)
    {
        sumOfChars += s[i];
    }
    return sumOfChars;
}

//this functions calculates the index to put the record of the patient in by using open addressing - double hashing method
int CalculateIndex (int key, int i, int sizeOfInsert)
{
    int h = key%sizeOfInsert;
    int hp = 1+(key%(sizeOfInsert-1));
    int index = (h+i*hp)%sizeOfInsert;
    return index;
}

//This function creates the table that is to be used
Hash CreateTable (int neededSize)
{
    Hash table = (Hash)malloc(sizeof(struct HashNode)*neededSize);
    if(table == NULL)
    {
        printf("\tOut of Memory. Can't Create The Hash Table\n");
        exit(1);
    }
    numberOfElementsInTable = 0;
    //initially set all the records to - and status to free
    for(int i=0; i<neededSize; i++)
    {
        strcpy(table[i].info.PatientName, "-");
        strcpy(table[i].info.Gender, "-");
        strcpy(table[i].info.DateOfAdmission, "-");
        strcpy(table[i].info.DateOfBirth, "-");
        strcpy(table[i].info.Illness, "-");
        strcpy(table[i].info.Address, "-");
        strcpy(table[i].info.BloodType, "-");
        table[i].status=0;
    }
    return table;
}

//This function performs the rehashing to the main table
void Rehash(struct patientInfo t)
{
    //define a new table with double size of original size
    int tempTableSize = GeneratePrime(ceil(TableSize*2), 0);
    Hash tempTable = CreateTable(tempTableSize);
    //move all visited and deleted records from the main table to the temp table
    for(int i=0; i<TableSize; i++)
    {
        if(Table[i].status == 1 || Table[i].status==2)
        {
            InsertNodeToHash(Table[i].info, tempTable, tempTableSize, Table[i].status);
        }
    }
    //After inserting let the main table be the temp table (points at it)
    Table = tempTable;
    //update the tableSize
    TableSize = tempTableSize;
    //insert the last node that led to the hashing
    InsertNodeToHash(t, Table, TableSize,0);
}

//This function inserts a node from the tree or from the user to the table
//stat is used to when inserting a deleted record from the old table to the new one so that the status stay 2
void InsertNodeToHash(struct patientInfo t, Hash Table, int sizeOfInsert, int stat)
{

    int key = CharSum(t.PatientName); //calculate the sum of ASCII codes for the name
    double loadFactor = (numberOfElementsInTable+1) / (double)sizeOfInsert; //calculate the load factor in order to check it before performing the insert operation
    int i=0;
    if(loadFactor<=0.7) //can do insertion
    {
        int index = CalculateIndex(key, i, sizeOfInsert);
        //check for empty space
        while( Table[index].status==1 && i<=(sizeOfInsert-1) )
        {
            i++;
            index = CalculateIndex(key, i, sizeOfInsert);
        }
        if(i<=sizeOfInsert-1 ) //if i is the range
        {
            if(stat==2)
            {
                Table[index].status=2;
            }
            else
            {
                Table[index].status=1;
            }
            //copy the contents
            Table[index].info = t;
            //increment the number of elements in the table
            numberOfElementsInTable++;
        }
        else //if i is larger than tableSize -1 then there is no spot for the record and hence, it needs rehashing
        {
            Rehash(t);
        }
    }
    else if (loadFactor>0.7)//if the load factor is larger than 0.7 then the table needs rehashing
    {
        Rehash(t);
    }
}

//this function is used to search for the patient with the name that is sent to the function
int FindInHash (char patientName [SIZE])
{
    int key = CharSum(patientName);
    int i=0;
    //It calculates the index each time from i=0 to i=tablesize-1, if the record is found return the index of the record else return -1
    while (i<TableSize)
    {
        int index = CalculateIndex(key, i, TableSize);
        if(Table[index].status==1 && strcmp(Table[index].info.PatientName, patientName)==0)
        {
            return index;
        }
        i++;
    }
    return -1;
}




void DeleteInHash()
{
    char patientName [SIZE];
    printf("\tEnter the name of the patient you want to delete\n\t");
    scanf("\n\t");
    gets(patientName);
    int index = FindInHash(patientName);
    if(index == -1)
    {
        printf("\tPatient does not exist in the table\n");
    }
    else
    {
        Table[index].status=2;
        numberOfElementsInTable--;
        printf("\tPatient is now marked deleted\n");
    }
}

void InsertRecordToTable()
{
    struct patientInfo EnteredInfo;
    char bloodTypes [][4] = {"A-", "A+", "B-", "B+", "AB-", "AB+", "O-", "O+" };

    char patientName[SIZE];
    printf("\tPlease Enter Patient's name:\n\t");
    scanf("\n\t");
    gets(patientName);


    while(FindInHash(patientName)!=-1)
    {
        printf("\tPatient Name already exists in the tree. Enter 1 if you want to enter a new name: ");
        char tryAgain;
        scanf("%c", &tryAgain);
        if(strncmp(&(tryAgain),"1",1)!=0)
        {
            printf("\tOperation was not complete\n");
            break;
        }
        printf("\tPlease Enter Patient's name:\n\t");
        scanf("\n\t");
        gets(patientName);
    }
    strcpy(EnteredInfo.PatientName, patientName);

    while(1)
    {
        printf("\tPlease Enter Gender (F/M):\n\t");
        scanf("\n\t");
        gets(EnteredInfo.Gender);
        if(strcasecmp(EnteredInfo.Gender,"F")==0 || strcasecmp(EnteredInfo.Gender,"M")==0)
        {
            break;
        }
        else
        {
            printf("\tInvalid Gender. Try Again.\n");
        }
    }

    printf("\tPlease Enter Date of admission:\n\t");

    char dateOfAdmission[9];
    EnterValidDate(dateOfAdmission);
    strncpy(EnteredInfo.DateOfAdmission, dateOfAdmission, 8);


    printf("\tPlease Enter Date Of Birth:\n\t");
    char dateOfBirth[9];
    EnterValidDate(dateOfBirth);
    strcpy(EnteredInfo.DateOfBirth, dateOfBirth);

    printf("\tPlease Enter The Illness:\n\t");
    scanf("\n\t");
    gets(EnteredInfo.Illness);

    printf("\tPlease Enter The Address:\n\t");
    scanf("\n\t");
    gets(EnteredInfo.Address);

    while(1)
    {
        printf("\tPlease Enter The Blood Type:\n\t");
        scanf("\n\t");
        gets(EnteredInfo.BloodType);
        boolean validBloodType=0;
        for(int i=0; i<8; i++)
        {
            if(strcasecmp(EnteredInfo.BloodType, bloodTypes[i])==0)
            {
                validBloodType=1;
            }
        }
        if(validBloodType)
            break;
        printf("\tInvalid Blood Type. Please try again.\n");
    }

    InsertNodeToHash(EnteredInfo, Table, TableSize,0);
    printf("\tRecord is inserted to the table\n");
}

void PrintIndexInHash(int i, boolean toFile)
{
    if(toFile)
    {
        fprintf(fOutPtr, "\t%d\t", i);
        if(Table[i].status==0)
            fprintf(fOutPtr, "Free");
        else if(Table[i].status==1)
            fprintf(fOutPtr, "Visited");
        else
            fprintf(fOutPtr, "Deleted");
        fprintf(fOutPtr, "\t\t%-15s", Table[i].info.PatientName );
        fprintf(fOutPtr, "\t\t  %-2s ", Table[i].info.Gender );
        fprintf(fOutPtr, "\t       %-8s ", Table[i].info.DateOfAdmission );
        fprintf(fOutPtr, "\t\t%-8s ", Table[i].info.DateOfBirth );
        fprintf(fOutPtr, "\t    %-15s ", Table[i].info.Illness );
        fprintf(fOutPtr, "\t    %-15s ", Table[i].info.Address );
        fprintf(fOutPtr, "\t%-4s ", Table[i].info.BloodType );
        fprintf(fOutPtr, "\n\n");
    }
    else
    {
        printf("\t%d\t", i);
        if(Table[i].status==0)
            printf("Free");
        else if(Table[i].status==1)
            printf("Visited");
        else
            printf("Deleted");
        printf("\t\t%-15s", Table[i].info.PatientName );
        printf("\t\t  %-2s ", Table[i].info.Gender );
        printf("\t       %-8s ", Table[i].info.DateOfAdmission );
        printf("\t\t%-8s ", Table[i].info.DateOfBirth );
        printf("\t    %-15s ", Table[i].info.Illness );
        printf("\t    %-15s ", Table[i].info.Address );
        printf("\t%-4s ", Table[i].info.BloodType );
        printf("\n\n");
    }
}

void PrintTable(boolean toFile)
{
    if(numberOfElementsInTable==0)
    {
        printf("The Whole Table is Empty.\n");
    }

    if(!toFile)
    {
        printf("\tTable Size : %d\n\n", TableSize);
        printf("      Index     Status\t       Patient Name             Gender       Date Of Admission        Date Of Birth           Illness               Address         Blood Type\n\n");
    }
    else
    {
        fprintf(fOutPtr, "\tTable Size : %d\n\n", TableSize);
        fprintf(fOutPtr, "      Index     Status\t       Patient Name             Gender       Date Of Admission        Date Of Birth           Illness               Address         Blood Type\n\n");
    }
    for(int i=0; i<TableSize; i++)
    {
        PrintIndexInHash(i, toFile);
    }
}

//This function inserts all the nodes from the tree into the Table
//In order method is used (left node right)
void InsertAllToHash(AVL t)
{
    if(t!=NULL)
    {
        InsertAllToHash(t->Left);
        InsertNodeToHash(t->info,Table, TableSize,0);
        InsertAllToHash(t->Right);
    }
    else
    {
        return;
    }
}

//Print the size of the table
void PrintSizeOfTable()
{
    printf("\tThe size of the table is %d\n\n", TableSize);
}

//prints the method used to calculate the index with the equation
void PrintHashFunction()
{
    printf("\tDouble hashing is the method used in this project\n");
    printf("\tEquation: index = (h + i * hp) %% tableSize , where i is: 0,1,2,...,tableSize-1\n");
    printf("\n\th = key %% tableSize\n");
    printf("\thp = 1 + (key %% (tableSize - 1))\n");
    printf("\n\tThe whole equation is as follows:\n");
    printf("\tIndex = ((key %% tableSize) + i * (1 + (key %% (tableSize - 1))) ) %% tableSize, where i is: 0,1,2,...,tableSize-1\n\n");
}

//This function finds a record of a patient and prints the output to the console
void FindPatientInTable()
{
    char patientNameToFind[SIZE];
    printf("\n\tEnter the name of the patient you want to find:\n\t");
    scanf("\n\t");
    gets(patientNameToFind);
    int indexOfFoundRecord = FindInHash(patientNameToFind);
    if(indexOfFoundRecord!=-1)
    {
        printf("\tPatient was Found:\n");
        printf("      Index     Status\t       Patient Name             Gender       Date Of Admission        Date Of Birth           Illness               Address         Blood Type\n\n");

        PrintIndexInHash(indexOfFoundRecord, 0);
    }
    else
    {
        printf("\tPatient record does not exist in the table\n");
    }
}

//The menu that shows the user the list of operations
void HashMenu()
{
    printf("\t----------------------------------------Hash Menu------------------------------------------\n");
    printf("\n\t Choose one of the following.\n\n") ;
    printf("\t (1): Print Hashed Table.\n\n") ;
    printf("\t (2): Print Table Size.\n\n") ;
    printf("\t (3): Print Used Hash Function.\n\n") ;
    printf("\t (4): Insert a New Record to The Hash Table.\n\n") ;
    printf("\t (5): Search For a Specific Patient.\n\n") ;
    printf("\t (6): Delete a Specific Record.\n\n") ;
    printf("\t (7): Save Table Back to File (patients_hash.data).\n\n") ;
    printf("\t (8): Exit from the program.\n\n");
    printf("\t--------------------------------------------------------------------------------------\n");
}

//This function performs the Hash options shown in the menu to the user
void HashWork()
{
    //evaluate tableSize and create the table
    //!
    //!Here I use ceil for when the result of the division is a prime and fraction, so the program will not need rehashing while inserting the last node in the tree
    //!
    TableSize = GeneratePrime(ceil(sizeOfTree/0.7),1);
    Table = CreateTable(TableSize);
    numberOfItems=sizeOfTree;
    //Inser all nodes from tree to hash table
    InsertAllToHash(Tree);
    printf("\n\n\tThe Hash Table is Created and The Records Saved in The Tree are Loaded to The Table.\n");
    char select;
    int selection;
    while(1)
    {
        HashMenu();
        printf("\tEnter Your Selection:\n\t");
        scanf("%s", &select);
        if(strncmp(&(select),"0",1)>=0 && strncmp(&(select),"8",1)<=0)
        {
            selection = atoi(&(select));
        }
        else
        {
            ClearScreen();
            printf("\tInvalid choice. You can only enter digits from 1 to 8\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            continue;
        }

        switch(selection)
        {
        case 1:
            ClearScreen();
            printf("\n\t...........You have chosen option 1: Print Hashed Table...........\n\n");
            PrintTable(0);
            printf("\t Printing Operation is Done.\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 2:
            ClearScreen();
            printf("\n\t...........You have chosen option 2: Print Table Size...........\n\n");
            PrintSizeOfTable();
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 3:
            ClearScreen();
            printf("\n\t...........You have chosen option 3: Print Used Hash Function...........\n\n");
            PrintHashFunction();
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 4:
            ClearScreen();
            printf("\n\t...........You have chosen option 4: Insert a New Record to The Hash Table...........\n\n");
            InsertRecordToTable();
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 5:
            ClearScreen();
            printf("\n\t...........You have chosen option 5: Search For a Specific Patient...........\n\n");
            FindPatientInTable();
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 6:
            ClearScreen();
            printf("\n\t...........You have chosen option 6: Delete a Specific Record...........\n\n");
            DeleteInHash();
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 7:
            ClearScreen();
            printf("\n\t...........You have chosen option 7: Save Table Back to File (patients_hash.data)...........\n");
            fOutPtr = fopen("patients_hash.data.txt", "w");
            PrintTable(1);
            fclose(fOutPtr);
            printf("\tSaving The Contents To The File Is Complete\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            printf("\tThank You For Using My Program\n");
            exit(0);
            break;
        case 8:
            printf("\tThank You For Using My Program\n");
            exit(0);
            break;
        default:
            ClearScreen();
            printf("\tWrong selection. Try again Please.\n");
            printf("\t\t_______________________________________________________________________________\n\n");
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    AVLWork();

    return 0;
}
