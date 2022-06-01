#include <stdio.h>
#include <stdlib.h>

int *generate_ref_arr(size_t sz, size_t page_max);
int lru(int *ref_arr, size_t ref_arr_sz, size_t frame_sz);

int main (int argc, char** argv){
	if (argc<4) return -1;

	int ref_arr_sz = atoi(argv[1]);
	int page_max = atoi(argv[2]);
	int frame_sz = atoi(argv[3]);

	int* ref = generate_ref_arr(ref_arr_sz, page_max);
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
     
void log_frame(int pg, int* frames, int frame_size, int faulted){
	printf("%d | ", pg);

	for(int i=0; i<frame_size; i++)
		if(frames[i]!=-1) printf("%d ", frames[i]);
		else printf(". ");

	if(faulted) printf("(fault)");

	printf("\n");
}

int lru(int *ref_arr, size_t ref_arr_sz, size_t frame_sz){
	int i,j,hand = 0;
	int page_faults = 0;

	int *frames = (int*)malloc(sizeof(int)*frame_sz);
	int *ref = (int*)malloc(sizeof(int)*frame_sz);
	for(i=0; i<frame_sz; i++) {frames[i]=-1; ref[i]=0;}

	for(i=0; i<ref_arr_sz; i++){
		int found=0;
		for(j=0; j<frame_sz; j++)
			if(frames[j] == ref_arr[i]){
				ref[j]=1;
				found=1;
				break;
			}
		
		// page found
		if(found) {
			log_frame(ref_arr[i], frames, frame_sz, 0);
			continue;
		}

		// find victim
		while(ref[hand]){
			ref[hand] = 0;
			hand = (hand+1)%frame_sz;
		}

		frames[hand]=ref_arr[i];
		ref[hand]=1;
		log_frame(ref_arr[i], frames, frame_sz, 1);
		page_faults++;
	}

	
	return page_faults;
}
	
