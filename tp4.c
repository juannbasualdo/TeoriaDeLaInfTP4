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
}

void calculoMatrizPosteriori(float matrizPosteriori[MAX_ROWS][MAX_COLS], float fuente[], float matrizCanal[MAX_ROWS][MAX_COLS], float probB[]) {

   int i, j;

   for ( i = 0 ; i < 2 ; i++ )
      for ( j = 0 ; j < 2 ; j++ )
         if (probB[j] != 0)
            matrizPosteriori[i][j] =  ((fuente[i]*matrizCanal[i][j])/probB[j]);

}

void calculaSucSimul(float probSucSimul[], float fuente[], float matrizCanal[MAX_ROWS][MAX_COLS]) {
    int i, j, k = 0;

    for ( i = 0 ; i < 2 ; i++ ) {
       for ( j = 0 ; j < 2 ; j++ ) {
          probSucSimul[k++] = (fuente[i])*(matrizCanal[i][j]);
       }
    }
 
}

void equivocacionCanal(float probSucSimul[], float matrizPosteriori[MAX_ROWS][MAX_COLS], float *equivoc_AB) {
   int k = 0;

   for (int i = 0 ; i < 2 ; i++) 
      for ( int j = 0 ; j < 2 ; j++)
         if (matrizPosteriori[i][j] != 0)
            (*equivoc_AB) += probSucSimul[k++]*log2(1/matrizPosteriori[i][j]);

         
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
   printf("H(A/b = %d) = %f bits\n",num,suma);
}

void generaMensajes(int matMensajes[MAX_MENS][MAX_MENS], int N, int M, float fuente[]) {

   int    i, j;
   double random_value;
   int    random_number;

   // Establecer la semilla para la generación de números aleatorios
   srand(time(NULL));


   printf("\n");
   for ( i = 0 ; i < N ; i++ ) {
      for ( j = 0 ; j < M ; j++ ) {
         // Generar un número aleatorio entre 0 y RAND_MAX
         random_number = rand();
         // Convertir el número a un valor entre 0 y 1
          random_value = (double)random_number / RAND_MAX;
          //printf("Valor aleatorio [%d][%d]: %f      ",i,j,random_value);

         if (random_value >= 0 && random_value <= fuente[0])
            matMensajes[i][j] = 0;
         else
            matMensajes[i][j] = 1;
            
      }  
      printf("\n");
   }    
   
   printf("\n");
   printf("Simulacion del envio de %d mensajes aleatorios de longitud %d:\n",N,M);

   for (i = 0 ; i < N ; i ++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matMensajes[i][j]);
      printf("\n");
   }   

}

void paridadCruzada(int matMensajes[MAX_MENS][MAX_MENS], int *N, int *M) {
   int resultadoAnterior;
   int i, j, aux;
   int bitVRC, bitLRC;

 
   //Paridad vertical(VRC):
   for ( j = 0 ; j < *M ; j ++) {
      resultadoAnterior = matMensajes[*N - 1][j] ^ matMensajes[*N - 2][j];
      for ( i = (*N - 3) ; i >= 0 ; i-- ) { //Hace el XOR entre cada uno de los elementos de cada columna
         aux = resultadoAnterior;
         resultadoAnterior = matMensajes[i][j] ^ aux;
      }
      if (resultadoAnterior == 1)
         matMensajes[*N][j] = 1;
      else
         matMensajes[*N][j] = 0;        
   }

   //Paridad longitudinal(LRC):
   for ( i = 0 ; i < *N ; i ++) {
      resultadoAnterior = matMensajes[i][0] ^ matMensajes[i][1];
      for ( j =  2 ; j < *M ; j++ ) { //Hace el XOR entre cada uno de los elementos de cada fila
         aux = resultadoAnterior;
         resultadoAnterior = matMensajes[i][j] ^ aux;
      }
      if (resultadoAnterior == 1)
         matMensajes[i][*M] = 1;
      else
         matMensajes[i][*M] = 0;        
   }

   //XOR entre los bits de la ultima fila (resultantes de VRC):
   j = 2;
   resultadoAnterior = matMensajes[*N][0] ^ matMensajes[*N][1];
   while( j < *M ) {
      aux = resultadoAnterior;
      resultadoAnterior = matMensajes[*N][j] ^ aux;
      j++;
   }
   bitVRC = resultadoAnterior;

   //XOR entre los bits de la ultima columna (resultantes de LRC):
   i = *N - 3;
   resultadoAnterior = matMensajes[*N - 1][*M] ^ matMensajes[*N - 2][*M];
   while( i >= 0 ) {
      aux = resultadoAnterior;
      resultadoAnterior = matMensajes[i][*M] ^ aux;
      i--;
   }
   bitLRC = resultadoAnterior;

   

   printf("\n");
   printf("Matriz resultante de aplicar el metodo de paridad cruzada:\n");
   for (i = 0 ; i <= *N ; i ++ ) {
      for ( j = 0 ; j <= *M ; j++ )
         printf("%d  ",matMensajes[i][j]);
      printf("\n");
   }          

   if (bitVRC == bitLRC) {
      matMensajes[*N][*M] = bitVRC; //bit de paridad cruzada
      printf("Los bits coinciden (VRC y LRC)\n");
   }
   else
      printf("Los bits no coinciden\n");

   (*N)++;
   (*M)++;

}


