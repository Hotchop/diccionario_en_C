#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define ARCH_DIC "diccionario.bin"
#define ARCH_DEFAULT "arch%i.bin"
#define TAM_MAX 10000 /**cantidad maxima de caracteres del texto que se ingresa por usuario**/
#define TAM_DIC 10000 /**cantidad maxima de palabras que se pueden guardar en el dic**/
#define FIL 10

/**estructuras**/
typedef struct
{
    char palabra[20];
    int idDOC;
    int pos; //incrementa palabra por palabra, y no letra por letra
} termino;

typedef struct nodoT    ///Estructura de Ocurrencias
{
    int idDOC;
    int pos;
    struct nodoT* sig;
} nodoT;

typedef struct nodoA    ///Estructura Arbol Buscador
{
    char palabra[20];
    int frecuencia;
    nodoT* ocurrencias;
    struct nodoA* der;
    struct nodoA* izq;
} nodoA;

typedef struct    ///Frecuencia de termino en un doc especifico
{
    char palabra[20];
    int frecuencia;

} frecuenciaArchivo;

typedef struct nodoP    ///Estructura Lista de Palabras
{
    termino dato;
    struct nodoP* sig;
}nodoP;

/**cabeceras**/

/**archivos y diccionario**/
void crearArchivos(int cantArch);
void mostrarArchivos(int cantArch);
void limpiarArreglo(char a[],int j);
void mostrarUnTermino(termino t);
void mostrarDic(termino dic[],int v);
void insertarTermino(termino dic[],int* validos, char termino[], int idDocTermino, int posTerminoEnDoc);
void arregloToArchivoDic(termino dic[], int validos);
void archivoToDic(termino dic[], int* validos, int cantArch);
void mostrarArchivoDic();

/**arbol buscador**/
nodoT* creaNodoOcurrencia(termino dato);
nodoA* creaNodoBuscador(termino dato);
nodoA* encuentra(nodoA* buscador,char* dato);
void insertaOcurrencia(nodoT** lista,termino dato);
void insertaPalabra(nodoA** A,termino dato);
void cargaBuscador(nodoA** A,termino dato);
void diccionarioToBuscador(nodoA** A);
void inorder(nodoA* A);

/**punto 1*/
void buscarPostearAparicionesPalabra(nodoA* arbol,char* palabraBuscar);

/**punto 2*/
int cargarArregloIds(int a[], int dim);
void mostrarArregloids(int a[], int v);
int existePalabraEnDoc(nodoA* arbol, char* palabra, int id);
void buscarAparicionesAND(nodoA* arbol, char* palabra, int cantArch);

/**punto 3*/
int ingresarTerminosUsuario(int fil,int col, char t[][col]);
void mostrarTerminosUsuario(int fil,int col,char t[][col], int v);
void buscarTerminosMismoDocumento(nodoA* arbol, int cantArch);

/**punto 4**/
nodoP* creaNodoPalabra(char* aux);
void palabraToLista(nodoP** lista,char* aux);
void separaFrase(nodoP** lista,char* texto);
int comparaPalabras(nodoA* A,nodoP** lista);
nodoP* existeFrase(nodoA* A,nodoP* lista);
void buscaFrase(nodoA* A);

/**punto 5**/
int cuentaID(nodoT* lista,int idDOC);
frecuenciaArchivo maxfrecuencia(nodoA* A,int idDOC);
void frecuenciaPorDoc(nodoA* A);

/**punto 6**/
int minimo(int a,int b);
int Levenshtein(char *s1,char *s2);
void buscasimilar(nodoA* A,char* palabra);
void sugerirSimilares(nodoA* A);

/**generales**/
void limpiarConsola();
void menu(nodoA* buscador);

/**funciones archivos y diccionario**/

void crearArchivos(int cantArch)
{
    /** crea N archivos segun se pase por parametro (cantArch)
    la carga de archivos es manual, debe ser texto sin comas ni puntos*/

    int i = 0;
    char nombreArch[20];

    while( i < cantArch )
    {
        snprintf(nombreArch,20,ARCH_DEFAULT,i);

        FILE* pfile = fopen(nombreArch, "wb");
        char texto[TAM_MAX] = { 0 };

        if( pfile )
        {
            printf("\nSe creara el archivo con id %i con el nombre %s...\n",i,nombreArch);
            printf("\nIngrese el texto >> \n\n");
            fflush(stdin);
            gets(texto);

            fwrite(&texto,sizeof(char)* strlen(texto),1,pfile);

            fclose(pfile);
        }

        i++;
    }
}


