#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>

typedef struct {
    int u;
    std::vector<int>v;
    std::vector<int>w;
} Edge;

typedef struct {
    std::vector<Edge> edges;
    std::vector<int> weights;
    std::vector<int> additional;
} ThreadStruct;

typedef struct {
    int threadNo;
    int firstNode;
    int lastNode;
} AdditionalStruct;

const int INF = 1e7;
int nodes;
bool negativeCycle = false;
ThreadStruct threadStruct;

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS               0
#define arg ((AdditionalStruct *)args)

void* threadBellmanFord(void *args) {
    for(int i = arg->firstNode; i < arg->lastNode; ++i)
        for (int k = 0; k < threadStruct.edges[i].v.size(); ++k)
            if (threadStruct.weights[threadStruct.edges[i].u] + threadStruct.edges[i].w[k] <
                    threadStruct.weights[threadStruct.edges[i].v[k]]) {
                if (negativeCycle) {
                    printf("\nGraph contains a negative-weight cycle\n");
                    exit(1);
                } else {
                    threadStruct.additional[threadStruct.edges[i].v[k]] =
                            threadStruct.weights[threadStruct.edges[i].u] + threadStruct.edges[i].w[k];
                }
            }
}


void bellmanFord(int src)
{
    int status, status_addr;
    int numthreads = 0;
    int step = 2;
	for(int j = 0; j < nodes; j+=step)
      ++numthreads;
    std::vector<pthread_t> threads(numthreads);
    std::vector<AdditionalStruct> addStr(threads.size());
    printf("Threads.size: %i\n", threads.size());

    for (int i = 0; i < nodes; ++i)
        threadStruct.weights.push_back(INF);
    threadStruct.weights[src] = 0;

    threadStruct.additional = threadStruct.weights;
    for(int i = 0; i < nodes + 1; ++i){

        printf("Step #(%i,%i)\r", i + 1, nodes + 1);
        fflush(stdout);
        negativeCycle = i == nodes;

        for(int j = 0, threadNo = 0; j < nodes; j += step, ++threadNo){
            addStr[threadNo].threadNo = threadNo;
            addStr[threadNo].firstNode = j;
            addStr[threadNo].lastNode = (j + step < nodes) ?
                                    j + step : nodes;

            status = pthread_create(&threads[threadNo], NULL, threadBellmanFord,
                                    (void *) &addStr[threadNo]);
            if (status != SUCCESS) {
                printf("\nCan not create thread. Status: %d\n", status);
                exit(ERROR_CREATE_THREAD);
            }
        }

        for (int j = 0; j < threads.size(); ++j) {
            status = pthread_join(threads[j], (void **) &status_addr);
            if (status != SUCCESS) {
                printf("\nCan not join thread. Status: %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
        }
        threadStruct.weights = threadStruct.additional;

    }
}

void fileOpen(ThreadStruct &threadStruct){
    std::fstream bell;
    int w;
    bell.open("Test//bellman.txt");
    if(bell.is_open()) {
        bell >> nodes;

        if (nodes <= 0)
            return;

        for (int i = 0; i < nodes; ++i) {
            Edge edge;
            edge.u = i;
            for (int j = 0; j < nodes; ++j) {
                bell >> w;
                if (w != 0) {
                    edge.v.push_back(j);
                    edge.w.push_back(w);
                }
            }
            threadStruct.edges.push_back(edge);
        }
    }
    bell.close();
}

int main()
{

    fileOpen(threadStruct);

    int source_vertex = 0;

    std::clock_t start;
    start = std::clock();

    bellmanFord(source_vertex);

    double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout << std::endl;
    for (int i = 0; i < nodes; ++i)
        std::cout << "Node: " << i << " - " << threadStruct.weights[i] <<"\n";

    std::cout << std::endl;

    std::cout << "Pthreads execution time: " << duration;

    return 0;
}