void enviaMensajes(float matrizCanal[MAX_ROWS][MAX_COLS], int matMensajes[MAX_LONG][MAX_MENS], int matMensajes2[MAX_LONG][MAX_MENS], int N, int M, int estaP) {

   int    i, j;
   double random_value;
   int    random_number;

   printf("\nMatriz del canal:\n");
   for ( i = 0 ; i < 2 ; i++ ) {
      for ( j = 0 ; j < 2 ; j++ )
         printf("%f ",matrizCanal[i][j]);
      printf("\n");
   }      
   printf("\n");

   for ( i = 0 ; i < N ; i++ ) {
      for ( j = 0 ; j < M ; j++ ) {
         // Generar un número aleatorio entre 0 y RAND_MAX
         random_number = rand();
         // Convertir el número a un valor entre 0 y 1
          random_value = (double)random_number / RAND_MAX;
          //printf("Valor aleatorio [%d][%d]: %f     ",i,j,random_value);

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
      //printf("\n");
   }

   

   printf("\nMatriz 2: (junto con los bits de paridad)\n");
   for (i = 0 ; i < N ; i ++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matMensajes2[i][j]);
      printf("\n");     
   }
   printf("\n");

   if (estaP) {
      N--;
      M--;
   }
}


void analiza(int matMensajes2[MAX_LONG][MAX_MENS], int N, int M) {
   int i, j, resultadoAnterior, aux;
   int matErrores[MAX_LONG][MAX_MENS], cantSospechosos = 0, filaError = 0, columnaError = 0; 
   
   //bitsCol[MAX_MENS] = {0}, bitsFil[MAX_LONG] = {0};
   
   //primero hago el XOR entre los bits de cada fila
   //si no hay errores (al menos no pares) deberia darme 0 el XOR de cada una de las filas
   for ( i = 0 ; i < N ; i++ ) {
      resultadoAnterior = matMensajes2[i][0] ^ matMensajes2[i][1];
      for ( j = 2 ; j < M ; j++ ) {
         aux = resultadoAnterior;
         resultadoAnterior = aux ^ matMensajes2[i][j];   
      }
      if (resultadoAnterior == 1) {
         for ( j = 0 ; j < M ; j++ ) 
            matErrores[i][j] = 1;   
            
      }
      else 
         for ( j = 0 ; j < M ; j++ )
            matErrores[i][j] = 0;          
   }   

   //hago el XOR entre los bits de cada columna
   //si no hay errores (al menos no pares) deberia darme 0 el XOR de cada una de las columnas
   for ( j = 0 ; j < M ; j++ ) {
      resultadoAnterior = matMensajes2[0][j] ^ matMensajes2[1][j];
      for ( i = 2 ; i < N ; i++ ) {
         aux = resultadoAnterior;
         resultadoAnterior = aux ^ matMensajes2[i][j];   
      }
      if (resultadoAnterior == 1) {
         for ( i = 0 ; i < N ; i++ ) 
            matErrores[i][j]++;
      }      
      else
         for ( i = 0 ; i < N ; i++ ) {
            matErrores[i][j];   
           
         } 
   }       
        


   printf("\nMatriz de errores: \n");         
   for ( i = 0 ; i < N ; i++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matErrores[i][j]);
      printf("\n");
   }      
   
  
   //no considero erroneos los bits de paridad cambiados
   for ( i = 0 ; i < N ; i++ ) 
      for ( j = 0 ; j < M ; j++ ) {
         if ( matErrores[i][j] == 2 ) {
            printf("Bit sospechoso en la fila: %d y columna: %d\n",i,j);
            cantSospechosos++; 
            filaError    = i;
            columnaError = j;
         }        
      }      
   printf("\n"); 


   if ( cantSospechosos == 1 ) {
      printf("Bit erroneo en la fila: %d y la columna: %d\n",filaError,columnaError); 
      printf("Al ser solo un error se puede corregir\n\n");
   }
   else
      printf("Por existir mas de un error, no es posible su correccion, unicamente se informan sospechosos\n\n");
   
}


