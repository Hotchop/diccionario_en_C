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

typedef struct    ///Frecuencia de termino en un doc especifico
{
    char palabra[20];
    int frecuencia;

}frecuenciaArchivo;

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

///OP 5

int cuentaID(nodoT* lista,int idDOC)    ///Cuenta todas las ocurrencias del termino en un idDOC especifico
{
    int existe = 0;
    while(lista && lista->idDOC > idDOC)
    {
        lista = lista->sig;
    }
    while(lista && lista->idDOC == idDOC)
    {
        existe++;
        lista = lista->sig;
    }
    return existe;
}


frecuenciaArchivo maxfrecuencia(nodoA* A,int idDOC)   ///Regresa el valor del termino con mas frecuencia en un doc
{
    frecuenciaArchivo aux,max1,max2;
    strcpy(aux.palabra,"");
    aux.frecuencia = -1;
    if(A)
    {
        int existe = cuentaID(A->ocurrencias,idDOC);
        if(existe != 0)
        {
            strcpy(aux.palabra,A->palabra);
            aux.frecuencia = existe;
            if(A->der != NULL)
            {
                max1 = maxfrecuencia(A->der,idDOC);
                if(aux.frecuencia > max1.frecuencia)
                {
                    max1 = aux;
                }
            }
            else
            {
                max1 = aux;
            }
            if(A->izq != NULL)
            {
                max2 = maxfrecuencia(A->izq,idDOC);
                if(aux.frecuencia > max2.frecuencia)
                {
                    max2 = aux;
                }
            }
            else
            {
                max2 = aux;
            }
            if(max1.frecuencia >= max2.frecuencia)
            {
                return max1;
            }
            else
            {
                return max2;
            }
        }
        else
        {
            max1 = maxfrecuencia(A->der,idDOC);
            max2 = maxfrecuencia(A->izq,idDOC);
            if(max1.frecuencia > max2.frecuencia)
            {
                return max1;
            }
            else
            {
                return max2;
            }
        }
    }
    else
    {
        return aux;
    }
}


void frecuenciaPorDoc(nodoA* A) ///Funcion principal, muestra el termino mas frecuente en el doc, o error si no existe el id
{
    int idDOC;
    printf("Ingrese id del documento\n");
    scanf("%i",&idDOC);
    frecuenciaArchivo resultado = maxfrecuencia(A,idDOC);
    if(resultado.frecuencia > 0)
    {
        printf("La palabra mas frecuente en el documento %i es [ %s ] (f = %i)\n",idDOC,resultado.palabra,resultado.frecuencia);
    }
    else
    {
        printf("El documento no existe\n");
    }
}

///OP 6
int min(int a,int b)    ///Compara el minimo de dos valores
{
   if (a < b )
       return a;
   else
      return b;
}

int Levenshtein(char *s1,char *s2)  ///Devuelve un valor de a cuanta distancia estan dos terminos
{
    int t1,t2,i,j,*m,costo,res,ancho;

// Calcula tamanios strings
    t1=strlen(s1);
    t2=strlen(s2);

// Verifica que exista algo que comparar
    if (t1==0) return(t2);
    if (t2==0) return(t1);
    ancho=t1+1;

// Reserva matriz con malloc                     m[i,j] = m[j*ancho+i] !!
    m=(int*)malloc(sizeof(int)*(t1+1)*(t2+1));
    if (m==NULL) return(-1); // ERROR!!

// Rellena primera fila y primera columna
    for (i=0; i<=t1; i++) m[i]=i;
    for (j=0; j<=t2; j++) m[j*ancho]=j;

// Recorremos resto de la matriz llenando pesos
    for (i=1; i<=t1; i++) for (j=1; j<=t2; j++)
        {
            if (s1[i-1]==s2[j-1]) costo=0;
            else costo=1;
            m[j*ancho+i]=min(min(m[j*ancho+i-1]+1,     // Eliminacion
                                       m[(j-1)*ancho+i]+1),              // Insercion
                                m[(j-1)*ancho+i-1]+costo);
        }      // Sustitucion

// Devolvemos esquina final de la matriz
    res=m[t2*ancho+t1];
    free(m);
    return(res);
}

void buscasimilar(nodoA* A,char* palabra)   ///Muestra por pantalla todas las palabras con una distancia <= 3
{
    if(A)
    {
        buscasimilar(A->izq,palabra);
        int distancia = Levenshtein(palabra,A->palabra);
        if(distancia <= 3 && distancia != 0)
        {
            printf("%s\n",A->palabra);
        }
        buscasimilar(A->der,palabra);

    }
}

void sugerirSimilares(nodoA* A)
{
    char palabra[20];
    printf("Ingrese termino\n");
    fflush(stdin);
    gets(palabra);
    printf("Sugerencias de terminos similares a %s:\n",palabra);
    buscasimilar(A,palabra);
}

///Menu
void menu(nodoA* buscador)
{
    char opcion = '-';
    while(opcion != '*')
    {
        printf("\n\nSeleccione operacion a realizar (Ingrese * para finalizar programa)\n");
        printf("-------------------------------------------------------------------\n\n");
        printf("[A] - Muestra todos los terminos y sus frecuencias\n");
        printf("[1] - \n");
        printf("[2] - \n");
        printf("[3] - \n");
        printf("[4] - \n");
        printf("[5] - Muestra termino mas frecuente de un archivo\n");
        printf("[6] - Sugerencias de terminos similares\n");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
            case 'A':inorder(buscador);
                break;
            case '1':;
                break;
            case '2':;
                break;
            case '3':;
                break;
            case '4':;
                break;
            case '5':frecuenciaPorDoc(buscador);
                break;
            case '6':sugerirSimilares(buscador);
                break;
        }
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

    menu(buscador);

    return 0;
}
