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
         if (matrizPosteriori[i][j] != 0)
            *equivoc_AB += probSucSimul[k++]*log2(1/matrizPosteriori[i][j]);

         
}


void calculoDeEntropiaDeX(float vector[], float *entropiaDeX) {
   int i;

   for ( i = 0 ; i < 2 ; i++ )
      if (vector[i] != 0)  
         *entropiaDeX += vector[i]*log2(1/vector[i]);

}

void entropiaPosteriori_A(float matrizPosteriori[MAX_ROWS][MAX_COLS], int num) {
   float suma = 0; 
   for ( int i = 0 ; i < 2 ; i++ ) {
      if (matrizPosteriori[i][num] != 0)
         suma += matrizPosteriori[i][num]*log2(1/matrizPosteriori[i][num]);   
   }
   printf("H(A/b = %d) = %f\n",num,suma);
}


void generaMensajes(int matMensajes[MAX_MENS][MAX_MENS], int N, int M, float fuente[]) {

   int    i, j;
   double random_value;
   int    random_number;

   // Establecer la semilla para la generación de números aleatorios
   srand(time(NULL));

   //dejo la primer fila libre para bits de LRC
   j = 0;
   while ( j <= M) {
      matMensajes[0][j] = -1;
      j++;
   }

   //dejo la primer fila libre para bits de VRC 
   i = 0;
   while (i <= N) {
      matMensajes[i][M] = -1;
      i++;
   }

   printf("\n");
   for ( i = 1 ; i <= N ; i++ ) {
      for ( j = 0 ; j < M ; j++ ) {
         // Generar un número aleatorio entre 0 y RAND_MAX
         random_number = rand();
         // Convertir el número a un valor entre 0 y 1
          random_value = (double)random_number / RAND_MAX;
          printf("Valor aleatorio [%d][%d]: %f      ",i,j,random_value);

         if (random_value >= 0 && random_value <= fuente[0])
            matMensajes[i][j] = 0;
         else
            matMensajes[i][j] = 1;
            
      }  
      printf("\n");
   }    
   
   printf("\n");
   printf("Simulacion del envio de %d mensajes aleatorios de longitud %d:\n",N,M);

   for (i = 1 ; i <= N ; i ++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matMensajes[i][j]);
      printf("\n");
   }   

}

void paridadCruzada(int matMensajes[MAX_MENS][MAX_MENS], int N, int M) {
   int resultadoAnterior;
   int i, j, aux;
   int bitVRC, bitLRC;
   
   //Paridad vertical(VRC):
   for ( j = 0 ; j < M ; j ++) {
      resultadoAnterior = matMensajes[N][j] ^ matMensajes[N - 1][j];
      for ( i = N - 2 ; i > 0 ; i-- ) { //Hace el XOR entre cada uno de los elementos de cada columna
         aux = resultadoAnterior;
         resultadoAnterior = matMensajes[i][j] ^ aux;
      }
      if (resultadoAnterior == 1)
         matMensajes[0][j] = 1;
      else
         matMensajes[0][j] = 0;        
   }

   //Paridad longitudinal(LRC):
   for ( i = 1 ; i <= N ; i ++) {
      resultadoAnterior = matMensajes[i][0] ^ matMensajes[i][1];
      for ( j =  2 ; j < M ; j++ ) { //Hace el XOR entre cada uno de los elementos de cada fila
         aux = resultadoAnterior;
         resultadoAnterior = matMensajes[i][j] ^ aux;
      }
      if (resultadoAnterior == 1)
         matMensajes[i][M] = 1;
      else
         matMensajes[i][M] = 0;        
   }

   //XOR entre los bits de la primer fila (resultantes de VRC):
   j = 0;
   resultadoAnterior = matMensajes[0][0] ^ matMensajes[0][1];
   while( j < M ) {
      aux = resultadoAnterior;
      resultadoAnterior = matMensajes[i][j] ^ aux;
      j++;
   }
   bitVRC = resultadoAnterior;

   //XOR entre los bits de la ultima columna (resultantes de LRC):
   i = N - 2;
   resultadoAnterior = matMensajes[N][M] ^ matMensajes[N - 1][M];
   while( i > 0 ) {
      aux = resultadoAnterior;
      resultadoAnterior = matMensajes[i][M] ^ aux;
      i--;
   }
   bitLRC = resultadoAnterior;

   if (bitVRC == bitLRC) {
      matMensajes[0][M] = bitVRC;
      printf("Los bits coinciden (VRC y LRC)\n");
   }
   else
      printf("Los bits no coinciden\n");

   printf("\n");
   printf("Matriz resultante de aplicar el metodo de paridad cruzada:\n");
   for (i = 0 ; i <= N ; i ++ ) {
      for ( j = 0 ; j <= M ; j++ )
         printf("%d  ",matMensajes[i][j]);
      printf("\n");
   }   
   
       

}


