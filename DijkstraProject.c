#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<unistd.h>
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

//struct to make the graph from
struct graphNode
{
    int weight;
    char town [100];
};

typedef struct graphNode* edge;
//variable that holds the size of graph (maximum number of nodes in the file)
int numOfNodes = 0;
edge **graph;

//function to check if a string is a number
int isNumber(char* s)
{
    for (int i = 0; i < strlen(s); i++)
        if (!isdigit(s[i]))
            return 0;

    return 1;
}

//function to read from the file and save the result to the graph array
void ReadFromFile()
{
    //Read the first time to calculate the maximum nodes number
    FILE *fp;
    fp = fopen("segments.txt", "r");
    while(!feof(fp))
    {
        char line[500];
        int v1, v2;
        fgets(line, 500, fp);
        char* token;
        token = strtok(line, " ");
        v1 = atoi(token);
        token = strtok(NULL, " ");
        v2 = atoi(token);

        numOfNodes = MAX(MAX(numOfNodes, v1), v2);
    }
    fclose(fp);
    numOfNodes++;

    //create graph and reserve space for all the nodes
    graph = (edge**)malloc(numOfNodes*sizeof(struct graphNode));
    for(int i=0; i<numOfNodes; i++)
    {
        graph[i] = (edge*)malloc(numOfNodes*sizeof(struct graphNode));
        for(int j=0; j<numOfNodes; j++)
        {
            graph[i][j] = (edge)malloc(sizeof(struct graphNode));
            graph[i][j]->weight=0;
            strcpy(graph[i][j]->town, "");
        }
    }

    //Second time reading to fill the graph
    fp = fopen("segments.txt", "r");

    while(!feof(fp))
    {
        char line[500];
        int v1, v2, cost;
        char town[300] = "";
        fgets(line, 500, fp);
        char* token;
        token = strtok(line, " ");
        v1 = atoi(token);
        token = strtok(NULL, " ");
        v2 = atoi(token);
        token = strtok(NULL, " ");

        while(token != NULL)
        {
            if(!isNumber(token))
            {
                if(strcmp(town, "")!=0)
                    strcat(town," ");
                strcat(town, token);
            }
            else
            {
                cost = atoi(token);
            }
            token = strtok(NULL, " \n");
        }
        graph[v1][v2]->weight=cost;
        graph[v2][v1]->weight=cost;
        strcpy(graph[v1][v2]->town, town);
        strcpy(graph[v2][v1]->town, town);
    }
    fclose(fp);

}

//find the minimum distance
int least(int dist[], int visited[])
{
    int min = 99999;
    int indexOfMin;

    for (int v=0; v<numOfNodes; v++)
    {
        if (visited[v] == 0 && dist[v] <= min)
            min = dist[v], indexOfMin = v;
    }

    return indexOfMin;
}

//print the result of the operation by backtracking the process (to the console)
void printResult(int j, int prev[])
{
    if(prev[j]==-1)
    {
        return;
    }
    printResult(prev[j], prev);
    printf("\t%d ->\t%-20s -> %d\n",prev[j],graph[j][prev[j]]->town, j);
}

//print the result of the operation by backtracking the process (to the file)
void fPrintResult(int j, int prev[], FILE* fp)
{
    if(prev[j]==-1)
    {
        return;
    }
    fPrintResult(prev[j], prev, fp);
    fprintf(fp, "\t%d ->\t%-20s -> %d\n",prev[j],graph[j][prev[j]]->town, j);
}

//Dijkstra algorithm that calculates the minimum distance from a vertex to another
void Dijkstra(int from, int to)
{
    int visited[numOfNodes];
    int dist[numOfNodes];
    int prev[numOfNodes];
    for(int i=0; i<numOfNodes; i++)
    {
        visited[i]=0, dist[i]=99999, prev[i]=-1;
    }
    dist[from]=0;
    for(int i=0; i<numOfNodes-1; i++)
    {
        int u = least(dist, visited);
        if(dist[u]==99999)
        {
            break;
        }
        visited[u]=1;
        for(int j=0; j<numOfNodes; j++)
        {
            if (visited[j]==0 && graph[u][j]->weight !=0 && dist[u] != 99999 && dist[u] + graph[u][j]->weight < dist[j])
            {
                dist[j] = dist[u] + graph[u][j]->weight;
                prev[j]=u;
            }

        }
    }
    //print the cost and the towns past either to the console or to the file
    if(visited[to]!=0)
    {
        printf("\n\tThe cost needed to go from vertex %d to vertex %d is: %d\n", from, to, dist[to]);
        printf("\n\tThe following path is followed:\n");
    }
    else
    {
        printf("\n\tThere is no way from vertex %d to vertex %d\n", from, to);
    }

    printResult( to, prev);

    printf("\n\t Enter 1 if you want to save the data in the file: ");
    int save;
    scanf("%d", &save);
    if(save==1)
    {
        FILE*fp;
        fp = fopen("route.txt", "a");
        if(visited[to]!=0)
        {
            fprintf(fp, "The cost needed to go from vertex %d to vertex %d is: %d\n", from, to, dist[to]);
        }
        else
        {
            fprintf(fp, "There is no way from vertex %d to vertex %d\n", from, to);
        }

        fprintf(fp, "The following path is followed:\n");
        fPrintResult( to, prev, fp);
        fprintf(fp, "\n\n");
        fclose(fp);
        printf("\n\tData is Saved to the file successfully\n");
    }

}

