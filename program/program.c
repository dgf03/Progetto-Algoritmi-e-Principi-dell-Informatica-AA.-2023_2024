#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXS 110
#define C_MAX 16
#define M_ING 150
#define M_RIC 1000
#define C_Q 100
#define C_RO 5000

/*
STRUTTURE DATI DA IMPLEMENTARE:
- magazzino: hash
- menu: hash
- elementi della ricetta: RBT, ordinati in base alla quantità richiesta
- ordini: lista per attesa e pronto, RBT per print
- scorta: minheap
*/

typedef struct{
	int q;
	int scadenza;
}scorta_t;

typedef struct {
	scorta_t *arr;
	int size;
	int capacity;
}heap_scorta_t;

typedef struct ingrediente_{
	char *nome;
	int nlen;
	int peso;
	heap_scorta_t *scorta;
	
	struct ingrediente_ *next;
}ingrediente_t;

typedef struct ing_info_{
	ingrediente_t *info;
	int q_ric;
	
	int color;
	struct ing_info_ *next;
}ing_info_t;

typedef struct ricetta_{
	char *nome;
	int nlen;
	int peso;
	int ordini;
	ing_info_t *lista;
	
	struct ricetta_ *prev;
	struct ricetta_ *next;
}ricetta_t;

typedef struct{
	ricetta_t *info;
	int time;
	int n;
}ordine_t;

typedef struct list_ordine_{
	ordine_t ordine;
	
	struct list_ordine_ *next;
}list_ordine_t;

typedef struct{
	ordine_t *arr;
	int capacity;
	int size;
}heap_ordine_t;

void aggiungi_ricetta(ricetta_t**, ingrediente_t**, char*, int, int, int); //T(n)=O((1+alfa)n) - caso medio
void rimuovi_ricetta(ricetta_t**, char*, int); //T(n)=O((1+alfa)n) - caso medio
void rifornimento(ingrediente_t**, list_ordine_t**, heap_ordine_t*, list_ordine_t**, char*, int, int, int); //T(n)=O((1+alfa)n) - caso medio
void ordine(list_ordine_t**, heap_ordine_t*, list_ordine_t**, ricetta_t**, int, int, char*);

void insertRecipe(ricetta_t**, ricetta_t*, int); //T(n)=O(1)
ricetta_t* searchRecipe(ricetta_t**, char*, int, int); //T(n)=O((1+alfa)n) - caso medio
void removeRecipe(ricetta_t**, ricetta_t*, int); //T(n)=O(1)

void insertIngredient(ingrediente_t**, ingrediente_t*, int); //T(n)=O(1)
ingrediente_t* searchIngredient(ingrediente_t**, char*, int, int); //T(n)=O((1+alfa)n) - caso medio

void insertIngredienttoRecipe(ing_info_t**, ing_info_t*); //T(n)=O(log(n))
void removeallIngredientsfromRecipe(ing_info_t *lista);

heap_scorta_t *createHeapQuantity(int); //T(n)=O(1)
void insertHelperQuantity(heap_scorta_t*, int); //T(n)=O(log(n))
void heapifyQuantity(heap_scorta_t*, int); //T(n)=O(log(n))
void insertQuantity(heap_scorta_t*, int, int); //T(n)=O(log(n))
int searchQuantity(heap_scorta_t*, int, int); //T(n)=O(log(n))

list_ordine_t *insertWaitOrder(list_ordine_t **lista, list_ordine_t*, list_ordine_t*); //T(n)=O(1)
void removeQuantities(ing_info_t*, int); //T(n)=O(nlog^2(n))
void removeExpiredProducts(ingrediente_t**, int, int); //T(n)=O(n)
int PresenceofIngredients(ing_info_t*, int); //T(n)=O(log(n))

heap_ordine_t* createHeapOrder(int); //T(n)=O(1)
void insertHelperRO(heap_ordine_t*, int);
void extractRO(heap_ordine_t*);
void insertRO(heap_ordine_t*, int, ricetta_t*, int); //T(n)=O(1)
void moveOrder(list_ordine_t **attesa, heap_ordine_t *pronto, list_ordine_t **la);

void printOrder(heap_ordine_t*, int, int);
void insertHelperPrint(heap_ordine_t*, int);
void heapifyPrint(heap_ordine_t*, int);
void insertPrint(heap_ordine_t*, int, ricetta_t*, int);
void extractPrint(heap_ordine_t*);

int g(char*, int, int); //T(n)=O(1)
int convertChartoID(char); //T(n)=O(1)

