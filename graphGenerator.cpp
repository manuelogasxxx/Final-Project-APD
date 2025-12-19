#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;

class GrafoGenerator {
private:
    int numVertices;
    int densidad; // porcentaje de densidad (0-100)
    int pesoMaximo;
    bool permitirNegativos;
    int numGrafos;
    
public:
    // Constructor
    GrafoGenerator(int vertices, int densidad, int pesoMax, bool negativos, int num) {
        numVertices = vertices;
        this->densidad = densidad;
        pesoMaximo = pesoMax;
        permitirNegativos = negativos;
        numGrafos = num;
        
        // Inicializar semilla aleatoria
        srand(time(0));
    }
    
    // Generar un grafo aleatorio
    vector<vector<int>> generarGrafo() {
        vector<vector<int>> grafo(numVertices, vector<int>(numVertices, 0));
        
        // Inicializar con infinitos (representados como un valor muy grande)
        int INF = 1000000;
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (i == j) {
                    grafo[i][j] = 0; // Diagonal principal en 0
                } else {
                    grafo[i][j] = INF;
                }
            }
        }
        
        // Generar aristas según la densidad
        int maxAristas = numVertices * (numVertices - 1);
        int numAristas = (maxAristas * densidad) / 100;
        
        for (int a = 0; a < numAristas; a++) {
            int i = rand() % numVertices;
            int j = rand() % numVertices;
            
            // Evitar bucles y aristas repetidas
            if (i == j || grafo[i][j] != INF) {
                a--;
                continue;
            }
            
            // Generar peso aleatorio
            int peso;
            if (permitirNegativos) {
                // Peso entre -pesoMaximo y pesoMaximo
                peso = (rand() % (2 * pesoMaximo + 1)) - pesoMaximo;
            } else {
                // Peso entre 1 y pesoMaximo
                peso = 1 + (rand() % pesoMaximo);
            }
            
            grafo[i][j] = peso;
        }
        
        return grafo;
    }
    
    // Escribir un grafo en un archivo
    void escribirGrafo(ofstream &archivo, const vector<vector<int>> &grafo, int idGrafo) {
        archivo << "Grafo " << idGrafo << endl;
        archivo << numVertices << endl;
        
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (i != j && grafo[i][j] != 1000000) {
                    archivo << i << " " << j << " " << grafo[i][j] << endl;
                }
            }
        }
        
        archivo << "-1 -1 -1" << endl; // Marcador de fin de grafo
        archivo << endl; // Línea en blanco para separar grafos
    }
    
    // Generar múltiples grafos y guardarlos en un archivo
    void generarArchivoGrafos(const string &nombreArchivo) {
        ofstream archivo(nombreArchivo);
        
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
            return;
        }
        
        archivo << numGrafos << endl << endl; // Número total de grafos
        
        for (int i = 0; i < numGrafos; i++) {
            cout << "Generando grafo " << i+1 << " de " << numGrafos << "..." << endl;
            vector<vector<int>> grafo = generarGrafo();
            escribirGrafo(archivo, grafo, i+1);
        }
        
        archivo.close();
        cout << "Archivo '" << nombreArchivo << "' generado con éxito!" << endl;
    }
    
    // Función para mostrar un grafo en consola (opcional, para depuración)
    void mostrarGrafo(const vector<vector<int>> &grafo) {
        int INF = 1000000;
        
        cout << "Matriz de adyacencia (" << numVertices << "x" << numVertices << "):" << endl;
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (grafo[i][j] == INF) {
                    cout << "INF\t";
                } else {
                    cout << grafo[i][j] << "\t";
                }
            }
            cout << endl;
        }
    }
};

// Función para leer grafos desde un archivo
vector<vector<vector<int>>> leerGrafosDesdeArchivo(const string &nombreArchivo) {
    ifstream archivo(nombreArchivo);
    vector<vector<vector<int>>> todosLosGrafos;
    
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
        return todosLosGrafos;
    }
    
    int numGrafos;
    archivo >> numGrafos;
    
    for (int g = 0; g < numGrafos; g++) {
        // Saltar líneas en blanco y encabezados
        string linea;
        getline(archivo, linea); // Salto de línea después del número
        
        // Leer encabezado "Grafo X"
        getline(archivo, linea);
        
        int numVertices;
        archivo >> numVertices;
        
        vector<vector<int>> grafo(numVertices, vector<int>(numVertices, 1000000));
        
        // Inicializar diagonal principal
        for (int i = 0; i < numVertices; i++) {
            grafo[i][i] = 0;
        }
        
        // Leer aristas
        while (true) {
            int i, j, peso;
            archivo >> i >> j >> peso;
            
            if (i == -1 && j == -1 && peso == -1) {
                break; // Fin del grafo
            }
            
            grafo[i][j] = peso;
        }
        
        todosLosGrafos.push_back(grafo);
        
        // Saltar línea en blanco
        getline(archivo, linea);
    }
    
    archivo.close();
    return todosLosGrafos;
}

