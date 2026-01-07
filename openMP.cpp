/*
Este archivo contiene las implementaciones de 
*/
#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
#include <fstream>
#include <limits>
#include <omp.h>
#define MAX_THREADS 8
using namespace std;
const double INF = numeric_limits<double>::infinity();
void floydWarshallOMP(vector<vector<double>> &dist) {
    int V = dist.size();
    for (int k = 0; k < V; k++) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if(dist[i][k] != 1e8 && dist[k][j]!= 1e8)
                dist[i][j] = min(dist[i][j],dist[i][k] + dist[k][j]);
            }
        }
    }
}
/*
Código modificado de Geeks for Geeks:
https://www.geeksforgeeks.org/dsa/floyd-warshall-algorithm-dp-16/
*/

void floydWarshallOMPOptimized(vector<double> &dist, int V) {
    // Los hilos se crean una sola vez.
    #pragma omp parallel num_threads(MAX_THREADS)
    {
        for (int k = 0; k < V; k++) {
            #pragma omp for schedule(static)
            for (int i = 0; i < V; i++) {
                
                double dist_ik = dist[i * V + k];
                if (dist_ik == INF) continue;
                #pragma omp simd
                for (int j = 0; j < V; j++) {
                    double &res = dist[i * V + j];
                    double sum = dist_ik + dist[k * V + j];
                    if (sum < res) {
                        res = sum;
                    }
                }
            }
        }
    }
}
/*
Optimizaciones:
->uso de matriz aplanada
->Buenas prácticas de openMP para evitar overhead
*/

int main(void){

}