void extractStr(char*, char*, int*); //T(n)=O(n)
int writeInt(char*, char*); //T(n)=O(n)
int equalString(char*, char*, int, int); //T(n)=O(log(n))

void initialiseMenu(ricetta_t**, int); //T(n)=O(log(n))
void initialiseMagazzino(ingrediente_t**, int); //T(n)=O(log(n))

int main(){
	ricetta_t *menu[M_RIC];
	ingrediente_t *magazzino[M_ING];
	list_ordine_t *attesa, *last_a;
	heap_ordine_t *pronto;
	char command[C_MAX+1], c;
	int length, time, passaggio, capacity;
	
	passaggio = writeInt(command, &c);
	capacity = writeInt(command, &c);
	
	initialiseMenu(menu, M_RIC);
	initialiseMagazzino(magazzino, M_ING);
	attesa = last_a = NULL;
	pronto = createHeapOrder(C_RO);
	
	time = 0;
	extractStr(command, &c, &length);
	while(length){
		if(command[0]=='a')
			aggiungi_ricetta(menu, magazzino, &c, M_RIC, M_ING, C_Q);
		else if(command[2]=='m')
			rimuovi_ricetta(menu, &c, M_RIC);
		else if(command[2]=='f')
			rifornimento(magazzino, &attesa, pronto, &last_a, &c, time, M_ING, C_Q);
		else if(command[0]=='o')
			ordine(&attesa, pronto, &last_a, menu, time, M_RIC, &c);
		
		while(c!='\n' && scanf("%c", &c));
		
		time++;
		if(time%passaggio == 0)
			printOrder(pronto, capacity, C_RO);
		
		removeExpiredProducts(magazzino, time, M_ING);
		extractStr(command, &c, &length);
	}
	return 0;
}

void aggiungi_ricetta(ricetta_t **menu, ingrediente_t **magazzino, char *c, int dim_ric, int dim_ing, int capacity){
	ricetta_t *ric;
	ingrediente_t *ing;
	ing_info_t *in;
	char str[MAXS+1];
	int i, j, length;
	
	extractStr(str, c, &length);
	i = g(str, dim_ric, length);
	if(!searchRecipe(menu, str, i, length)){
		ric = malloc(sizeof(ricetta_t));
		
		ric->nome = strdup(str);
		ric->nlen = length;
		ric->peso = 0;
		ric->ordini = 0;
		
		ric->lista = NULL;
		
		ric->prev = NULL;
		ric->next = NULL;
		
		do{
			extractStr(str, c, &length);
			j = g(str, dim_ing, length);
			ing = searchIngredient(magazzino, str, j, length);
			if(!ing){
				ing = malloc(sizeof(ingrediente_t));
				ing->nome = strdup(str);
				ing->nlen = length;
				ing->peso = 0;
				ing->scorta = createHeapQuantity(capacity);
				ing->next = NULL;
				
				insertIngredient(magazzino, ing, j);
			}
			in = malloc(sizeof(ing_info_t));
			in->info = ing;
			in->q_ric = writeInt(str, c);
			
			insertIngredienttoRecipe(&(ric->lista), in);
			
			ric->peso += in->q_ric;
		}while(*c!='\n');
		
		insertRecipe(menu, ric, i);
		printf("aggiunta\n");
	}
	else
		printf("ignorato\n");
}

void rimuovi_ricetta(ricetta_t **menu, char *c, int dim_ric){
	ricetta_t *cancel;
	char str[MAXS+1];
	int j, length;
	
	extractStr(str, c, &length);
	j = g(str, dim_ric, length);
	cancel = searchRecipe(menu, str, j, length);
	if(cancel && cancel->ordini==0){
		removeallIngredientsfromRecipe(cancel->lista);
		free(cancel->nome);
		
		removeRecipe(menu, cancel, j);
		free(cancel);
		printf("rimossa\n");
	}
	else if(!cancel)
		printf("non presente\n");
	else printf("ordini in sospeso\n");
}

void rifornimento(ingrediente_t **magazzino, list_ordine_t **attesa, heap_ordine_t *pronto, list_ordine_t **la, char *c, int time, int dim_ing, int capacity){
	ingrediente_t *ing;
	char str[MAXS+1];
	int j, q, s, length;
	do{
		extractStr(str, c, &length);
		j = g(str, dim_ing, length);
		ing = searchIngredient(magazzino, str, j, length);
		if(!ing){
			ing = malloc(sizeof(ingrediente_t));
			ing->nome = strdup(str);
			ing->nlen = length;
			ing->peso = 0;
			ing->scorta = createHeapQuantity(capacity);
			ing->next = NULL;
			
			insertIngredient(magazzino, ing, j);
		}
		
		q = writeInt(str, c);
		s = writeInt(str, c);
		if(s > time){
			if(!searchQuantity(ing->scorta, q, s))
				insertQuantity(ing->scorta, q, s);
			
			ing->peso += q;
		}
	}while(*c!='\n');
	
	moveOrder(attesa, pronto, la);
	
	printf("rifornito\n");
}