//Menu to be shown to the user
void Menu()
{
    printf("\t----------------------------------------Menu------------------------------------------\n");
    printf("\n\t Choose one of the following.\n\n") ;
    printf("\t (1): Read the file and Fill the graph.\n\n") ;
    printf("\t (2): Find the shortest path between two vertices using Dijkstra Algorithm.\n\n") ;
    printf("\t (3): Exit from the program.\n\n");
    printf("\t--------------------------------------------------------------------------------------\n");
}

//This functions clears the screen for easier using of the program
void ClearScreen()
{
    system("cls");
    printf("\n\t\t_______________________________________________________________________________\n\n");
}

//Function that empties the file to start writing data with each run
void EmptyFile()
{
    if( access( "route.txt", F_OK ) == 0 )
    {
        if (remove("route.txt") == 0)
            printf("Deleted successfully");
        else
            printf("Unable to delete the file");
    }
}

int main()
{
    system("color f0");
    EmptyFile();
    char select;
    int selection;
    int didReading=0;
    //loop for selections
    while(1)
    {
        Menu();
        printf("\tEnter Your Selection:\n\t");
        scanf("%s", &select);
        if(strncmp(&(select),"0",1)>=0 && strncmp(&(select),"3",1)<=0)
        {
            selection = atoi(&(select));
        }
        else
        {
            ClearScreen();
            printf("\tInvalid choice. You can only enter digits from 1 to 3\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            continue;
        }

        switch(selection)
        {
        case 1:
            ClearScreen();
            printf("\n\t...........You have chosen option 1: Read the file and Fill the graph...........\n\n");
            ReadFromFile();
            didReading=1;
            printf("\t The data is read and filled in the graph successfully.\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 2:
            ClearScreen();
            printf("\n\t...........You have chosen option 2: Find the shortest path between two vertices using Dijkstra Algorithm...........\n\n");
            //chack if the user want to read the file
            if(!didReading)
            {
                printf("\tYou need to read the file and fill the graph before choosing this option\n");
                printf("\tEnter 1 if you want to read the file: ");
                char doReading;
                scanf("%s", &doReading);
                if(strncmp(&(doReading),"1",1)==0)
                {
                    ReadFromFile();
                    didReading=1;
                    printf("\n\tFile is successfully read\n");
                }
                else
                {
                    printf("\tOperation was not complete. Please read the file first\n");
                    continue;
                }
            }
            //loop to read two vertices each time and calculate the shortest path
            while(1)
            {
                int from,to;
                int fromFlag=0, toFlag=0;
                printf("\n\tPlease Enter the starting vertex:  ");
                scanf("%d", &from);
                printf("\tPlease Enter the vertex you want to arrive to:  ");
                scanf("%d", &to);

                if(from>=0 && from<=numOfNodes)
                {
                    fromFlag=1;
                }
                if(to>=0 && to<=numOfNodes)
                {
                    toFlag=1;
                }
                if(!fromFlag || !toFlag)
                {
                    printf("\n\tThe vertices you entered are not in the right format\n");
                }
                else
                {
                    Dijkstra(from, to);
                }
                printf("\n\tEnter -1 to stop searching: ");
                int stopReading;
                scanf("%d", &stopReading);
                if(stopReading==-1)
                {
                    printf("\n\tSearching for path operation stopped\n");
                    break;
                }
            }
            printf("\t\t_______________________________________________________________________________\n\n");
            break;
        case 3:
            ClearScreen();
            printf("\n\t...........You have chosen option 3: Exit from the program...........\n\n");
            printf("\tThank you For using my program\n");
            printf("\t\t_______________________________________________________________________________\n\n");
            exit(0);
            break;

        default:
            ClearScreen();
            printf("\tWrong selection. Try again Please.\n");
            printf("\t\t_______________________________________________________________________________\n\n");
        }
    }
    return 0;
}
