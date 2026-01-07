#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
#include <fstream>
#include <limits>
#include <omp.h>
#define B 16
using namespace std;
const double INF = numeric_limits<double>::infinity();

vector<vector<double>> leerGrafo(string nombreArchivo) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        // Devolvemos una matriz vacía en caso de error
        return {};
    }

    int numVertices;
    long long numAristas;

    // 1. Leer la cabecera (N y M)
    // Tu generador usa espacios: "numVertices numAristas"
    archivo >> numVertices >> numAristas;

    cout << "Leyendo grafo de " << numVertices << " vertices..." << endl;

    // 2. Inicializar la matriz
    // Llenamos todo con INF inicialmente
    vector<vector<double>> matriz(numVertices, vector<double>(numVertices, INF));

    // 3. Establecer la diagonal principal en 0
    for (int i = 0; i < numVertices; ++i) {
        matriz[i][i] = 0.0;
    }

    // 4. Leer las aristas y llenar la matriz
    int u, v;
    double w;

    // Leemos hasta que se acabe el archivo o hayamos leido numAristas
    // Tu formato es: origen destino peso
    while (archivo >> u >> v >> w) {
        // Validación de rango por seguridad
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            
            // IMPORTANTE: Como es un generador aleatorio, podría generar 
            // la misma arista dos veces. Nos quedamos con el peso menor.
            if (w < matriz[u][v]) {
                matriz[u][v] = w;
            }

            // NOTA: Tu generador crea un grafo DIRIGIDO (A->B no implica B->A).
            // Si quieres que sea NO DIRIGIDO, descomenta la siguiente línea:
            // if (w < matriz[v][u]) matriz[v][u] = w; 
        }
    }

    archivo.close();
    cout << "Lectura finalizada." << endl;
    
    return matriz;
}

vector<double> leerGrafoAplanado(string nombreArchivo) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return {};
    }

    int numVertices;
    long long numAristas;

    archivo >> numVertices >> numAristas;

    cout << "Leyendo grafo de " << numVertices << " vertices..." << endl;

    vector<double> matriz((numVertices*numVertices),INF);

    //diagonal principal en 0
    for (int i = 0; i < numVertices; ++i) {
        matriz[i*numVertices+i] = 0.0;
    }

    int u, v;
    double w;

    // Leemos hasta que se acabe el archivo o hayamos leido numAristas
    // Tu formato es: origen destino peso
    while (archivo >> u >> v >> w) {
        // Validación de rango por seguridad
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            int index = u*numVertices+v;
            if (w < matriz[index]) {
                matriz[index] = w;
            }
        }
    }

    archivo.close();
    cout << "Lectura finalizada." << endl;
    
    return matriz;
}
// Solves the all-pairs shortest path
// problem using Floyd Warshall algorithm
// obtenido de Geeks for geeks
void floydWarshall(vector<vector<double>> &dist) {
    int V = dist.size();

    // Add all vertices one by one to
    // the set of intermediate vertices.
    for (int k = 0; k < V; k++) {

        // Pick all vertices as source one by one
        for (int i = 0; i < V; i++) {

            // Pick all vertices as destination
            // for the above picked source
            for (int j = 0; j < V; j++) {

                // shortest path from
                // i to j 
                if(dist[i][k] != INF && dist[k][j]!= INF)
                dist[i][j] = min(dist[i][j],dist[i][k] + dist[k][j]);
            }
        }
        
    }
}

void floydWarshallSecuencialOptimizado(vector<double>& dist, int V) {

    for (int k = 0; k < V; k++) {
        // Optimización de acceso a la fila K:
        double* rowK = &dist[k * V];
        for (int i = 0; i < V; i++) {
            // Puntero base a la fila I
            double* rowI = &dist[i * V];
            // Si no hay camino de i->k, continuar
            if (rowI[k] == INF) continue;
            // Guardamos el valor para acceso rapido
            double dist_ik = rowI[k];
            //instrucciones SIMD
            for (int j = 0; j < V; j++) {
                double new_dist = dist_ik + rowK[j];
                if (new_dist < rowI[j]) {
                    rowI[j] = new_dist;
                }
            }
        }
    }
}