void mostrarArchivos(int cantArch)
{
    /**muestra N archivos ya cargados**/
    int i = 0;
    char nombreArch[20];

    while( i < cantArch )
    {
        snprintf(nombreArch,20,ARCH_DEFAULT,i);
        FILE* pfile = fopen(nombreArch, "rb");
        char texto[TAM_MAX] = { 0 };

        printf("\nContenido del archivo: %s\n\n",nombreArch);

        if( pfile )
        {
            fread(&texto, sizeof(texto),1,pfile);

            puts(texto);

            fclose(pfile);
        }

        i++;
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

void insertarTermino(termino dic[],int* validos, char termino[], int idDocTermino, int posTerminoEnDoc)
{
    /**busca la ultima pos valida e inserta el nuevo termino */

    int i = 0;

    while( i < (*validos) )
    {
        i++;
    }

    strcpy(dic[i].palabra,termino);
    dic[i].idDOC = idDocTermino;
    dic[i].pos = posTerminoEnDoc;

    (*validos)++;
}

void arregloToArchivoDic(termino dic[], int validos)
{
    FILE* pfile = fopen(ARCH_DIC, "wb");
    int i = 0;

    if( pfile )
    {
        while( i < validos )
        {
            fwrite(&dic[i], sizeof(dic[i]), 1, pfile);

            i++;
        }

        fclose(pfile);
    }

}

void archivoToDic(termino dic[], int* validos, int cantArch)
{
    /**abre cada archivo, separa cada palabra del texto del archivo y la guarda en
    una celda del arreglo de terminos. Una vez finaliza con todos los archivos,
    crea el archivo dic.bin con la informacion del arreglo*/
    int i_arch = 0; /**para recorrer todos los archivos desde 0 hasta cantArch**/
    char nombreArch[20];

    while( i_arch < cantArch )
    {
        snprintf(nombreArch,20,ARCH_DEFAULT,i_arch);
        FILE* pfile = fopen(nombreArch,"rb");
        char texto[TAM_MAX] = { 0 };
        int letraActualDoc = 0;
        int letraActualPalabra = 0;
        char palabra[20] = { 0 };
        int posPalabraEnDoc = 0;
        int cantLetrasTotalesDoc;

        if( pfile )
        {
            fread(&texto,sizeof(texto),1,pfile);

            cantLetrasTotalesDoc = strlen(texto);

            while( letraActualDoc < cantLetrasTotalesDoc ) /**compara todas las letras del doc**/
            {
                while( isalpha(texto[letraActualDoc]) ) /** mientras la letra actual sea una letra del abecedario**/
                {
                    /**copia cada letra leida distinta de espacio a la cadena auxiliar*/
                    palabra[letraActualPalabra] = texto[letraActualDoc];

                    letraActualPalabra++;

                    letraActualDoc++;
                }

                /**tras salir del while,palabra contiene 1 termino, los caracteres hasta
                el primer espacio encontrado*/

                /**ese termino se guarda en una celda del arreglo de terminos*/
                insertarTermino(dic,validos,palabra,i_arch,posPalabraEnDoc);

                /** posicion de la siguiente palabra en el doc*/
                posPalabraEnDoc++;

                /**limpia la cadena auxiliar para repetir el proceso con la siguiente palabra del doc*/
                limpiarArreglo(palabra, letraActualPalabra);
                letraActualPalabra = 0;


                /**el proceso sigue con todas las letras*/
                letraActualDoc++;
            }

            fclose(pfile);

        }

        /**el proceso se repite con el siguiente archivo*/
        i_arch++;
    }

    /**copia la info al archivo del dic*/
    arregloToArchivoDic(dic, (*validos));

}

void mostrarArchivoDic()
{
    FILE* pfile = fopen(ARCH_DIC,"rb");
    termino t;
    int cantTerminos = 0;

    if( pfile )
    {
        while( fread(&t,sizeof(termino),1,pfile) > 0 )
        {
            mostrarUnTermino(t);
            cantTerminos++;
        }

        fclose(pfile);
    }

    printf("\nEl diccionario contiene %i terminos.\n", cantTerminos);
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

nodoA* encuentra(nodoA* buscador,char* dato)     ///Se fija si en el buscador la palabra ya esta, y devuelve su posicion de ser asi
{
    nodoA* existe = NULL;
    if(buscador)
    {
        if(strcmp(buscador->palabra,dato) < 0)
        {
            return encuentra(buscador->der,dato);
        }
        else
        {
            if(strcmp(buscador->palabra,dato) > 0)
            {
                return encuentra(buscador->izq,dato);
            }
            else
            {
                if(strcmp(buscador->palabra,dato) == 0)
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
    nodoA* existe = encuentra(*A,dato.palabra);
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

void buscarPostearAparicionesPalabra(nodoA* arbol,char* palabraBuscar)
{
    /**punto 1**/

    nodoA* rta;

    rta = encuentra(arbol,palabraBuscar);

    if( rta != NULL )
    {
        while( rta->ocurrencias != NULL )
        {
            printf("\nId: %i\n",rta->ocurrencias->idDOC);
            printf("\nPos: %i\n",rta->ocurrencias->pos);

            rta->ocurrencias = rta->ocurrencias->sig;
        }
    }
    else
    {
        printf("\nNo se encuentra la palabra en ningun documento.\n");
    }

}

int cargarArregloIds(int a[], int dim)
{
    int i = 0;
    int num;
    char opcion;

    do
    {
        printf("\nIngrese el id del archivo a buscar:\n");
        scanf("%i",&num);

        a[i] = num;
        i++;

        printf("\nContinuar: enter, Salir: ctrl+z\n>> ");
        fflush(stdin);
    }
    while( (opcion = getchar()) != EOF && i < dim );

    return i;
}

void mostrarArregloids(int a[], int v)
{
    int i = 0;

    while( i < v )
    {
        printf("| %i ",a[i]);

        i++;
    }

    printf("|\n");
}

int existePalabraEnDoc(nodoA* arbol, char* palabra, int id)
{
    int flag = 0;

    nodoA* rta = encuentra(arbol, palabra);

    if( rta != NULL )
    {
        nodoT* lista = rta->ocurrencias;

        if( lista != NULL )
        {
            if( lista->idDOC == id )
            {
                flag = 1;
            }
            else
            {
                while( lista != NULL && lista->idDOC < id )
                {
                    lista = lista->sig;
                }

                if( lista != NULL && lista->idDOC == id )
                {
                    flag = 1;
                }
            }
        }

    }

    return flag;
}

void buscarAparicionesAND(nodoA* arbol, char* palabra, int cantArch)
{
    /**punto 2 */

    int arregloIds[cantArch];
    int validosIds = cargarArregloIds(arregloIds,cantArch);
    int i = 0;
    int flag = 1; /**se asegura que esté en todos los docs(doc1 and doc2 and docN)*/

    printf("\nSe buscara la palabra %s en los docs: ",palabra);
    mostrarArregloids(arregloIds,validosIds);

    while( i < validosIds && flag == 1 )
    {
        int existePalabra = existePalabraEnDoc(arbol,palabra,i);

        if( !existePalabra ) /**si no existe en algun documento el and es falso*/
        {
            flag = 0;
        }

        i++;
    }

    if( flag == 1 )
    {
        /**printear todas las apariciones**/
        printf("\nLa palabra %s esta en todos los documentos y el 'and' es verdadero.\n",palabra);
        buscarPostearAparicionesPalabra(arbol, palabra);
    }
    else
    {
        printf("\nLa palabra %s NO esta en todos los documentos y el 'and' es falso.\n",palabra);
    }

}

int ingresarTerminosUsuario(int fil,int col, char t[][col])
{
    int i = 0;
    char opcion;

    do
    {
        printf("\nIngrese el termino %i >> ",i);
        fflush(stdin);
        scanf("%s",&t[i]);
        i++;

        printf("\nContinuar: enter, Salir: ctrl+z\n>> ");
        fflush(stdin);
    }
    while( ( opcion = getchar() ) != EOF && i < fil );

    return i;
}

void mostrarTerminosUsuario(int fil,int col,char t[][col], int v)
{
    int i = 0;

    while( i < v )
    {
        printf("- %s \n", t[i]);
        i++;
    }
}

void buscarTerminosMismoDocumento(nodoA* arbol, int cantArch)
{
    /**punto 3*/

    char terminosBuscar[FIL][20]; /**se pueden ingresar max de FIL(10) palabras de 20 caracteres max**/
    int validosTerminos = 0;
    int i = 0;
    int existePalabra = 0;
    int idBuscar;

    validosTerminos = ingresarTerminosUsuario(FIL,20,terminosBuscar);

    printf("\nIngrese el id del documento a realizar la busqueda >> ");
    scanf("%i",&idBuscar);

    printf("\nTerminos a buscar en el doc %i:\n",idBuscar);
    mostrarTerminosUsuario(FIL, 20, terminosBuscar, validosTerminos);

    while( i < validosTerminos )
    {
        nodoA* aux = arbol;
        existePalabra = existePalabraEnDoc(aux, terminosBuscar[i], idBuscar);

        if( existePalabra )
        {
            printf("\nLa palabra %s esta en el doc %i.\n",terminosBuscar[i],idBuscar);
            buscarPostearAparicionesPalabra(arbol,terminosBuscar[i]);
        }
        else
        {
            printf("\nLa palabra %s NO esta en el doc %i.\n",terminosBuscar[i],idBuscar);
        }

        i++;
    }

}

nodoP* creaNodoPalabra(char* aux)
{
    nodoP* nuevo = (nodoP*)malloc(sizeof(nodoP));
    strcpy(nuevo->dato.palabra,aux);
    nuevo->dato.idDOC = -1;
    nuevo->dato.pos = -1;
    nuevo->sig = NULL;
    return nuevo;
}

void palabraToLista(nodoP** lista,char* aux)
{
    nodoP* seg;
    nodoP* nuevo = creaNodoPalabra(aux);
    if(*lista == NULL)
    {
        nuevo->sig = *lista;
        *lista = nuevo;
    }
    else
    {
        seg = *lista;
        while(seg->sig != NULL)
        {
            seg = seg->sig;
        }
        seg->sig = nuevo;
    }
}

void separaFrase(nodoP** lista,char* texto)
{
    char aux[20] = {0};
    int i = 0, j = 0;
    while( i < strlen(texto) )
    {
        while(isalpha(texto[i]))
        {
            aux[j] = texto[i];
            i++;
            j++;
        }
        palabraToLista(lista,aux);
        while( j >= 0 )
        {
            aux[j] = 0;
            j--;
        }
        j = 0;
        i++;
    }
}

int comparaPalabras(nodoA* A,nodoP** lista)
{
    int resultado = 0;
    if((*lista)->sig != NULL)   /*Si no estoy en la ultima palabra de la frase*/
    {
        nodoA* existe = encuentra(A,(*lista)->sig->dato.palabra);
        if(existe != NULL)                                          /*Si la palabra siguente en la lista en el buscador*/
        {
            nodoT* ocurrencias = existe->ocurrencias;

            /* Comprueba de no estar al final y que la palabra sea continua en el mismo documento*/
            while((ocurrencias != NULL && ocurrencias->idDOC < (*lista)->dato.idDOC) || (ocurrencias != NULL && ocurrencias->idDOC == (*lista)->dato.idDOC && ocurrencias->pos < (((*lista)->dato.pos) + 1)))
            {
                ocurrencias = ocurrencias->sig;
            }
            /*Si se cumple, estan continuas y llama recursivamente*/
            if(ocurrencias != NULL && ocurrencias->pos == (((*lista)->dato.pos) + 1))
            {
                (*lista)->sig->dato.idDOC = ocurrencias->idDOC;
                (*lista)->sig->dato.pos = ocurrencias->pos;
                resultado = comparaPalabras(A,&(*lista)->sig);
            }
        }
    }
    else
    {
        if((*lista)->sig == NULL)   ///Si estoy parado en la ultima palabra, la frase esta completa
        {
            resultado = 1;
        }
        else
        {
            resultado = 2;  ///Si no, es que una de las palabras no estaba en el buscador
        }
    }
    return resultado;   ///Cualquier otro caso devuelve 0, y es porque la palabra existia, pero no en el mismo doc o en la pos siguiente
}

nodoP* existeFrase(nodoA* A,nodoP* lista)   ///0 == continua buscando, 1 == la frase esta, 2 == una de las palabras no existe
{
    int flag = 0;
    nodoP* resultado = NULL;
    nodoA* cabecera = encuentra(A,lista->dato.palabra);
    if(cabecera != NULL)
    {
        nodoT* ocurrencias = cabecera->ocurrencias;
        while(ocurrencias != NULL && flag == 0)
        {
            lista->dato.idDOC = ocurrencias->idDOC;
            lista->dato.pos = ocurrencias->pos;
            flag = comparaPalabras(A,&lista);
            if(flag == 0)
            {
                ocurrencias = ocurrencias->sig;
            }
        }
        if(ocurrencias != NULL && flag == 1)
        {
            resultado = lista;
        }
    }
    return resultado;
}

void buscaFrase(nodoA* A)
{
    char texto[100];
    nodoP* lista = NULL;
    printf("\nIngrese frase que desea buscar:\n");
    fflush(stdin);
    gets(texto);
    separaFrase(&lista,texto);
    nodoP* resultado = existeFrase(A,lista);
    if(resultado != NULL)
    {
        printf("\nLa frase existe en el documento id %i comenzando en la posicion %i\n",resultado->dato.idDOC,resultado->dato.pos);
    }
    else
    {
        printf("\nLa frase ingresada no se encuentra en ninguno de los documentos\n");
    }
}

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
int minimo(int a,int b)    ///Compara el minimo de dos valores
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
            m[j*ancho+i]=minimo(minimo(m[j*ancho+i-1]+1,     // Eliminacion
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

void limpiarConsola()
{
    printf("\n");
    system("pause");
    system("cls");
}

///Menu
void menu(nodoA* buscador)
{
    termino diccionario[TAM_DIC];
    int validosDiccionario = 0;
    char palabraBuscar[20] = { 0 };
    int cantArch;
    char opcion = '-';

    while(opcion != '*')
    {
        printf("\n\nSeleccione operacion a realizar (Ingrese * para finalizar programa)\n");
        printf("-------------------------------------------------------------------\n\n");
        printf("[A] - Ingresar archivo/s\n");
        printf("[B] - Mostrar archivo diccionario\n");
        printf("[C] - Muestra todos los terminos y sus frecuencias\n");
        printf("[1] - Buscar y postear ocurrencias de un termino\n");
        printf("[2] - Buscar y postear ocurrencias de un termino en un doc y otro/s\n");
        printf("[3] - Buscar y postear ocurrencias de mas de un termino en un mismo doc\n");
        printf("[4] - Buscar una frase en un archivo\n");
        printf("[5] - Muestra termino mas frecuente de un archivo\n");
        printf("[6] - Sugerencias de terminos similares\n\n>> ");

        fflush(stdin);
        scanf("%c",&opcion);

        switch (opcion)
        {
            case 'A':

                do
                {
                    printf("\nIngrese la cantidad de archivos a crear >> ");
                    fflush(stdin);
                    scanf("%i",&cantArch);
                }while( cantArch <= 0 );

                crearArchivos(cantArch);

                mostrarArchivos(cantArch);

                archivoToDic(diccionario, &validosDiccionario, cantArch);

                diccionarioToBuscador(&buscador);

                limpiarConsola();

                break;

            case 'B':

                if( cantArch > 0 )
                {

                    printf("\n-Archivo Diccionario-\n");
                    mostrarArchivoDic();
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case 'C':

                if( cantArch > 0 )
                {
                    inorder(buscador);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '1':

                if( cantArch > 0 )
                {
                    printf("\nIngrese la palabra a buscar (operacion or) >> ");
                    fflush(stdin);
                    gets(palabraBuscar);
                    buscarPostearAparicionesPalabra(buscador, palabraBuscar);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '2':

                if( cantArch > 0 )
                {
                    printf("\nIngrese la palabra a buscar (operacion and) >> ");
                    fflush(stdin);
                    gets(palabraBuscar);
                    buscarAparicionesAND(buscador,palabraBuscar,cantArch);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '3':

                if( cantArch > 0 )
                {
                    buscarTerminosMismoDocumento(buscador, cantArch);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '4':

                if( cantArch > 0 )
                {
                    buscaFrase(buscador);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '5':

                if( cantArch > 0 )
                {
                    frecuenciaPorDoc(buscador);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;

            case '6':

                if( cantArch > 0 )
                {
                    sugerirSimilares(buscador);
                }
                else
                {
                    printf("\nNo se han cargado archivos.\n\n");
                }

                limpiarConsola();

                break;
        }
    }
}

int main()
{
    nodoA* buscador = NULL;

    system("color 1F");

    SetConsoleTitle("Trabajo Practico Buscador");

    menu(buscador);

    static TCHAR szAppName[] = TEXT("DIC");

   MessageBox(NULL, TEXT("Gracias por usar el DIC !"),
          szAppName, MB_ICONWARNING);

    return 0;
}