void compara(int matMensajes[MAX_LONG][MAX_MENS], int matMensajes2[MAX_LONG][MAX_MENS], int N, int M) {
   int i, j, errores = 0;

   printf("--Comparacion bit a bit de ambas matrices--\n");

   printf("Matriz 1:\n");
   for ( i = 0 ; i < N ; i++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matMensajes[i][j]);
      printf("\n");
   }      

   printf("\nMatriz 2 (paso a traves del canal):\n");
   for ( i = 0 ; i < N ; i++ ) {
      for ( j = 0 ; j < M ; j++ )
         printf("%d  ",matMensajes2[i][j]);
      printf("\n");
   }   

   for ( i = 0 ; i < N ; i++ )
      for ( j = 0 ; j < M ; j++ )
         if ( matMensajes[i][j] != matMensajes2[i][j] ) {
            printf("Error en el bit de la fila: %d y columna: %d\n",i,j);
            errores++;
         }   

   if (errores == 0)
      printf("La matriz no se vio alterada al pasar por el canal\n");  
   else
      printf("Cantidad total de errores:%d\n",errores);       
}


int main(int argc, char *argv[]) {

    char *nombre_archivo = argv[1]; 
    int  N, M;
    int estaP = 0;
   
    N = atoi(argv[2]);
    M = atoi(argv[3]);
    if (argc == 5)
       if (!strcmp(argv[4],"-p"))
          estaP = 1;
    

    float fuente[MAX_COLS]; //prob de que entre un 0 y de que entre un 1
    float matrizCanal[MAX_ROWS][MAX_COLS];
    float probPosteriori[MAX_ROWS][MAX_COLS];
    float probB[MAX_COLS] = {0}; //probabilidad de que salga 0 y 1 sin saber que entro
    float probSucSimul[4]; //probabilidad sucesos simultaneos
    float matrizPosteriori[MAX_ROWS][MAX_COLS];
    float equivoc_AB = 0, entropiaDeA = 0, entropiaDeB = 0;
    int   matMensajes[MAX_LONG][MAX_MENS];
    int   matMensajes2[MAX_LONG][MAX_MENS];
   
 

    //A)
    leerArchivo(nombre_archivo, fuente, matrizCanal); 

    //B)
    calculaSucSimul(probSucSimul,fuente,matrizCanal);    

    
    for  (int i = 0 ; i++ ;i <2)
       for ( int j = 0 ; j++ ; j <2 )
          printf("%f  ",matrizCanal[i][j]);

    printf("\n\n\n");   

    calculaProbB(probB,fuente,matrizCanal);

    calculoMatrizPosteriori(matrizPosteriori,fuente,matrizCanal,probB);

    equivocacionCanal(probSucSimul,matrizPosteriori,&equivoc_AB);
    
    calculoDeEntropiaDeX(fuente,&entropiaDeA);
    
    calculoDeEntropiaDeX(probB,&entropiaDeB);
    
    printf("H(A/B) = %f bits\n",equivoc_AB);
    printf("H(B/A) = %f bits\n",entropiaDeB - (entropiaDeA - equivoc_AB));

      //correccion -> eliminar funcion
    if (entropiaDeA - equivoc_AB > 0) {
      printf("I(A,B) = %f bits\n",entropiaDeA - equivoc_AB);
      printf("I(B,A) = %f bits\n",entropiaDeA - equivoc_AB);
    }
    else {
      printf("I(A,B) = %f bits\n",equivoc_AB - entropiaDeA);
      printf("I(B,A) = %f bits\n",equivoc_AB - entropiaDeA);
      
    }

    printf("\n---Entropias del canal:---\n");
    printf("H(A)   = %f bits\n",entropiaDeA);
    printf("H(B)   = %f bits\n",entropiaDeB);
    printf("H(A,B) = %f bits\n",entropiaDeB + equivoc_AB);
    entropiaPosteriori_A(matrizPosteriori,0);
    entropiaPosteriori_A(matrizPosteriori,1);
   
    //C)
    generaMensajes(matMensajes,N,M,fuente);
   
    //D)
    if (estaP)
       paridadCruzada(matMensajes,&N,&M);
   
    //E)
    enviaMensajes(matrizCanal,matMensajes,matMensajes2,N,M,estaP);
    if (estaP) {
      N--;
      M--;
    }
    if (estaP)
       analiza(matMensajes2,N,M);
    
    compara(matMensajes,matMensajes2,N,M);

    return 0;
}

