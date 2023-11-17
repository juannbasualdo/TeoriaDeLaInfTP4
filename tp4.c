#include <stdio.h>
#include <stdlib.h>

#define MAX_COLS 2
#define MAX_ROWS 2

//A)
int leerArchivo(char *nombreArchivo, float fuente[MAX_COLS], float canal[MAX_ROWS][MAX_COLS]) {

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

    //matriz del canal binario
    for ( int i = 0 ; i < MAX_ROWS ; i++ ) {
        for ( int j = 0 ; j < MAX_COLS ; j++) {
            if (fscanf(archivo, "%f", &canal[i][j]) != 1) {
                fprintf(stderr, "Error al leer la matriz del canal binario.\n");
                fclose(archivo);
                exit(-1); //error
            }
        }
    }

    fclose(archivo);
    return 0;
}



int main() {

    float fuente[MAX_COLS];
    float canal [MAX_ROWS][MAX_COLS];

    (leerArchivo("tp4_sample4.txt", fuente, canal)); //A)
       

    return 0;
}
