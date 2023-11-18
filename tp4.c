#include <stdio.h>
#include <stdlib.h>

#define MAX_COLS 2
#define MAX_ROWS 2

//A)
int leerArchivo(char *nombreArchivo, float fuente[MAX_COLS], float matrizCanal[MAX_ROWS][MAX_COLS]) {

    FILE *archivo = fopen(nombreArchivo,"rt");

    if (archivo == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo.\n");
        exit(-1); //error
    }

    //probabilidades de la fuente binaria
    for ( int i = 0 ; i < MAX_COLS ; i++ ) {
        if (fscanf(archivo, "%f", &fuente[i]) != 1) {
            fprintf(stderr, "Error al leer las probabilidades de la fuente.\n");
            fclose(archivo);
            exit(-1); //error
        }
    }

    //matriz del matrizCanal binario
    for ( int i = 0 ; i < MAX_ROWS ; i++ ) {
        for ( int j = 0 ; j < MAX_COLS ; j++) {
            if (fscanf(archivo, "%f", &matrizCanal[i][j]) != 1) {
                fprintf(stderr, "Error al leer la matriz del matrizCanal binario.\n");
                fclose(archivo);
                exit(-1); //error
            }
        }
    }



    fclose(archivo);
    return 0;
}


//B)
//Primero calculo las probabilidades a posteriori

void calculaProbB(float probSalida[MAX_COLS]) { //calcula la probabilidad de que salga 0 y 1 sin saber que entro
   
   float suma = 0;

   for ( int i = 0 ; i < 2 ; i++ ) {
      for ( int j = 0 ; j < 2 ; j++ ) {
        // suma += (fuente[j]*matrizCanal[i][j]);
      }
   }
}

void incisoB(float probPosteriori) {
   for ( int i = 0 ; i < 4 ; i++ ) {
      //post -> P(0\0)
      //post -> P(0\1)
      //post -> P(1\0)
      //post -> P(1\1)


   }
}


int main() {

    float fuente[MAX_COLS]; //prob de que entre un 0 y de que entre un 1
    float matrizCanal [MAX_ROWS][MAX_COLS];
    float probPosteriori[MAX_ROWS][MAX_COLS];
    float probSalida[MAX_COLS]; //probabilidad de que salga 0 y 1 sin saber que entro

    leerArchivo("tp4_sample4.txt", fuente, matrizCanal); //A)
       

    return 0;
}
