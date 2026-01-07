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
void generarGrafoDirigidoDenso(int numVertices, unsigned long long numAristas,
                               double pesoMin, double pesoMax, string nombreArchivo,
                               mt19937_64& gen) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo." << endl;
        return;
    }
    
    archivo.sync_with_stdio(false);
    archivo << numVertices << " " << numAristas << "\n";
    
    // Para grafo dirigido denso, generar todas las posibles y mezclar
    vector<pair<int, int>> todasAristas;
    unsigned long long maxPosibles = (unsigned long long)numVertices * (numVertices - 1);
    todasAristas.reserve(maxPosibles);
    
    cout << "Preparando " << maxPosibles << " aristas posibles..." << endl;
    
    // Generar todas las aristas dirigidas posibles (sin bucles)
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            if (i != j) {  // No incluir bucles (i,i)
                todasAristas.emplace_back(i, j);
            }
        }
        if (i % 100 == 0) {
            cout << "Procesando vertice " << i << "/" << numVertices << "\r" << flush;
        }
    }
    
    cout << "\nMezclando " << todasAristas.size() << " aristas..." << endl;
    
    // Mezclar aleatoriamente usando Fisher-Yates
    for (unsigned long long i = todasAristas.size() - 1; i > 0; --i) {
        uniform_int_distribution<unsigned long long> dist(0, i);
        unsigned long long j = dist(gen);
        swap(todasAristas[i], todasAristas[j]);
        
        if (i % 10000000 == 0) {
            cout << "Mezclando... " << (todasAristas.size() - i) * 100 / todasAristas.size() << "%\r" << flush;
        }
    }
    
    cout << "\nEscribiendo " << numAristas << " aristas al archivo..." << endl;
    
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
    cout << "Tamaño estimado: " << (numAristas * 25 / 1000000) << " MB" << endl;
}

void generarGrafoDirigidoDisperso(int numVertices, unsigned long long numAristas,
                                  double pesoMin, double pesoMax, string nombreArchivo,
                                  mt19937_64& gen) {
    ofstream archivo(nombreArchivo);
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
            
            // Solo evitar bucles (origen != destino)
            // En grafo dirigido, (i,j) y (j,i) son DIFERENTES
            
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

void generarGrafoDirigido(int numVertices, int densidad, double pesoMin, double pesoMax, string nombreArchivo) {
    // Para grafo DIRIGIDO (sin bucles):
    unsigned long long maxAristas = (unsigned long long)numVertices * (numVertices - 1); // n × (n-1)
    unsigned long long numAristas = (maxAristas * densidad) / 100;
    
    if (numAristas > maxAristas) {
        numAristas = maxAristas;
    }
    
    cout << "Generando grafo DIRIGIDO con " << numVertices << " vertices y " 
         << numAristas << " aristas (" << densidad << "% de " << maxAristas 
         << " posibles)..." << endl;
    
    // Crear generador aleatorio
    random_device rd;
    mt19937_64 gen(rd());
    
    // Usar el método apropiado según la densidad
    if (densidad > 70) {
        generarGrafoDirigidoDenso(numVertices, numAristas, pesoMin, pesoMax, nombreArchivo, gen);
    } else {
        generarGrafoDirigidoDisperso(numVertices, numAristas, pesoMin, pesoMax, nombreArchivo, gen);
    }
}




int main(void){
    /*
    generarGrafoDirigido(2048,100,1,1000,"2048_100_1.txt");
    generarGrafoDirigido(2048,100,1,1000,"2048_100_2.txt");
    generarGrafoDirigido(2048,100,1,1000,"2048_100_3.txt");
    generarGrafoDirigido(2048,50,1,1000,"2048_50_2.txt");
    generarGrafoDirigido(2048,50,1,1000,"2048_50_3.txt");
    generarGrafoDirigido(2048,25,1,1000,"2048_25_1.txt");
    generarGrafoDirigido(2048,25,1,1000,"2048_25_2.txt");
    generarGrafoDirigido(2048,25,1,1000,"2048_25_3.txt");
    */
   generarGrafoDirigido(512,50,1,1000,"512_50_1.txt");
}