#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>
 
// Number of vertices in the graph
#define V 9

#define forE(X) for(int i = 0;i < X; i++)
#define getMin(array) for(int i = 0;i < X; i++){ if(array[i] < distance[i + pid*3]){distance[i + pid*3] = array[i]; }
 
// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(int dist[], bool sptSet[])
{
    // Initialize min value
    int min = INT_MAX, min_index;
    
    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}
 
// A utility function to print the constructed distance array
void printSolution(int dist[])
{
    printf("Vertex \t\t Distance from Source\n");
    for (int i = 0; i < V; i++)
        printf("%d \t\t %d\n", i, dist[i]);
}
 
// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int SOURCE, int n, int graph[V][V], int src)
{
    n = V;
 
    int dist[V]; // The output array.  dist[i] will hold the shortest
    // distance from src to i
 
    bool sptSet[V]; // sptSet[i] will be strue if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized
 
    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX, sptSet[i] = false;
 
    // Distance of source vertex from itself is always 0
    dist[src] = 0;
 
    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(dist, sptSet);
 
        // Mark the picked vertex as processed
        sptSet[u] = true;
 
        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < V; v++)
 
            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
                && dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }
 
    // print the constructed distance array
    printSolution(dist);
}
 
// driver program to test above function
int main()
{
    //3 processes
    int pid, np, mtag, count, root = 0;
    MPI_Status status;
    MPI_Request req_s;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);//np always set to 3
    
    int source = 0;
    /* Let us create the example graph discussed above */
    int graph[V][V] = { { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
                        { 4, 0, 8, 0, 0, 0, 0, 11, 0 },
                        { 0, 8, 0, 7, 0, 4, 0, 0, 2 },
                        { 0, 0, 7, 0, 9, 14, 0, 0, 0 },
                        { 0, 0, 0, 9, 0, 10, 0, 0, 0 },
                        { 0, 0, 4, 14, 10, 0, 2, 0, 0 },
                        { 0, 0, 0, 0, 0, 2, 0, 1, 6 },
                        { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
                        { 0, 0, 2, 0, 0, 0, 6, 7, 0 } };
 
    int temporarySmallArray[V/3 + pid*3][10]; //3 separate sections;

    /* //temporary small chunk either collect these three (process 1 contains the second one listed. process 3 contains the first one)
    { 0, 0, 7, 0, 9, 14, 0, 0, 0 }, |  { 0, 4, 0, 0, 0, 0, 0, 8, 0 },  | { 0, 0, 0, 0, 0, 2, 0, 1, 6 },
    { 0, 0, 0, 9, 0, 10, 0, 0, 0 }, |  { 4, 0, 8, 0, 0, 0, 0, 11, 0 }, | { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
    { 0, 0, 4, 14, 10, 0, 2, 0, 0 },|  { 4, 0, 8, 0, 0, 0, 0, 11, 0 }, | { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
    */

    forE(3){
        for(int j = 0; j<10; i++){
            /*
            Setting the first chunk of the graph stored in temporarySmallArray's values
                { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
                { 4, 0, 8, 0, 0, 0, 0, 11, 0 },
                { 0, 8, 0, 7, 0, 4, 0, 0, 2 },
            */
            temporarySmallArray[i][j] = graph[i+pid*3][j];
        }
    }
    
    if(pid=0){
        MPI_RECV(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
        getMin(array)
        MPI_RECV(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
        getMin(array)
        MPI_RECV(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
        getMin(array)
    }
    if(pid=1){
        dijkstra(0,V,temporarySmallArray,0);
        MPI_SEND(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
    }
    if(pid=2){
        dijkstra(0,V,temporarySmallArray,0);
        MPI_SEND(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
    }
    if(pid=3){
        dijkstra(0,V,temporarySmallArray,0);
        MPI_SEND(array, 10, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
    }
    
    
 
    return 0;
}
