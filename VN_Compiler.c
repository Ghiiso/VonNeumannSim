#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define FILENAME "code.txt"
/*
Definizioni liste
----------------------------------------------------------------------------------------------------------------------------
*/

struct CELLA{
    /*
    contiene le celle di memoria con contenuto e indirizzo    
    */
    int contenuto;
    int indirizzo;
    struct CELLA *prox;
};

typedef struct CELLA elemento;
typedef elemento *memlist;
memlist memoria;

struct LINEA{
    /*
    ogni elemento contiene la stringa del comando, il suo numero e la linea associata.
    se il numero è 0, il numero viene ignorato
    */
    char comando[6];
    int num_comando;
    int linea;
    struct LINEA *prox;
};

typedef struct LINEA ln;
typedef ln *linea;
linea listaLinee;


void append(memlist *l,int ind,int e){
    memlist p;
    if(*l==NULL){
        p = malloc(sizeof(elemento));
        p->contenuto = e;
        p->indirizzo = ind;
        p->prox = NULL;
        *l = p;
    }
    else append(&((*l)->prox),ind,e);
}

void inserisciOrdine(memlist *l,int ind,int elem){
    /*
    Inserisce una cella nella lista
    */
    memlist p, pCorrente, pPrecedente;
    pCorrente = *l;
    pPrecedente = NULL;
    while(pCorrente!=NULL && pCorrente->indirizzo < ind){
        pPrecedente = pCorrente;
        pCorrente = pCorrente->prox;
    }

    p = malloc(sizeof(elemento));
    p->indirizzo = ind;
    p->contenuto = elem;
    p->prox = pCorrente;
    if(pPrecedente != NULL) pPrecedente->prox = p;
    else *l = p;
}

void init(memlist *l,linea *s){
    *l = NULL;
    *s = NULL;
    append(l,1,0);
}



/*
Comandi
----------------------------------------------------------------------------------------------------------------------------------
*/


int checkInd(memlist *l,int ind){
    /*
    Controlla se l'indice è presente nella lista
    */
    if(*l!=NULL && (*l)->indirizzo!=ind) checkInd(&(*l)->prox,ind);
    else if(*l==NULL) return 0;
    else return 1;
}

void store(memlist *l,int ind,int e){
    /*
    se la cella esiste, sostituisce il valore, altrimenti aggiunge la cella
    */
    memlist curr=*l;
    if(checkInd(l,ind)){
        while(curr->indirizzo!=ind) curr=curr->prox;
        curr->contenuto = e;
    }
    else inserisciOrdine(l,ind,e);
}
void read(int* accum){
    printf("\nInput: ");
    scanf("%d",accum);
}

int load(memlist *l,int ind){
    /*
    ritorna il valore della cella dato un indirizzo
    */
    if((*l)->indirizzo==ind) return (*l)->contenuto;
    else if((*l)->prox!=NULL) load(&((*l)->prox),ind);
}

void stampaLista(memlist *l){
    if(*l!=NULL){
        printf("\n[%d] %d", (*l)->indirizzo,(*l)->contenuto);
        stampaLista(&((*l)->prox));
    }
}

void stampaLista2(linea *l){
    if(*l!=NULL){
        printf("\n%d) %s %d", (*l)->linea,(*l)->comando,(*l)->num_comando);
        stampaLista2(&((*l)->prox));
    }
}

void scrivi(int a){
    printf("\nOutput: %d\n",a);
}

void formattaLinea(char lineaFile[],char com[],int* n){
    /*
    Divide lineaFile in comando e numero trasformando numero in int
    */
    int i;
    strcpy(com,"");
    *n=0;
    for(i=0;i<6;i++){
        if(lineaFile[i] == ' ' || lineaFile[i]=='\n') break;
        else com[i]=lineaFile[i];
    }
    com[i]='\0';
    char *num = lineaFile+i;
    *n = atoi(num);
}

void appendLinea(linea *l,int ind,char e[]){
    /*
        Aggiunge un elemento alla lista delle linee dividendo la str del file
        in comando e numero
    */
    linea p;
    char comando[6];
    int n_com=0;
    if(*l==NULL){
        formattaLinea(e,comando,&n_com);
        p = malloc(sizeof(ln));
        p->linea = ind;
        strcpy(p->comando,comando);
        
        p->num_comando = n_com;
        p->prox = NULL;
        *l = p;
    }
    else appendLinea(&((*l)->prox),ind,e);
}

