#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iomanip>
#include <random>
#include <algorithm>
#include <sstream>
#include <unordered_set>
using namespace std;
const double INF = numeric_limits<double>::infinity();
void generarGrafoDisperso(int numVertices, unsigned long long numAristas, 
                          double pesoMin, double pesoMax, string nombreArchivo,
                          mt19937_64& gen) {
    ofstream archivo(nombreArchivo, ios::binary | ios::out);
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo." << endl;
        return;
    }
    
    archivo.sync_with_stdio(false);
    archivo << numVertices << " " << numAristas << "\n";
    
    unordered_set<long long> aristasGeneradas;
    aristasGeneradas.reserve(numAristas * 1.2);
    
    uniform_int_distribution<int> distVertices(0, numVertices - 1);
    uniform_real_distribution<double> distPesos(pesoMin, pesoMax);
    
    unsigned long long porcentaje = numAristas / 100;
    if (porcentaje == 0) porcentaje = 1;
    
    for (unsigned long long i = 0; i < numAristas; ++i) {
        int origen, destino;
        long long codigoArista;
        
        // Buscar una arista no generada
        do {
            origen = distVertices(gen);
            destino = distVertices(gen);
            
            // Asegurar origen < destino para evitar duplicados (grafo no dirigido)
            if (origen > destino) swap(origen, destino);
            
            // Codificar la arista en un solo número largo
            codigoArista = (long long)origen * numVertices + destino;
            
        } while (origen == destino || aristasGeneradas.count(codigoArista));
        
        aristasGeneradas.insert(codigoArista);
        double peso = distPesos(gen);
        
        archivo << origen << " " << destino << " " 
                << fixed << setprecision(4) << peso << "\n";
        
        if (i % porcentaje == 0) {
            cout << "Progreso: " << (i * 100) / numAristas << "% (" 
                 << i << "/" << numAristas << " aristas)\r" << flush;
        }
    }
    
    archivo.close();
    cout << "\nGeneracion completada. Archivo: " << nombreArchivo << endl;
}

void generarGrafoDenso(int numVertices, unsigned long long numAristas,
                       double pesoMin, double pesoMax, string nombreArchivo,
                       mt19937_64& gen) {
    // Método más eficiente para grafos densos
    ofstream archivo(nombreArchivo, ios::binary | ios::out);
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo." << endl;
        return;
    }
    
    archivo.sync_with_stdio(false);
    archivo << numVertices << " " << numAristas << "\n";
    
    // Generar todas las aristas posibles (triangular superior)
    vector<pair<int, int>> todasAristas;
    unsigned long long maxPosibles = (unsigned long long)numVertices * (numVertices - 1) / 2;
    todasAristas.reserve(maxPosibles);
    
    cout << "Preparando " << maxPosibles << " aristas posibles..." << endl;
    
    for (int i = 0; i < numVertices; ++i) {
        for (int j = i + 1; j < numVertices; ++j) {
            todasAristas.emplace_back(i, j);
        }
    }
    
    cout << "Mezclando aristas..." << endl;
    
    // Mezclar aleatoriamente usando Fisher-Yates
    for (unsigned long long i = maxPosibles - 1; i > 0; --i) {
        uniform_int_distribution<unsigned long long> dist(0, i);
        unsigned long long j = dist(gen);
        swap(todasAristas[i], todasAristas[j]);
    }
    
    cout << "Escribiendo aristas al archivo..." << endl;
    
    uniform_real_distribution<double> distPesos(pesoMin, pesoMax);
    unsigned long long porcentaje = numAristas / 100;
    if (porcentaje == 0) porcentaje = 1;
    
    for (unsigned long long i = 0; i < numAristas; ++i) {
        double peso = distPesos(gen);
        archivo << todasAristas[i].first << " " << todasAristas[i].second << " "
                << fixed << setprecision(4) << peso << "\n";
        
        if (i % porcentaje == 0) {
            cout << "Progreso: " << (i * 100) / numAristas << "%\r" << flush;
        }
    }
    
    archivo.close();
    cout << "\nGeneracion completada. Archivo: " << nombreArchivo << endl;
}
void generarGrafoOptimizado(int numVertices, int densidad, double pesoMin, double pesoMax, string nombreArchivo) {
    // Calcular número exacto de aristas sin superar el máximo posible
    unsigned long long maxAristas = (unsigned long long)numVertices * (numVertices - 1) / 2;
    unsigned long long numAristas = (maxAristas * densidad) / 100;
    
    if (numAristas > maxAristas) numAristas = maxAristas;
    
    cout << "Generando grafo con " << numVertices << " vertices y " << numAristas 
         << " aristas (" << densidad << "% de " << maxAristas << " posibles)..." << endl;

    // Inicializar generadores aleatorios
    random_device rd;
    mt19937_64 gen(rd());  // Usar mt19937_64 para más aleatoriedad
    uniform_real_distribution<double> distPesos(pesoMin, pesoMax);
    
    // Usar un set para evitar duplicados (estructura intermedia más eficiente)
    unordered_set<long long> aristasGeneradas;
    aristasGeneradas.reserve(numAristas * 1.1); // Reservar espacio para reducir rehashes
    
    // Generar todas las aristas posibles y seleccionar aleatoriamente
    if (densidad > 50) {
        // Para densidades altas: generar todas y eliminar aleatoriamente
        cout << "Usando metodo para grafo denso..." << endl;
        generarGrafoDenso(numVertices, numAristas, pesoMin, pesoMax, nombreArchivo, gen);
        return;
    } else {
        // Para densidades bajas: generar aleatoriamente
        cout << "Usando metodo para grafo disperso..." << endl;
        generarGrafoDisperso(numVertices, numAristas, pesoMin, pesoMax, nombreArchivo, gen);
        return;
    }
}




int main(void){
    random_device rd;
    mt19937_64 gen(rd());
    generarGrafoOptimizado(8192,100,1,1000,"8192_100_1");
}