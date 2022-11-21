#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARCH0 "arch0.bin"
#define TAM_MAX 1000 /**cantidad de caracteres maximo del texto que se ingresa por usuario**/
#define TAM_DIC 10000 /**cantidad de palabras que se pueden guardar en el dic**/

typedef struct {
  char palabra[20];
  int idDOC;
  int pos; //incrementa palabra por palabra, y no letra por letra
}termino;

void crearArchivo()
{
    FILE* pfile = fopen(ARCH0, "wb");
    char aux[TAM_MAX];
    char* texto = NULL;

    if( pfile )
    {
        printf("\nIngrese el texto:\n");
        fflush(stdin);
        gets(aux);

        /**poner + 1 para que guarde el punto final en el texto tambien*/
        texto = (char*) malloc( sizeof(char) *strlen(aux) + 1);

        strcpy(texto,aux);

        fwrite(&texto,sizeof(texto),1,pfile);

        fclose(pfile);
    }
}

void mostrarArchivo()
{
    FILE* pfile = fopen(ARCH0, "rb");
    char* texto = NULL;

    if( pfile )
    {
        int tam = ftell(pfile);

        texto = (char*) malloc ( sizeof(tam) );

        fread(&texto, sizeof(texto),1,pfile);

        puts(texto);

        fclose(pfile);
    }
}

void limpiarArreglo(char a[],int j)
{
    while( j >= 0 )
    {
        a[j] = 0;
        j--;
    }
}

void mostrarUnTermino(termino t)
{
    printf("\n------------------------------------\n");
    printf("id DOC: %i\n",t.idDOC);
    printf("palabra: %s\n",t.palabra);
    printf("pos en el DOC: %i\n",t.pos);
    printf("\n-----------------------------------\n");
}

void mostrarDic(termino dic[],int v)
{
    int i = 0;

    while( i < v )
    {
        mostrarUnTermino(dic[i]);
        i++;
    }
}

void copiarTermino(termino* t, char p[], int p_id, int p_pos)
{
    strcpy(t->palabra, p);
    t->pos = p_pos;
    t->idDOC = p_id;
}

void archivoToDic(termino dic[], int* v)
{
    FILE* pfile = fopen(ARCH0,"rb");
    char* texto = NULL;
    int i = 0;
    int j = 0;
    char aux[20];
    int posPalabra = 0;

    if( pfile )
    {
        int tam = ftell(pfile);

        texto = (char*) malloc ( sizeof(tam) );

        fread(&texto,sizeof(texto),1,pfile);

        while( i < strlen(texto) )
        {
            while( isalpha(texto[i]) )
            {
                /*copia cada letra leida distinta de espacio a la cadena auxiliar*/
                aux[j] = texto[i];
                j++;
                i++;
            }

            /*tras salir del while, aux contiene 1 termino, los caracteres hasta
            el primer espacio encontrado*/
            /*en el sistema esto se va a guardar en una celda del arreglo*/
            copiarTermino(&dic[posPalabra], aux, 0, posPalabra);
            (*v)++;

            posPalabra++;

            /*limpiar el arreglo y volver el contador j a 0*/
            limpiarArreglo(aux, j);

            j = 0;

            /*el proceso sigue con todos los terminos*/
            i++;
        }

        fclose(pfile);
    }

}

int main()
{
    termino diccionario[TAM_DIC];
    int validosDiccionario = 0;

    crearArchivo();

    archivoToDic(diccionario, &validosDiccionario);

    printf("\nDiccionario\n");
    mostrarDic(diccionario, validosDiccionario);

    system("pause");

    return 0;
}