int eseguiComando(int* acc, char comando[],int comm_num){
    /*
    Esegue il comando passato. Se non viene eseguito nessun comando ritorna 0
    */
    int flag=0;

    if(strcmp(comando,"")==0) return 1;
    if(strcmp(comando,"STORE")==0){
        store(&memoria,comm_num,*acc);
        flag=1;
        }
    if(strncmp(comando,"STORE@",6)==0){
        store(&memoria,load(&memoria,comm_num),*acc);
        flag=1;
    }
    if(strcmp(comando,"LOAD")==0){
        *acc = load(&memoria,comm_num);
        flag=1;
    }
    if(strcmp(comando,"LOAD=")==0){
        *acc = comm_num;
        flag=1;
    }
    if(strcmp(comando,"LOAD@")==0){ 
        *acc = load(&memoria,load(&memoria,comm_num));
        flag=1;
    }
    if(strcmp(comando,"WRITE")==0){
        if(comm_num==0) scrivi(*acc);
        else scrivi(comm_num);
        flag=1;
    }
    if(strcmp(comando,"READ")==0){
        if(comm_num==0) read(acc);
        else *acc = comm_num; 
        flag=1;
    }
    if(strcmp(comando,"ADD")==0){
        *acc += load(&memoria,comm_num);
        flag=1;
    }
    if(strcmp(comando,"ADD=")==0){
        *acc += comm_num;
        flag=1;
    }
    if(strcmp(comando,"SUB")==0){
        *acc -= load(&memoria,comm_num);
        flag=1;
    }
    if(strcmp(comando,"SUB=")==0){
        *acc -= comm_num;
        flag=1;
    }
    if(strcmp(comando,"MULT")==0){
        *acc *= load(&memoria,comm_num);
        flag=1;
    
    }
    if(strcmp(comando,"MULT=")==0){
        *acc *= comm_num;
        flag=1;    
    }
    if(strcmp(comando,"DIV")==0){
        if(load(&memoria,comm_num)!=0) *acc /= load(&memoria,comm_num);
        else return 1;
    }
    if(strcmp(comando,"DIV=")==0){
        if(comm_num!=0) *acc /= comm_num;
        else return 1;
    }
    return flag;
}
void branch(char comando[],int comm_num,int* linea,int acc){
    /*
    Esegue i comandi di flusso di esecuzione.
    Incrementa linea di 1 solo se non viene eseguito niente
    */
    if(strcmp(comando,"END")==0) *linea = 0;
    else if(strcmp(comando,"BR")==0) *linea = comm_num;
    else if(strcmp(comando,"BEQ")==0 && acc==0) *linea = comm_num;
    else if(strcmp(comando,"BGE")==0 && acc>=0)*linea = comm_num;
    else if(strcmp(comando,"BG")==0 && acc>0)*linea = comm_num;
    else if(strcmp(comando,"BLE")==0 && acc<=0)*linea = comm_num;
    else if(strcmp(comando,"BL")==0 && acc<0)*linea = comm_num;
    else *linea +=1;
}

/*
Gestione file
----------------------------------------------------------------------------------------------------------------------------------------------
*/

void convertiTesto(FILE *f,linea *l){
    int i=1;
    char stringa[128];
    while(fgets(stringa,sizeof(stringa),f)!=NULL){
        appendLinea(l,i,stringa);
        i++;
        if(strncmp(stringa,"END",3)==0) break;
    }
}


int main(int argc,char** argv){
    FILE *fp;
    if(argc==1) fp = fopen(FILENAME,"r");
    else fp = fopen(argv[1],"r");
    rewind(fp);
    fflush(fp);
    char nomeComando[7];
    int accumulatore=0,num_linea=1;
    init(&memoria,&listaLinee);
    convertiTesto(fp,&listaLinee);
    fclose(fp);
    linea cursore = listaLinee;
    while(num_linea!=0){
        while(cursore->linea!=num_linea){
            cursore = cursore->prox;
        }
        
        if(!eseguiComando(&accumulatore,cursore->comando,cursore->num_comando)){
            branch(cursore->comando,cursore->num_comando,&num_linea,accumulatore);
            cursore = listaLinee;
        }
        else num_linea++;
    }
    system("pause");
}