void generaMat2(float matrizCanal[MAX_ROWS][MAX_COLS], int matMensajes[MAX_LONG][MAX_MENS], int matMensajes2[MAX_LONG][MAX_MENS], int N, int M) {

   int i, j;
   double random_value;
   int    random_number;

   printf("\nMatriz del canal:\n");
   for ( i = 0 ; i < 2 ; i++ ) {
      for ( j = 0 ; j < 2 ; j++ )
         printf("%f ",matrizCanal[i][j]);
      printf("\n");
   }      
   printf("\n");

   for ( i = 0 ; i <= N ; i++ ) {
      for ( j = 0 ; j <= M ; j++ ) {
         // Generar un número aleatorio entre 0 y RAND_MAX
         random_number = rand();
         // Convertir el número a un valor entre 0 y 1
          random_value = (double)random_number / RAND_MAX;
          printf("Valor aleatorio [%d][%d]: %f     ",i,j,random_value);

         if (matMensajes[i][j] == 0)
            if (random_value <= matrizCanal[0][0])
               matMensajes2[i][j] = 0;
            else
               matMensajes2[i][j] = 1;   
         else
            if (random_value <= matrizCanal[1][0])
               matMensajes2[i][j] = 0;
            else
               matMensajes2[i][j] = 1;      
      }
      printf("\n");
   }

   printf("\nMatriz 2:\n");
   for (i = 0 ; i <= N ; i ++ ) {
      for ( j = 0 ; j <= M ; j++ )
         printf("%d  ",matMensajes2[i][j]);
      printf("\n");     
   }
   printf("\n");
}

void analiza(int matMensajes2[MAX_LONG][MAX_MENS], int N, int M) {
   int i, j, resultadoAnterior, aux;
   int matErrores[MAX_LONG][MAX_MENS];
   //int vecFila[N] = {0}, vecCol[N]={0};
   
   //primero hago el XOR entre los bits de cada fila
   //si no hay errores (al menos no pares) deberia darme 0 el XOR de cada una de las filas
   for ( i = 0 ; i <= N ; i++ ) {
      resultadoAnterior = matMensajes2[i][0] ^ matMensajes2[i][1];
      for ( j = 2 ; j <= M ; j++ ) {
         aux = resultadoAnterior;
         resultadoAnterior = aux ^ matMensajes2[i][j];   
      }
      if (resultadoAnterior == 1) {
         printf("Error detectado en la fila: %d\n",i);
         for ( j = 0 ; j <= M ; j++ )
            matErrores[i][j] = 1;   
      }
      else 
         for ( j = 0 ; j <= M ; j++ )
            matErrores[i][j] = 0;   
   }   

   //hago el XOR entre los bits de cada columna
   //si no hay errores (al menos no pares) deberia darme 0 el XOR de cada una de las columnas
   for ( j = 0 ; j <= M ; j++ ) {
      resultadoAnterior = matMensajes2[0][j] ^ matMensajes2[1][j];
      for ( i = 2 ; i <= N ; i++ ) {
         aux = resultadoAnterior;
         resultadoAnterior = aux ^ matMensajes2[i][j];   
      }
      if (resultadoAnterior == 1) {
         printf("Error detectado en la columna: %d\n",j);
         for ( i = 0 ; i <= N ; i++ )
            matErrores[i][j] = 1;
      } 
       
      else
         for ( i = 0 ; i <= N ; i++ )
            matErrores[i][j] = 0;
          
   }   
            
   for ( i = 0 ; i <= N ; i++ )
      for ( j = 0 ; j <= M ; j++ )
         if (matErrores[i][j] == 1)
            printf("Error en el bit ubicado en la fila %d y columna %d\n",i,j);         
}


int main() {

    //int argc, char argv[]
    //char nombre_archivo = argv[1];
    //char nombre_archivo; 
    //strcpy(nombre_archivo,);
    int  N, M;
    int estaP = 1;
    /*
    
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
    float equivoc_AB, entropiaDeA = 0, entropiaDeB = 0;
    int   matMensajes[N+1][M+1];
    int   matMensajes2[N+1][M+1];

    //A)
    leerArchivo("tp4_sample0.txt", fuente, matrizCanal); 

    //B)
    calculaSucSimul(probSucSimul,fuente,matrizCanal);    

    calculaProbB(probB,fuente,matrizCanal);

    calculoMatrizPosteriori(matrizPosteriori,fuente,matrizCanal,probB);

    equivocacionCanal(probSucSimul,matrizPosteriori,&equivoc_AB);
    calculoDeEntropiaDeX(fuente,&entropiaDeA);
    
    calculoDeEntropiaDeX(probB,&entropiaDeB);
    
    printf("La equivocacion del canal es H(A/B) = %f bits\n",equivoc_AB);
    printf("H(B/A) = %f bits\n",entropiaDeB - (entropiaDeA - equivoc_AB));
    printf("La informacion mutua del canal es I(A,B) = %f bits\n",entropiaDeA - equivoc_AB);
    printf("\n---Entropias del canal:---\n");
    printf("H(A)   = %f\n",entropiaDeA);
    printf("H(B)   = %f\n",entropiaDeB);
    printf("H(A,B) = %f\n",entropiaDeB + equivoc_AB);
    entropiaPosteriori_A(matrizPosteriori,0);
    entropiaPosteriori_A(matrizPosteriori,1);
   
    //C)
    //generaMensajes(matMensajes,10,10,fuente);
   
    //D)
    if (estaP)
      // paridadCruzada(matMensajes,10,10);
   
    //E)
    //generaMat2(matrizCanal,matMensajes,matMensajes2,10,10);
    //analiza(matMensajes2,10,10);

    return 0;
}

