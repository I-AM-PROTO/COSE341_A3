#include <stdio.h>
#include <stdlib.h>

int *generate_ref_arr(size_t sz, size_t page_max);
int lru(int *ref_arr, size_t ref_arr_sz, size_t frame_sz);

typedef struct Node {
	int data;
	struct Node* next;
	struct Node* prev;
}Node;

typedef struct Stack {
	int size, max_size;
	Node* top;
}Stack;

Node* create_node(){
	Node* ret = (Node*)malloc(sizeof(Node));
	ret->data = -1;
	ret->next = NULL;
	ret->prev = NULL;
	return ret;
}

Stack* create_stack(int max_size){
	Stack* ret = (Stack*)malloc(sizeof(Stack));
	ret->size = 0;
	ret->max_size = max_size;
	ret->top = NULL;
	return ret;
} 

void insert(Stack* st, int data){
	Node* newNode = create_node();
	newNode->data = data;
	if(st->top != NULL){
		newNode->next = st->top;
		st->top->prev = newNode;
	}
	st->top = newNode;
	st->size++;
}

void delete(Stack* st, Node* delNode){
	if(delNode->next != NULL) delNode->next->prev = delNode->prev;
	if(delNode->prev != NULL) delNode->prev->next = delNode->next;
	if(delNode == st->top) st->top = delNode->next;
	st->size--;
	free(delNode);
}

// returns whether faulted or not
// return page to be replaced by rep
int simulate(Stack* st, int pg, int *rep){	
	*rep = -1;
	Node *N = st->top;

	//fprintf(stderr, "%p %p %d", st, N, pg);	
	// zero pages(page fault, no replace)
	if(N==NULL){
		insert(st, pg);
		return 1;
	}
	
	// find node
	while(N->next!=NULL && N->data!=pg)
		N = N->next;
	
	// found page
	if(N->data==pg){
		delete(st, N);
		insert(st, pg);
		return 0;
	}
	// page fault
	else{
		if(st->size == st->max_size){
			*rep = N->data;
			delete(st, N);
		}
		insert(st, pg);
		return 1;
	}
}

void log_stack(int pg, Stack* st, int faulted){
	printf("%d | ", pg);
	
	Node* N = st->top;
	for(int i=0; i<st->size; i++){
		printf("%d ", N->data);
		N = N->next;
	}
	for(int i=0; i<st->max_size-st->size; i++){
		printf(". ");
	}

	if(faulted) printf("(fault)");

	printf("\n");
}

void log_frame(int pg, int* frames, int frame_size, int faulted){
	printf("%d | ", pg);

	for(int i=0; i<frame_size; i++)
		if(frames[i]!=-1) printf("%d ", frames[i]);
		else printf(". ");

	if(faulted) printf("(fault)");

	printf("\n");
}

void log_full(int pg, int* frames, int frame_size, Stack* st, int faulted){
	printf("%d\t| ", pg);

	for(int i=0; i<frame_size; i++)
		if(frames[i]!=-1) printf("%d ", frames[i]);
		else printf(". ");
	
	if(faulted) printf("(fault)");
	else printf("\t");
	printf("\t| ");

	Node* N = st->top;
	
	for(int i=0; i<st->size; i++){
		printf("%d ", N->data);
		N = N->next;
	}
	for(int i=0; i<st->max_size-st->size; i++){
		printf(". ");
	}

	printf("\n");
}

int main (int argc, char** argv){
	if (argc<4) return -1;

	int ref_arr_sz = atoi(argv[1]);
	int page_max = atoi(argv[2]);
	int frame_sz = atoi(argv[3]);
	
	int* ref = generate_ref_arr(ref_arr_sz, page_max);
	printf("Page \t| Frames \t\t| Stack\n");
	printf("-----------------------------------------\n");
	int page_faults = lru(ref, ref_arr_sz, frame_sz);
	printf("%d\n", page_faults);
	free(ref);

	return 0;
}

int *generate_ref_arr(size_t sz, size_t page_max){
	int *ref_arr = (int*)malloc(sizeof(int)*sz);
        
	for(int i=0; i<sz; i++)
		ref_arr[i] = rand()%page_max;
	
	return ref_arr;
}

int lru(int *ref_arr, size_t ref_arr_sz, size_t frame_sz){
	int i,j;
	int page_faults = 0;

	Stack* st = create_stack(frame_sz);
	int* frames = (int*)malloc(sizeof(int)*frame_sz);
	for(int i=0; i<frame_sz; i++) frames[i] = -1;

	for(i=0; i<ref_arr_sz; i++){
		int rep;
		int faulted = simulate(st, ref_arr[i], &rep); 
		if(faulted)
			for(int j=0; j<frame_sz; j++)
				if(frames[j] == rep){
					frames[j] = ref_arr[i];
					break;
				}
		log_full(ref_arr[i], frames, frame_sz, st, faulted);
		page_faults += faulted;
	}

	// memory leak is my passion	

	return page_faults;
}
	
