#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARCH0 "arch0.bin"
#define ARCH_DIC "diccionario.bin"
#define TAM_MAX 1000 /**cantidad de caracteres maximo del texto que se ingresa por usuario**/
#define TAM_DIC 10000 /**cantidad de palabras que se pueden guardar en el dic**/

typedef struct {
  char palabra[20];
  int idDOC;
  int pos; //incrementa palabra por palabra, y no letra por letra
}termino;

typedef struct nodoT    ///Estructura de Ocurrencias
{
    int idDOC;
    int pos;
    struct nodoT* sig;
}nodoT;

typedef struct nodoA    ///Estructura Arbol Buscador
{
    char palabra[20];
    int frecuencia;
    nodoT* ocurrencias;
    struct nodoA* der;
    struct nodoA* izq;
}nodoA;

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

void arregloToArchivoDic(termino dic[], int v)
{
    FILE* pfile = fopen(ARCH_DIC, "wb");
    int i = 0;

    if( pfile )
    {
        while( i < v )
        {
            fwrite(&dic[i], sizeof(dic[i]), 1, pfile);

            i++;
        }

        fclose(pfile);
    }

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

    arregloToArchivoDic(dic, (*v));
}

void mostrarArchivoDic()
{
    FILE* pfile = fopen(ARCH_DIC,"rb");
    termino t;

    if( pfile )
    {
        while( fread(&t,sizeof(termino),1,pfile) > 0 )
        {
            mostrarUnTermino(t);
        }

        fclose(pfile);
    }
}

///Operaciones

nodoT* creaNodoOcurrencia(termino dato) ///Crea un nuevo nodo de ocurrencias para una palabra en el buscador
{
    nodoT* nuevo = (nodoT*)malloc(sizeof(nodoT));
    nuevo->idDOC = dato.idDOC;
    nuevo->pos = dato.pos;
    nuevo->sig = NULL;
    return nuevo;
}

nodoA* creaNodoBuscador(termino dato)
{
    nodoA* nuevo = (nodoA*)malloc(sizeof(nodoA));
    strcpy(nuevo->palabra,dato.palabra);
    nuevo->frecuencia = 1;
    nuevo->ocurrencias = creaNodoOcurrencia(dato);
    nuevo->der = NULL;
    nuevo->izq = NULL;
    return nuevo;
}

nodoA* encuentra(nodoA* buscador,termino dato)     ///Se fija si en el buscador la palabra ya esta, y devuelve su posicion de ser asi
{
    nodoA* existe = NULL;
    if(buscador)
    {
        if(strcmp(buscador->palabra,dato.palabra) < 0)
        {
            return encuentra(buscador->der,dato);
        }
        else
        {
            if(strcmp(buscador->palabra,dato.palabra) > 0)
            {
                return encuentra(buscador->izq,dato);
            }
            else
            {
                if(strcmp(buscador->palabra,dato.palabra) == 0)
                {
                    existe = buscador;
                }
            }
        }
    }
    return existe;
}

void insertaOcurrencia(nodoT** lista,termino dato)  ///Inserta en la sublista de forma ordenada, primero por id y despues por posicion
{
    if(*lista == NULL || (*lista)->idDOC > dato.idDOC || ((*lista)->idDOC == dato.idDOC && (*lista)->pos > dato.pos))
    {
        nodoT* nuevo = creaNodoOcurrencia(dato);
        nuevo->sig = *lista;
        *lista = nuevo;
    }
    else
    {
       insertaOcurrencia(&(*lista)->sig,dato);
    }
}

void insertaPalabra(nodoA** A,termino dato)     ///Inserta en el arbol buscador ordenado por orden alfabetico
{
    if(*A)
    {
        if(strcmp((*A)->palabra,dato.palabra) < 0)
        {
            insertaPalabra(&(*A)->der,dato);
        }
        else
        {
            insertaPalabra(&(*A)->izq,dato);
        }
    }
    else
    {
        *A = creaNodoBuscador(dato);
    }
}

void cargaBuscador(nodoA** A,termino dato) ///Busca si la palabra ya esta en el buscador. Si esta agrega ocurrencia y aumenta la frecuencia, sino, agrega la palabra
{
    nodoA* existe = encuentra(*A,dato);
    if(existe != NULL)
    {
        insertaOcurrencia(&(existe->ocurrencias),dato);
        existe->frecuencia++;
    }
    else
    {
        insertaPalabra(A,dato);
    }
}

void diccionarioToBuscador(nodoA** A)   ///Lee el diccionario y va cargando sus datos al buscador
{
    FILE* arch = fopen("diccionario.bin","rb");
    termino dato;
    if(arch)
    {
        while(fread(&dato,sizeof(termino),1,arch) > 0)
        {
            cargaBuscador(A,dato);
        }
    }
    fclose(arch);
}

void inorder(nodoA* A)
{
    if(A)
    {
        inorder(A->izq);
        printf("%s - %i \n",A->palabra,A->frecuencia);
        inorder(A->der);
    }
}

int main()
{
    termino diccionario[TAM_DIC];
    nodoA* buscador = NULL;
    int validosDiccionario = 0;

    crearArchivo();

    archivoToDic(diccionario, &validosDiccionario);

    printf("\nArchivo Dic:\n");
    mostrarArchivoDic();

    diccionarioToBuscador(&buscador);

    inorder(buscador);

    return 0;
}