void ordine(list_ordine_t **attesa, heap_ordine_t *pronto, list_ordine_t **la, ricetta_t **menu, int time, int dim_ric, char *c){
	list_ordine_t *o;
	ricetta_t *ric;
	char str[MAXS+1];
	int j, n, length;
	
	extractStr(str, c, &length);
	j = g(str, dim_ric, length);
	ric = searchRecipe(menu, str, j, length);
	if(ric){
		(ric->ordini)++;
		n = writeInt(str, c);
		if(PresenceofIngredients(ric->lista, n)){
			insertRO(pronto, time, ric, n);
			removeQuantities(ric->lista, n);
		}
		else{
			o = malloc(sizeof(list_ordine_t));
			o->next = NULL;
			o->ordine.time = time;
			o->ordine.info = ric;
			o->ordine.n = n;
			*la = insertWaitOrder(attesa, *la, o);
		}
		printf("accettato\n");
	}
	else
		printf("rifiutato\n");
}

void insertRecipe(ricetta_t **t, ricetta_t *x, int j){
	x->next = t[j];
	if(t[j])
		t[j]->prev = x;
	t[j] = x;
	x->prev = NULL;
}

ricetta_t* searchRecipe(ricetta_t **t, char *str, int j, int len){
	ricetta_t *x;
	
	x=t[j];
	while(x && equalString(x->nome, str, x->nlen, len))
		x = x->next;
	return x;
}

void removeRecipe(ricetta_t **t, ricetta_t *x, int j){
	if(x->prev)
		x->prev->next = x->next;
	else t[j] = x->next;
	if(x->next)
		x->next->prev = x->prev;
}

void insertIngredient(ingrediente_t **t, ingrediente_t *x, int j){
	x->next = t[j];
	t[j] = x;
}

ingrediente_t* searchIngredient(ingrediente_t **t, char *str, int j, int len){
	ingrediente_t *x;
	x=t[j];
	while(x && equalString(x->nome, str, x->nlen, len))
		x = x->next;
	return x;
}

void insertIngredienttoRecipe(ing_info_t **lista, ing_info_t *x){
	x->next = *lista;
	*lista = x;
}

void removeallIngredientsfromRecipe(ing_info_t *lista){
	ing_info_t *tmp;
	
	while(lista){
		tmp = lista;
		lista = lista->next;
		free(tmp);
	}
}

heap_scorta_t *createHeapQuantity(int capacity){
	heap_scorta_t *h;
	
	h = (heap_scorta_t*) malloc(sizeof(heap_scorta_t));
	
	h->arr = (scorta_t*) malloc(sizeof(scorta_t)*capacity);
	h->size = 0;
	h->capacity = capacity;
	
	return h;
}

void insertHelperQuantity(heap_scorta_t *h, int i){
	int p;
	scorta_t tmp;
	
	p = (i-1)/2;
	if(h->arr[p].scadenza > h->arr[i].scadenza){
		tmp = h->arr[p];
		h->arr[p] = h->arr[i];
		h->arr[i] = tmp;
		insertHelperQuantity(h, p);
	}
}

void heapifyQuantity(heap_scorta_t *h, int i){
	int left, right, min;
	scorta_t tmp;
	
	left = i*2+1;
	right = i*2+2;
	
	if(left >= h->size || left < 0)
		left = -1;
	if (right >= h->size || right < 0)
		right = -1;
	
	if(left!=-1 && h->arr[left].scadenza < h->arr[i].scadenza)
		min = left;
	else min = i;
	if(right!=-1 && h->arr[right].scadenza < h->arr[min].scadenza)
		min = right;
	
	if(min!=i){
		tmp = h->arr[min];
		h->arr[min] = h->arr[i];
		h->arr[i] = tmp;
		
		heapifyQuantity(h, min);
	}
}

void extractQuantity(heap_scorta_t *h){
	if(h->size>0){
		//supponiamo che size>0
		h->arr[0]=h->arr[h->size-1];
		(h->size)--;
		
		heapifyQuantity(h, 0);
	}
}

void insertQuantity(heap_scorta_t *h, int q, int s){
	if(h->size < h->capacity){
		h->arr[h->size].scadenza = s;
		h->arr[h->size].q = q;
		insertHelperQuantity(h, h->size);
		(h->size)++;
	}
}

