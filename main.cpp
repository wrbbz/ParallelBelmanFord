#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

typedef struct {
    int u;
    std::vector<int>v;
    std::vector<int>w;
} Edge;

const int INFINITY = 1e7;
int nodes;

void BellmanFord(int src, std::vector<Edge> &edges, std::vector<int> &weights)
{
    for (int i = 0; i < nodes; ++i)
        weights.push_back(INFINITY);
    weights[src] = 0;

    for (int i = 0; i < nodes; ++i) {
        printf("Step # %i/%i\r", i + 1, nodes);
	fflush(stdout);
        for (int j = 0; j < edges.size(); ++j) {
            for (int k = 0; k < edges[j].v.size(); ++k)
                if (weights[edges[j].u] + edges[j].w[k] < weights[edges[j].v[k]]) {
                    weights[edges[j].v[k]] = weights[edges[j].u] + edges[j].w[k];
                }
        }
    }

    for (int i = 0; i < nodes; ++i) {
        for (int j = 0; j < edges.size(); ++j) {
            for(int k = 0; k < edges[j].v.size(); ++k) {
                if (weights[edges[j].u] + edges[j].w[k] < weights[edges[j].v[k]]) {
                    std::cout << "Graph contains a negative-weight cycle\n";
                    exit(1);
                }
            }
        }
    }
}

void fileOpen(std::vector<Edge> &edges){
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
            edges.push_back(edge);
        }
    }
    bell.close();
}

int main()
{
    std::clock_t start;
    std::vector<Edge> edges;
    std::vector<int> d;

    fileOpen(edges);

    int source_vertex = 0;

    start = std::clock();

    BellmanFord(source_vertex, edges, d);

    double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout << std::endl;
    for (int i = 0; i < nodes; ++i)
        std::cout << "Node: " << i << " - " << d[i] <<"\n";

    std::cout << std::endl;

    std::cout << "Sequential execution time: " << duration;

    return 0;
}
