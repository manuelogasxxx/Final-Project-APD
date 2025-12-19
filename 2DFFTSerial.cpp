/*
Serial Algorithm to 2DFFT transformation

->Se necesita una imagen cuadrada

*/
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <complex> // Usaremos la librería estándar C++ para números complejos
using namespace std;

const double PI = 3.14159265358979323846;

//Reordenamiento bit-reverse (FTT iteratica)
void bitReverseCopy(vector<complex<double>>& data, int N) {
    int j = 0;
    for (int i = 0; i < N; ++i) {
        if (i < j) {
            swap(data[i], data[j]);
        }
        int mask = N >> 1;
        while (j & mask) {
            j ^= mask;
            mask >>= 1;
        }
        j ^= mask;
    }
}

/**
 * Transformada rápida de fourier 1D iterativa
 * Implementa el algoritmo FFT de Cooley-Tukey (iterativo).
 * @param data Vector de números complejos de tamaño N (debe ser potencia de 2).
 * @param N Tamaño del vector.
 * @param inverse True si se calcula la IFFT (Transformada Inversa).
 */
void FFT_1D(vector<complex<double>>& data, int N, bool inverse = false) {
    if (N <= 1) return;

    // 1. Reordenamiento Bit-Reverse
    bitReverseCopy(data, N);

    // 2. Etapas de Mariposa (Butterfly Stages)
    for (int len = 2; len <= N; len <<= 1) { // len: tamaño del sub-transformada
        double angle = 2.0 * PI / len;
        if (inverse) angle = -angle; // Cambio de signo para IFFT
        
        // Factor giratorio W_len = exp(-i * angle)
        complex<double> wlen(cos(angle), sin(angle)); 

        for (int i = 0; i < N; i += len) {
            complex<double> w(1.0); // Factor giratorio inicial (w^0)
            
            for (int j = 0; j < len / 2; ++j) {
                // Operación Mariposa (Butterfly)
                // U = data[i + j]; T = data[i + j + len/2] * w;
                complex<double> u = data[i + j];
                complex<double> t = data[i + j + len / 2] * w;

                data[i + j] = u + t;
                data[i + j + len / 2] = u - t;
                
                // Actualizar factor giratorio: w = w * wlen
                w *= wlen;
            }
        }
    }
    
    // Normalización para IFFT
    if (inverse) {
        for (int i = 0; i < N; ++i) {
            data[i] /= (double)N;
        }
    }
}


/**
 * Transformada rápida de fourier 2D secuencial
 * Aplica la 2D-FFT secuencial usando el método Fila-Columna.
 * La matriz se pasa como un vector 1D (fila-mayor) para facilitar la implementación.
 * @param matrix Matriz de datos complejos (N*N).
 * @param N Dimensión de la matriz (N x N).
 */
void FFT_2D_Secuencial(vector<complex<double>>& matrix, int N) {
    
    // --- FASE 1: Transformación por Filas ---
    for (int i = 0; i < N; ++i) {
        // La fila i comienza en el índice i * N
        // No necesitamos copiar la fila, solo pasar la dirección de inicio
        vector<complex<double>> row(N);
        copy(matrix.begin() + i * N, matrix.begin() + (i + 1) * N, row.begin());
        
        FFT_1D(row, N);
        
        // Copiar el resultado de vuelta a la matriz
        copy(row.begin(), row.end(), matrix.begin() + i * N);
    }
    
    // --- FASE 2: Transformación por Columnas ---
    vector<complex<double>> col(N); // Buffer temporal para la columna
    
    for (int j = 0; j < N; ++j) { // Iterar sobre cada columna j
        
        // Cargar la columna j en el buffer temporal 'col'
        for (int i = 0; i < N; ++i) { // Iterar sobre las filas
            col[i] = matrix[i * N + j]; // Acceso a memoria no consecutivo (¡lento!)
        }
        
        FFT_1D(col, N);
        
        // Escribir los resultados transformados de vuelta en la matriz
        for (int i = 0; i < N; ++i) {
            matrix[i * N + j] = col[i];
        }
    }
}

// =========================================================
// 5. FUNCIÓN DE PRUEBA (MAIN)
// =========================================================
int main() {
    // N debe ser potencia de 2 para la FFT de Cooley-Tukey
    const int N = 4096; // Mínimo requerido por el proyecto
    const int N_total = N * N;
    
    vector<complex<double>> matrix(N_total);
    
    // Inicializar la matriz con datos de ejemplo (por ejemplo, números reales simples)
    for (int i = 0; i < N_total; ++i) {
        // Inicialización simple: valor real igual al índice, parte imaginaria 0
        matrix[i] = complex<double>(i % 10, 0.0); 
    }
    
    // Hacemos una copia para verificar la IFFT posteriormente si se desea
    // vector<complex<double>> original_matrix = matrix; 

    cout << "Iniciando 2D-FFT secuencial para matriz de " << N << "x" << N << "..." << endl;

    // --- Medición de tiempo de ejecución (Usar una librería más precisa como <chrono> en el proyecto real) ---
    // Nota: Para una medición precisa de nanosegundos en un proyecto, usa <chrono> de C++.
    
    // Ejemplo de cómo llamar a la función (medición de tiempo omitida aquí por simplicidad)
    FFT_2D_Secuencial(matrix, N);

    cout << "2D-FFT secuencial finalizada." << endl;
    
    // Opcional: Imprimir un subconjunto de resultados para verificación
    cout << "\nPrimeros 5 elementos de la matriz transformada:" << endl;
    for (int i = 0; i < 5; ++i) {
        cout << matrix[i] << " ";
    }
    cout << endl;

    // Si quieres verificar la IFFT, puedes llamar a una función similar a FFT_2D pero con 'inverse=true'
    // IFFT_2D_Secuencial(matrix, N); 
    
    return 0;
}