int searchQuantity(heap_scorta_t *h, int q, int time){
	int i, ok;
	
	for(i=0, ok = 0; !ok && i<h->size; i++)
		if(h->arr[i].scadenza == time){
			h->arr[i].q += q;
			ok = 1;
		}
	return ok;
}

list_ordine_t *insertWaitOrder(list_ordine_t **lista, list_ordine_t *last, list_ordine_t *x){
	if(!(*lista))
		*lista = x;
	else last->next = x;
	return x;
}

void removeQuantities(ing_info_t *t, int n){
	int q;
	ing_info_t *curr;
	
	for(curr = t; curr; curr = curr->next){
		q = n * curr->q_ric;
		curr->info->peso -=q;
		while(q>0){
			if(curr->info->scorta->arr[0].q <= q){
				q -= curr->info->scorta->arr[0].q;
				extractQuantity(curr->info->scorta);
			}
			else{
				curr->info->scorta->arr[0].q -= q;
				q = 0;
			}
		}
	}
}

void removeExpiredProducts(ingrediente_t **magazzino, int time, int dim){
	ingrediente_t *curr;
	int i;
	
	for(i=0; i<dim; i++)
		for(curr = magazzino[i]; curr; curr = curr->next)
			if(curr->scorta->size>0 && curr->scorta->arr[0].scadenza == time){
				curr->peso -= curr->scorta->arr[0].q;
				extractQuantity(curr->scorta);
			}
}

int PresenceofIngredients(ing_info_t *t, int n){
	ing_info_t *curr;
	int ok;
	
	for(ok = 1, curr = t; ok && curr; curr = curr->next)
		if(ok && n*curr->q_ric > curr->info->peso)
			ok = 0;
	return ok;
}

/*HEAP PER READY E PRINT ORDERS - da rivedere*/
heap_ordine_t* createHeapOrder(int capacity){
	heap_ordine_t *h;
	
	h = (heap_ordine_t*) malloc(sizeof(heap_ordine_t));
	
	h->arr = (ordine_t*) malloc(sizeof(ordine_t)*capacity);
	h->size = 0;
	h->capacity = capacity;
	
	return h;
}

void insertHelperRO(heap_ordine_t *h, int i){
	int p;
	ordine_t tmp;
	
	p = (i-1)/2;
	if(h->arr[p].time > h->arr[i].time){
		tmp = h->arr[p];
		h->arr[p] = h->arr[i];
		h->arr[i] = tmp;
		insertHelperRO(h, p);
	}
}

void heapifyRO(heap_ordine_t *h, int i){
	int left, right, min;
	ordine_t tmp;
	
	left = i*2+1;
	right = i*2+2;
	
	if(left >= h->size || left < 0)
		left = -1;
	if (right >= h->size || right < 0)
		right = -1;
	
	if(left!=-1 && h->arr[left].time < h->arr[i].time)
		min = left;
	else min = i;
	if(right!=-1 && h->arr[right].time < h->arr[min].time)
		min = right;
	
	if(min!=i){
		tmp = h->arr[min];
		h->arr[min] = h->arr[i];
		h->arr[i] = tmp;
		
		heapifyRO(h, min);
	}
}

void extractRO(heap_ordine_t *h){
	if(h->size>0){
		//supponiamo che size>0
		h->arr[0]=h->arr[h->size-1];
		(h->size)--;
		
		heapifyRO(h, 0);
	}
}

void insertRO(heap_ordine_t *h, int time, ricetta_t *ric, int n){
	if(h->size < h->capacity){
		h->arr[h->size].info = ric;
		h->arr[h->size].time = time;
		h->arr[h->size].n = n;
		insertHelperRO(h, h->size);
		(h->size)++;
	}
}

void moveOrder(list_ordine_t **attesa, heap_ordine_t *pronto, list_ordine_t **la){
	list_ordine_t *prec, *curr, *tmp;
	
	prec = NULL;
	curr = *attesa;
	while(curr){
		if(PresenceofIngredients(curr->ordine.info->lista, curr->ordine.n)){
			removeQuantities(curr->ordine.info->lista, curr->ordine.n);
			tmp = curr;
			curr = curr->next;
			
			if(!prec)
				*attesa = curr;
			else
				prec->next = curr;
			if(*la == tmp)
				*la = prec;
			
			insertRO(pronto, tmp->ordine.time, tmp->ordine.info, tmp->ordine.n); //T(n)=O(log(n))
			
			free(tmp);
		}
		else{
			prec = curr;
			curr = curr->next;
		}
	}	
}

