#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_COLS 2
#define MAX_ROWS 2
#define MAX_LONG 20
#define MAX_MENS 40




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

void calculaProbB(float probB[MAX_COLS], float fuente[], float matrizCanal[][MAX_COLS]) { //calcula la probabilidad de que salga 0 y 1 sin saber que entro
   
   int i, j;

   for ( i = 0 ; i < 2 ; i++ ) {
      for ( j = 0 ; j < 2 ; j++ ) {
         probB[i] += (fuente[j]*matrizCanal[j][i]);
      }
   }

   /*
   for ( i = 0 ; i < 2 ; i++ )
      printf("P(B = %d) = %f\n",i,probB[i]);
   */
}

void calculoMatrizPosteriori(float matrizPosteriori[MAX_ROWS][MAX_COLS], float fuente[], float matrizCanal[MAX_ROWS][MAX_COLS], float probB[]) {

   int i, j;

   for ( i = 0 ; i < 2 ; i++ )
      for ( j = 0 ; j < 2 ; j++ )
         matrizPosteriori[i][j] =  ((fuente[i]*matrizCanal[i][j])/probB[j]);

   /*
   for ( i = 0; i < 2 ; i++ ) {
      for ( j  = 0 ; j < 2 ; j++ ) {
         printf("\n");
         printf("P( a = %d / b = % d ) = %f",i,j,matrizPosteriori[i][j]);
      }
   }
   */
}

void calculaSucSimul(float probSucSimul[], float fuente[], float matrizCanal[MAX_ROWS][MAX_COLS]) {
    int i, j, k = 0;

    for ( i = 0 ; i < 2 ; i++ ) {
       for ( j = 0 ; j < 2 ; j++ ) {
          probSucSimul[k++] = fuente[i]*matrizCanal[i][j];
       }
    }

   /*
   k = 0;
   for ( i = 0 ; i < 2 ; i++ ) {
      for ( j = 0 ; j < 2 ; j++ ) {
         printf("a = %d , b = %d\n",i,j);
         printf("%f",probSucSimul[k++]);
         printf("\n");
      }
    }
   */ 
}

void equivocacionCanal(float probSucSimul[], float matrizPosteriori[MAX_ROWS][MAX_COLS], float *equivoc_AB) {
   int k = 0;

   for (int i = 0 ; i < 2 ; i++) 
      for ( int j = 0 ; j < 2 ; j++)
         *equivoc_AB += probSucSimul[k++]*log2(1/matrizPosteriori[i][j]);

   printf("La equivocacion del canal es H(A/B) = %f bits\n",*equivoc_AB);      
}


void calculoDeEntropiaDeX(float vector[], float *entropiaDeX) {
   int i;

   for ( i = 0 ; i < 2 ; i++ )
      *entropiaDeX += vector[i]*log2(1/vector[i]);

}

void entropiaPosteriori_A(float matrizPosteriori[MAX_ROWS][MAX_COLS], int num) {
   float suma = 0; 
   for ( int i = 0 ; i < 2 ; i++ ) {
      suma += matrizPosteriori[i][num]*log2(1/matrizPosteriori[i][num]);   
   }
   printf("H(A/b = %d) = %f\n",num,suma);
}

void generaMensajes(float matMensajes[MAX_MENS][MAX_MENS], int N, int M, float fuente[]) {

   int    i, j;
   double random_value;
   int    random_number;

   // Establecer la semilla para la generación de números aleatorios
   srand(time(NULL));

   for ( i = 0 ; i < N ; i++ )
      for ( j = 0 ; j < M ; j++ ) {

         
         // Generar un número aleatorio entre 0 y RAND_MAX
         random_number = rand();
         // Convertir el número a un valor entre 0 y 1
          random_value = (double)random_number / RAND_MAX;
         // Imprimir el resultado
         printf("Número aleatorio entre 0 y 1: %lf\n", random_value);

         if (random_value >= 0 && random_value <= fuente[0])
            matMensajes[i][j] = 0;
         else
            matMensajes[i][j] = 1;
      }      
}


int main() {

    //int argc, char argv[]
    //char nombre_archivo = argv[1];
    //char nombre_archivo; 
    //strcpy(nombre_archivo,);
    int  N, M;
    /*
    int estaP = 0;
    N = atoi(&argv[2]);
    M = atoi(&argv[3]);
    if (argc == 5)
       if (!strcmp(&argv[4],"-p"))
          estaP = 1;
    */

    float fuente[MAX_COLS]; //prob de que entre un 0 y de que entre un 1
    float matrizCanal[MAX_ROWS][MAX_COLS];
    float probPosteriori[MAX_ROWS][MAX_COLS];
    float probB[MAX_COLS]; //probabilidad de que salga 0 y 1 sin saber que entro
    float probSucSimul[4]; //probabilidad sucesos simultaneos
    float matrizPosteriori[MAX_ROWS][MAX_COLS];
    float equivoc_AB, entropiaDeA, entropiaDeB;
    float matMensajes[N][M];

    leerArchivo("tp4_sample0.txt", fuente, matrizCanal); //A)

    calculaSucSimul(probSucSimul,fuente,matrizCanal);    //B)

    calculaProbB(probB,fuente,matrizCanal);

    calculoMatrizPosteriori(matrizPosteriori,fuente,matrizCanal,probB);

    equivocacionCanal(probSucSimul,matrizPosteriori,&equivoc_AB);
    calculoDeEntropiaDeX(fuente,&entropiaDeA);
    
    calculoDeEntropiaDeX(probB,&entropiaDeB);
    
    printf("La informacion mutua del canal es I(A,B) = %f bits\n",entropiaDeA - equivoc_AB);
    printf("\n---Entropias del canal:---\n");
    printf("H(A)   = %f\n",entropiaDeA);
    printf("H(B)   = %f\n",entropiaDeB);
    printf("H(A,B) = %f\n",entropiaDeB + equivoc_AB);
    entropiaPosteriori_A(matrizPosteriori,0);
    entropiaPosteriori_A(matrizPosteriori,1);
   
    generaMensajes(matMensajes,N,M,fuente);
    return 0;
}

