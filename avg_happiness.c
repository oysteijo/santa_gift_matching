#include "avg_happiness.h"

const uint16_t wishlist[n_children][10] = {
#include "data/child_wishlist.h"
};

const int32_t childlist[1000][n_child_pref] = {
#include "data/gift_goodkids.h"
};

float avg_happiness(uint16_t *giftlist)
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
	
/*	printf("normalized_child_happiness : %g\nnormalized_gift_happiness  : %g\n", normalized_child_happiness,normalized_gift_happiness); */
	return normalized_child_happiness + normalized_gift_happiness;
}