// Algoritmo Floyd-Warshall para verificación
void floydWarshall(const vector<vector<int>> &grafo) {
    int n = grafo.size();
    vector<vector<int>> dist = grafo;
    
    // Algoritmo Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
    
    // Mostrar resultado
    cout << "\nMatriz de distancias más cortas (Floyd-Warshall):" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] == 1000000) {
                cout << "INF\t";
            } else {
                cout << dist[i][j] << "\t";
            }
        }
        cout << endl;
    }
}

// Menú interactivo
void mostrarMenu() {
    cout << "\n======================================" << endl;
    cout << "    GENERADOR DE GRAFOS PARA FLOYD-WARSHALL" << endl;
    cout << "======================================" << endl;
    cout << "1. Generar grafos aleatorios" << endl;
    cout << "2. Leer grafos desde archivo y aplicar Floyd-Warshall" << endl;
    cout << "3. Generar y procesar inmediatamente" << endl;
    cout << "4. Salir" << endl;
    cout << "======================================" << endl;
    cout << "Seleccione una opcion: ";
}

int main() {
    int opcion;
    string nombreArchivo = "grafos.txt";
    
    do {
        mostrarMenu();
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                // Configuración del generador
                int vertices, densidad, pesoMax, numGrafos;
                char permitirNeg;
                
                cout << "\n--- Configuracion del generador de grafos ---" << endl;
                cout << "Numero de vertices por grafo: ";
                cin >> vertices;
                cout << "Densidad (0-100): ";
                cin >> densidad;
                cout << "Peso maximo: ";
                cin >> pesoMax;
                cout << "Permitir pesos negativos? (s/n): ";
                cin >> permitirNeg;
                cout << "Cuantos grafos desea generar? ";
                cin >> numGrafos;
                
                bool negativos = (permitirNeg == 's' || permitirNeg == 'S');
                
                // Generar grafos
                GrafoGenerator generador(vertices, densidad, pesoMax, negativos, numGrafos);
                generador.generarArchivoGrafos(nombreArchivo);
                break;
            }
                
            case 2: {
                // Leer grafos desde archivo
                cout << "\nLeyendo grafos desde '" << nombreArchivo << "'..." << endl;
                vector<vector<vector<int>>> grafos = leerGrafosDesdeArchivo(nombreArchivo);
                
                if (grafos.empty()) {
                    cout << "No se pudieron leer grafos. Asegurese de generar primero los grafos." << endl;
                } else {
                    cout << "Se leyeron " << grafos.size() << " grafos." << endl;
                    
                    // Aplicar Floyd-Warshall a cada grafo
                    for (size_t i = 0; i < grafos.size(); i++) {
                        cout << "\n=== Aplicando Floyd-Warshall al Grafo " << i+1 << " ===" << endl;
                        floydWarshall(grafos[i]);
                    }
                }
                break;
            }
                
            case 3: {
                // Generar un grafo y aplicar Floyd-Warshall inmediatamente
                int vertices, densidad, pesoMax;
                char permitirNeg;
                
                cout << "\n--- Generar un solo grafo ---" << endl;
                cout << "Numero de vertices: ";
                cin >> vertices;
                cout << "Densidad (0-100): ";
                cin >> densidad;
                cout << "Peso maximo: ";
                cin >> pesoMax;
                cout << "Permitir pesos negativos? (s/n): ";
                cin >> permitirNeg;
                
                bool negativos = (permitirNeg == 's' || permitirNeg == 'S');
                
                // Generar un solo grafo
                GrafoGenerator generador(vertices, densidad, pesoMax, negativos, 1);
                vector<vector<int>> grafo = generador.generarGrafo();
                
                cout << "\n=== Grafo generado ===" << endl;
                generador.mostrarGrafo(grafo);
                
                // Aplicar Floyd-Warshall
                floydWarshall(grafo);
                break;
            }
                
            case 4:
                cout << "\nSaliendo del programa..." << endl;
                break;
                
            default:
                cout << "\nOpcion no valida. Intente de nuevo." << endl;
        }
        
    } while (opcion != 4);
    
    return 0;
}