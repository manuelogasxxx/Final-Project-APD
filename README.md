# Proyecto final APD

Desarrollado por: Manuel Fernandez Mercado

Materia: Algoritmos paralelos y distribuidos


## Deployment

Copia el repositorio con el siguiente comando

```bash
  https://github.com/manuelogasxxx/Final-Project-APD.git
```

Puedes descargar mis casos de prueba desde el siguiente enlace de mediafire:

https://www.mediafire.com/folder/uacok23c6nb2r/Matrices

Para generar tus propios datos de prueba usa el archivo graphGeneratorV2.cpp, se muestra un ejemplo de uso acontinuacion:
```bash
  generarGrafoDirigido(512,50,1,1000,"512_50_1.txt");
  //generará un grafo de 512 vértices 
  //con 50% de densidad
  //con valor minimo de 1
  //máximo de 1000
  //será guardado en "512_50_1.txt" 
```
Asegúrate que estos archivos .txt se encuentren en el mismo directorio en donde ejecutarás el programa.

**Ejecución:**

Ingresa al archivo FloydWarshall.cpp, y genera un vector de strings con los nombres de los archivos a ejecutar  
```bash
  vector<string> a = {"512_100_1.txt","512_100_2.txt","512_100_3.txt","512_50_1.txt","512_50_2.txt"}; 
```

Posteriomente ejecuta en el main la función "ejecutar" de la siguiente forma:
```bash
    int main(void){
        ejecutar(a,"tiempos_512_OMP1.txt");
        vector<string> a = {"512_100_1.txt","512_100_2.txt","512_100_3.txt","512_50_1.txt","512_50_2.txt"};  
    }
   
```
Si deseas cambiar el algoritmo a ejecutar ve a la función y comenta/descomenta el algoritmo que quieras.

Usa una compilación clásica con el compilador de tu preferencia para los casos secuenciales y agrega la bandera -fopenmp para los casos paralelos.

**CUDA**
Para estos experimentos se usó un cuaderno de google colab con una Nvidia T4, puedes consultarlo en el siguiente enlace:

https://colab.research.google.com/drive/1lGT3UFpYJZasxhuAoP2qCwIqo6Ln3r6M?usp=sharing

Ejecuta las celdas de arriba a abajo, conecta tu cuenta de Drive y guarda en tu unidad los archivos .txt que requieras probar.
## Pruebas
A continuación se presenta una tabla