void printOrder(heap_ordine_t *pronto, int capacity, int dim){
	heap_ordine_t *print;
	
	print = createHeapOrder(dim);
	
	while(capacity>0 && pronto->size>0 && pronto->arr[0].n * pronto->arr[0].info->peso <= capacity){
		capacity -= pronto->arr[0].n * pronto->arr[0].info->peso;
		insertPrint(print, pronto->arr[0].time, pronto->arr[0].info, pronto->arr[0].n);
		extractRO(pronto);
	}
	
	if(!print->size)
		printf("camioncino vuoto\n");
	else while(print->size>0){
		printf("%d %s %d\n", print->arr[0].time, print->arr[0].info->nome, print->arr[0].n);
		(print->arr[0].info->ordini)--;
		extractPrint(print);
	}
}

void insertHelperPrint(heap_ordine_t *h, int i){
	int p;
	ordine_t tmp;
	
	p = (i-1)/2;
	if(h->arr[p].n * h->arr[p].info->peso < h->arr[i].n * h->arr[i].info->peso || (h->arr[p].n * h->arr[p].info->peso == h->arr[i].n * h->arr[i].info->peso && h->arr[p].time > h->arr[i].time)){
		tmp = h->arr[p];
		h->arr[p] = h->arr[i];
		h->arr[i] = tmp;
		insertHelperPrint(h, p);
	}
}

void heapifyPrint(heap_ordine_t *h, int i){
	int left, right, max;
	ordine_t tmp;
	
	left = i*2+1;
	right = i*2+2;
	
	if(left >= h->size || left < 0)
		left = -1;
	if (right >= h->size || right < 0)
		right = -1;
	
	if(left!=-1 && (h->arr[left].n * h->arr[left].info->peso > h->arr[i].n * h->arr[i].info->peso || (h->arr[left].n * h->arr[left].info->peso == h->arr[i].n * h->arr[i].info->peso && h->arr[left].time < h->arr[i].time)))
		max = left;
	else max = i;
	if(right!=-1 && (h->arr[right].n * h->arr[right].info->peso > h->arr[max].n * h->arr[max].info->peso ||(h->arr[right].n * h->arr[right].info->peso == h->arr[max].n * h->arr[max].info->peso && h->arr[right].time < h->arr[max].time)))
		max = right;
	
	if(max!=i){
		tmp = h->arr[max];
		h->arr[max] = h->arr[i];
		h->arr[i] = tmp;
		
		heapifyPrint(h, max);
	}
}

void insertPrint(heap_ordine_t *h, int time, ricetta_t *ric, int n){
	if(h->size < h->capacity){
		h->arr[h->size].info = ric;
		h->arr[h->size].time = time;
		h->arr[h->size].n = n;
		insertHelperPrint(h, h->size);
		(h->size)++;
	}
}

void extractPrint(heap_ordine_t *h){
	if(h->size>0){
		//supponiamo che size>0
		h->arr[0]=h->arr[h->size-1];
		(h->size)--;
		
		heapifyPrint(h, 0);
	}
}

int g(char *str, int m, int length){
	int k, i;
	for(i=0, k=0; i<length; i++)
		k += convertChartoID(str[i]);
	return k%m;
}

int convertChartoID(char c){
	if(c>='0' && c<='9')
		return c-'0';
	else if(c>='A' && c>='Z')
		return ('9'-'0'+1)+c-'A';
	else if(c>='a' && c>='z')
		return ('9'-'0'+1)+('Z'-'A'+1)+c-'a';
	return ('9'-'0'+1)+('Z'-'A'+1)+('z'-'a'+1);
}

void extractStr(char* str, char *c, int *len){
	for(*len=0; scanf("%c", c) && (*c!=' ' && *c!='\n'); (*len)++)
		str[*len] = *c;
	str[*len]='\0';
}

int writeInt(char *str, char *c){
	int len;
	
	extractStr(str, c, &len);
	return atoi(str);
}

int equalString(char *str1, char *str2, int len1, int len2){
	int i, ok;
	if(len1!=len2)
		return 1;
	
	for(i=0, ok = 0; !ok && i<len1; i++)
		if(str1[i]!=str2[i])
			ok = 1;
	return ok;
}

void initialiseMenu(ricetta_t **menu, int m){
	int i;
	
	for(i=0; i<m; i++)
		menu[i]=NULL;
}

void initialiseMagazzino(ingrediente_t **magazzino, int m){
	int i;
	for(i=0; i<m; i++)
		magazzino[i] = NULL;
}