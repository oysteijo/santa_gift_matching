#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "tinymt32.h"	

tinymt32_t rnd;

#define MAX_LINE 256

#define n_children 1000000
#define n_child_pref 1000
#define n_gift_type 1000

uint16_t wishlist[n_children][10] = {
#include "child_wishlist.h"
};

int32_t childlist[1000][n_child_pref] = {
#include "gift_goodkids.h"
};

static float avg_happiness(uint16_t *giftlist)
{
	int total_child_happiness = 0;
	int total_gift_happiness[n_gift_type] = {0};

	for( int child_id = 0; child_id <  n_children; child_id++ ){
		const int gift_id = giftlist[child_id];

		int child_happiness = -1;
		for (int rank=0 ; rank < 10 ; rank++)
			if( wishlist[child_id][rank] == gift_id ){
				child_happiness = (10-rank) * 2;
				break;
			}

		int gift_happiness = -1;
		for (int rank=0 ; rank < 1000 ; rank++)
			if( childlist[gift_id][rank] == child_id){
				gift_happiness = (1000-rank) * 2; 
				break;
			}

		total_child_happiness += child_happiness;
		total_gift_happiness[gift_id] += gift_happiness;
	}

	int sum = 0;
	for( int gift_id = 0; gift_id < n_gift_type; gift_id++ )
		sum += total_gift_happiness[gift_id];

	float normalized_gift_happiness = ((float)sum/(float)n_gift_type) / ((float) n_child_pref * 2.0f * 1000.0f);
	float normalized_child_happiness = (float) total_child_happiness / ((float)n_children * 10.0f * 2.0f);
	
	printf("normalized_child_happiness : %g\nnormalized_gift_happiness  : %g\n", normalized_child_happiness,normalized_gift_happiness);
	return normalized_child_happiness + normalized_gift_happiness;
}

void get_swap_idx( int idx[2] )
{
	idx[0] = (tinymt32_generate_uint32( &rnd ) % (1000000 - 4000)) + 4000;
	idx[1] = (tinymt32_generate_uint32( &rnd ) % (1000000 - 4000)) + 4000;
}

int main( int argc, char *argv[] )
{
    if(argc != 2){
		fprintf( stderr, "Usage: %s <filename>\n", argv[0]);
		return 0;
	}
	/* Open file */
    FILE *fp;
    if( !(fp =fopen(argv[1], "r") )){
        perror(argv[1]);
        return -1;
    }
	char buffer[MAX_LINE];
	uint16_t *predval = malloc(n_children * sizeof(uint16_t));
	if(!predval) return -1;

	int counter = 0;
	while ( fgets( buffer, sizeof buffer, fp ) ){
        if(!strncmp("ChildId", buffer, 7)) continue; 
		uint16_t cid;
		if (2 != sscanf(buffer, "%hu,%hu", &cid, &predval[counter]) ){
			printf("Malformed string at line %d ", counter);
			printf("'%s'", buffer);
		}
        counter++;
	}
	fclose(fp);

	tinymt32_init( &rnd, 42 );
	float prev_score = avg_happiness( predval );


	for( int i = 0; i < 1000; i++ ){
		int idx[2];
		get_swap_idx( idx );
		
		uint16_t tmp = predval[idx[0]];
		predval[idx[0]] = predval[idx[1]];
		predval[idx[1]] = tmp;

		float score =  avg_happiness( predval );
		if( score > prev_score) 
			continue;

		tmp = predval[idx[0]];
		predval[idx[0]] = predval[idx[1]];
		predval[idx[1]] = tmp;
		
	}

	printf("Score of file '%s': %f\n", argv[1], avg_happiness(predval));
	free(predval);
	return 0;
}
