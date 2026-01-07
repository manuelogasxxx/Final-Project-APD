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

using namespace std;
const double INF = numeric_limits<double>::infinity();
void generarGrafo(int numVertices, int densidad, double pesoMin, double pesoMax, string nombreArchivo) {
    ofstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo." << endl;
        return;
    }
    if(densidad<1){
        cerr<<"Número de aristas no válido"<<endl;
        return;
    }
    unsigned long long int numAristas = ((numVertices*numVertices)*densidad)/100;
    archivo.sync_with_stdio(false); 

    cout << "Generando grafo con " << numVertices << " vertices y " << numAristas << " aristas..." << endl;

    archivo << numVertices <<" "<< numAristas <<"\n";

    random_device rd;
    mt19937 gen(rd());
    
    uniform_int_distribution<int> distVertices(0, numVertices - 1);
    uniform_real_distribution<double> distPesos(pesoMin, pesoMax);

    long long porcentaje = numAristas / 10; // Para mostrar progreso cada 10%
    if (porcentaje == 0) porcentaje = 1;

    for (long long i = 0; i < numAristas; ++i) {
        int origen = distVertices(gen);
        int destino = distVertices(gen);
        
        // Evitar bucles propios (origen == destino)
        while (origen == destino) {
            destino = distVertices(gen);
        }

        double peso = distPesos(gen);

        archivo << origen << " " << destino << " " << fixed << setprecision(4) << peso << "\n";

        if (i % porcentaje == 0) {
            cout << "Progreso: " << (i * 100) / numAristas << "%\r" << flush;
        }
    }

    archivo.close();
    cout << "\nGeneracion completada. Archivo guardado como: " << nombreArchivo << endl;
}

vector<vector<double>> leerGrafo(string nombreArchivo){
    
    ifstream entrada(nombreArchivo);
    if(!entrada){
        cerr<<"Error al abrir el archivo\n";
        vector<vector<double>> r;
        return r;
    }
    int vertices,aristas;
    double u,v,peso;
    entrada>>vertices>>aristas;
    vector<vector<double>> respuesta(vertices,vector<double>(vertices,0));
    cout<<"vertices:"<<vertices<<" aristas:"<<aristas<<"\n";
    for(int i = 0;i<aristas;i++){
        entrada>>u>>v>>peso;
        respuesta[u][v] = peso;
    }

    entrada.close();
    return respuesta;
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






int main() {
    generarGrafo(1024,25,1,1000,"1024_25_1.txt");
    generarGrafo(1024,25,1,1000,"1024_25_2.txt");
    generarGrafo(1024,25,1,1000,"1024_25_3.txt");


    //leerGrafo("prueba.txt");
}