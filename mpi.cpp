#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <mpi.h>

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

const int INF = 1e7;
int rank, size;
int nodes;
bool negativeCycle = false;
ThreadStruct threadStruct;

void* threadBellmanFord(int firstNode, int lastNode) {

    for(int i = firstNode; i < lastNode; ++i)
        for (int k = 0; k < threadStruct.edges[i].v.size(); ++k)
            if (threadStruct.weights[threadStruct.edges[i].u] + threadStruct.edges[i].w[k] <
                threadStruct.weights[threadStruct.edges[i].v[k]]) {
                if (negativeCycle) {
                    printf("\nRank: %d, Graph contains a negative-weight cycle\n", rank);
                    exit(1);
                } else {
                    threadStruct.additional[threadStruct.edges[i].v[k]] =
                            threadStruct.weights[threadStruct.edges[i].u] + threadStruct.edges[i].w[k];
                }
            }
}


void BellmanFord(int src)
{
    int step = nodes/size;


    int firstNode = rank * step;
    int lastNode = rank == size - 1 ? nodes : firstNode + step;

    for (int i = 0; i < nodes + 1; ++i)
        threadStruct.weights.push_back(INF);
    threadStruct.weights[src] = 0;
    threadStruct.additional = threadStruct.weights;

    for(int i = 0; i < nodes; ++i){

        if(rank == 0) {
            printf("Step #(%i,%i)\r", i + 1, nodes);
            fflush(stdout);
        }

        negativeCycle = i == nodes;
        threadBellmanFord(firstNode, lastNode);

        MPI_Reduce(&threadStruct.additional[0], &threadStruct.weights[0],
                   threadStruct.additional.size(), MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

        MPI_Bcast(&threadStruct.weights[0], threadStruct.weights.size(), MPI_INT, 0, MPI_COMM_WORLD);
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

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    std::clock_t start;

    fileOpen(threadStruct);

    int source_vertex = 0;

    start = std::clock();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    BellmanFord(source_vertex);

    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

    if (rank == 0) {
        std::cout << std::endl;
        for (int i = 0; i < nodes; ++i)
            std::cout << "Node: " << i << " - " << threadStruct.weights[i] << "\n";

        std::cout << std::endl;

        std::cout << "MPI execution time: " << duration;
    }

    MPI_Finalize();
    return 0;
}