void floydWarshallOMP(vector<vector<double>> &dist) {
    int V = dist.size();

    // Add all vertices one by one to
    // the set of intermediate vertices.
    for (int k = 0; k < V; k++) {

        // Pick all vertices as source one by one
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < V; i++) {

            // Pick all vertices as destination
            // for the above picked source
            for (int j = 0; j < V; j++) {

                // shortest path from
                // i to j 
                if(dist[i][k] != 1e8 && dist[k][j]!= 1e8)
                dist[i][j] = min(dist[i][j],dist[i][k] + dist[k][j]);
            }
        }
        
    }
}

void floydWarshallOMPOptimized(vector<double> &dist, int V) {
    // Los hilos se crean una sola vez.
    #pragma omp parallel
    {
        for (int k = 0; k < V; k++) {
            #pragma omp for schedule(static)
            for (int i = 0; i < V; i++) {
                
                double dist_ik = dist[i * V + k];
                if (dist_ik == INF) continue;
                // Bucle vectorizable
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


#include <vector>
#include <algorithm>
#include <immintrin.h>
#include <cstring> // Para memcpy si es necesario
#include <climits>



// Versión corregida de update_block
void update_block(vector<double>&dist, int N, int r_i, int r_j, int r_k, int block_k) {
    int i_end = std::min(r_i + B, N);
    int j_end = std::min(r_j + B, N);
    int k_end = std::min(r_k + B, N);
    
    for (int k = block_k; k < block_k + B && k < N; ++k) {
        for (int i = r_i; i < i_end; ++i) {
            double dik = dist[i * N + k];
            if (dik == INF) continue;
            
            // Optimización: precargar fila i en localidad temporal
            double* dist_i = &dist[i * N];
            
            #pragma omp simd aligned(dist:64)
            for (int j = r_j; j < j_end; ++j) {
                double new_dist = dik + dist[k * N + j];
                if (new_dist < dist_i[j]) {
                    dist_i[j] = new_dist;
                }
            }
        }
    }
}

void blocked_floyd_warshall(vector<double>& dist, int N) {
    // Asegurar que los bloques no excedan N
    int blocks = (N + B - 1) / B;
    
    for (int kb = 0; kb < blocks; ++kb) {
        int k_start = kb * B;
        int k_end = std::min(k_start + B, N);
        
        // Fase 1: Bloque diagonal (actualización dentro del bloque k)
        update_block(dist, N, k_start, k_start, k_start, k_start);
        
        // Fase 2: Bloques en la misma fila y columna
        for (int ib = 0; ib < blocks; ++ib) {
            if (ib == kb) continue;
            int i_start = ib * B;
            
            // Columnas del bloque k para filas i
            update_block(dist, N, i_start, k_start, k_start, k_start);
            
            // Filas del bloque k para columnas j
            update_block(dist, N, k_start, i_start, k_start, k_start);
        }
        
        // Fase 3: Resto de la matriz
        for (int ib = 0; ib < blocks; ++ib) {
            if (ib == kb) continue;
            for (int jb = 0; jb < blocks; ++jb) {
                if (jb == kb) continue;
                
                int i_start = ib * B;
                int j_start = jb * B;
                
                update_block(dist, N, i_start, j_start, k_start, k_start);
            }
        }
    }
}

void ejecutar(vector<string> archivos,string salida){
    ofstream archivoSalida(salida);
    for(int i=0;i<archivos.size();i++){
        ifstream entrada(archivos[i]);
        vector<vector<double>>grafo = leerGrafo(archivos[i]);
        auto inicio=chrono::high_resolution_clock::now();
        floydWarshallOMP(grafo);
        auto fin=chrono::high_resolution_clock::now();
        chrono::duration<double> duracion = fin-inicio;
        archivoSalida<<duracion.count() <<endl;
        entrada.close();
    }
    archivoSalida.close();

}

void ejecutar2(vector<string> archivos,string salida,int tam){
    ofstream archivoSalida(salida);
    for(int i=0;i<archivos.size();i++){
        ifstream entrada(archivos[i]);
        vector<double>grafo = leerGrafoAplanado(archivos[i]);
        auto inicio=chrono::high_resolution_clock::now();
        //floydWarshallSecuencialOptimizado(grafo,tam);
        //blocked_floyd_warshall(grafo,tam);
        floydWarshallOMPOptimized(grafo,tam);
        auto fin=chrono::high_resolution_clock::now();
        chrono::duration<double> duracion = fin-inicio;
        archivoSalida<<duracion.count() <<endl;
        entrada.close();
    }
    archivoSalida.close();
}

int main() {
    
    
    vector<vector<double>> dist = {
        {0, 4, -2, INF},
        {4, 0, 3, INF},
        {INF, 3, 0, 1},
        {INF, INF, 1, 0}
    };
    vector<double> dist1 = {0, 4, -2, INF,4, 0, 3, INF,INF, 3, 0, 1,INF, INF, 1, 0};
    //floydWarshall(dist);
    blocked_floyd_warshall(dist1,4);
    cout<<"Resultado del algoritmo\n";
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
        cout<<dist1[i*4 +j]<<" ";
        }
        cout<<"\n";
    }
    vector<string> a = {"512_100_1.txt","512_100_2.txt","512_100_3.txt","512_50_1.txt","512_50_2.txt","512_50_3.txt","512_25_1.txt","512_25_2.txt","512_25_3.txt"};
    vector<string> b = {"1024_100_1.txt","1024_100_2.txt","1024_100_3.txt","1024_50_1.txt","1024_50_2.txt","1024_50_3.txt","1024_25_1.txt","1024_25_2.txt","1024_25_3.txt"};
    vector<string> c = {"2048_100_1.txt","2048_100_2.txt","2048_100_3.txt","2048_50_1.txt","2048_50_2.txt","2048_50_3.txt","2048_25_1.txt","2048_25_2.txt","2048_25_3.txt"};
    vector<string> d = {"4096_100_1.txt","4096_100_2.txt","4096_100_3.txt","4096_50_1.txt","4096_50_2.txt","4096_50_3.txt","4096_25_1.txt","4096_25_2.txt","4096_25_3.txt"};
    vector<string> e = {"8192_100_1.txt","8192_100_2.txt","8192_100_3.txt","8192_50_1.txt","8192_50_2.txt","8192_50_3.txt","8192_25_1.txt","8192_25_2.txt","8192_25_3.txt"};
    //ejecutar(a,"tiempos_1024_serial.txt");
    //ejecutar(b,"tiempos_2048_serial.txt");
    ejecutar(a,"tiempos_512_OMP1.txt");
    ejecutar(b,"tiempos_1024_OMP1.txt");
    ejecutar(c,"tiempos_2048_OMP1.txt");
    ejecutar(d,"tiempos_4096_OMP1.txt");
    ejecutar(e,"tiempos_8192_OMP1.txt");
    //ejecutar2(a,"tiempos_512_serial_opt.txt",512);
    //ejecutar2(c,"tiempos_2048_serial_opt.txt",2048);
    //ejecutar2(e,"tiempos_8192_serial_opt.txt",8192);
    
    /*
    ofstream salida("tiempoSerialOpt_1024.txt");
    vector<double> dist = leerGrafoAplanado("1024_100_3.txt");
    //vector<vector<double>> dist = leerGrafo("1024_100_3.txt");
    auto inicio=chrono::high_resolution_clock::now();
    floydWarshallSecuencialOptimizado(dist,1024);
    auto fin=chrono::high_resolution_clock::now();
    chrono::duration<double> duracion = fin-inicio;
    salida<<duracion.count() <<endl;
    salida.close();
    */
    return 0;
}