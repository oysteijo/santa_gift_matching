#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <glpk.h>
#include <assert.h>

/*
   Maximize
obj: x1 + 2 x2 + 3 x3 + x4
Subject To
c1: - x1 + x2 + x3 + 10 x4 <= 20
c2: x1 - 3 x2 + x3 <= 30
c3: x2 - 3.5 x4 = 0
Bounds
0 <= x1 <= 40
2 <= x4 <= 3
General
x4
End
*/

#define n_gift_type 1000
#define n_children  1000000
#define n_child_pref 1000
#define n_wish_pr_child 10
/* I've counted in advance */
#define num_variables 10990072

const uint16_t wishlist[n_children][n_wish_pr_child] = {
#include "child_wishlist.h"
};

const int32_t childlist[n_gift_type][n_child_pref] = {
#include "gift_goodkids.h"
};

#define NO_HAPPINESS -101

static int calc_happiness( int child_id, int gift_id )
{
	int child_happiness = -1;
	for (int rank=0 ; rank < n_wish_pr_child ; rank++)
		if( wishlist[child_id][rank] == gift_id ){
			child_happiness = (n_wish_pr_child-rank) * 2;
			break;
		}

	int gift_happiness = -1;
	for (int rank=0 ; rank < n_child_pref ; rank++)
		if( childlist[gift_id][rank] == child_id){
			gift_happiness = (n_child_pref-rank) * 2; 
			break;
		}

	return child_happiness*100 + gift_happiness;
}


int main(void)
{
	glp_prob *mip = glp_create_prob();
	glp_set_prob_name(mip, "Santa's Gift Matching Problem.");
	glp_set_obj_dir(mip, GLP_MAX);


	glp_add_rows(mip, n_gift_type + n_children);
	glp_add_cols(mip, num_variables);


	/* Allocate sparse matrix memory */
	int    *ia = malloc( (2 * num_variables + 1) * sizeof(int));
	int    *ja = malloc( (2 * num_variables + 1) * sizeof(int));
	double *ar = malloc( (2 * num_variables + 1) * sizeof(double)); 

	assert( ia );
	assert( ja );
	assert( ar );

	int constraint_col = 0;
	for( int gift_id = 0; gift_id < n_gift_type; gift_id++ ){

		/* BTW: This 'counter' is not general */
		if( !((gift_id+1) % 10) ){
			printf("Setting up problem %3d%% completed\r", (gift_id+1) / 10);
			fflush(stdout);
		}

		glp_set_row_bnds( mip, gift_id + 1, GLP_DB, 0, 1000 ); 
		for( int child_id = 0; child_id < n_children; child_id++ ){

			int happiness = calc_happiness( child_id, gift_id );
			if ( happiness == NO_HAPPINESS ) 
				continue;

			double val = happiness / 2000.0;
			constraint_col++;

			char varname[30];
			sprintf( varname, "%d,%d", child_id, gift_id);
			glp_set_col_name(mip, constraint_col, varname);
			glp_set_col_kind(mip, constraint_col, GLP_BV );
			glp_set_obj_coef(mip, constraint_col, val);

			ia[constraint_col] = gift_id + 1;
			ja[constraint_col] = constraint_col; 
			ar[constraint_col] = 1.0;

			ia[num_variables + constraint_col] = child_id + n_gift_type + 1;
			ja[num_variables + constraint_col] = constraint_col; 
			ar[num_variables + constraint_col] = 1.0; 
		}
	}
	printf("\n"); fflush(stdout); /* When printing \r to stdout you should add something like this. */

	/* Only one gift pr. child */
	for( int child_id = 0; child_id < n_children; child_id++ )
		glp_set_row_bnds( mip, child_id + n_gift_type + 1, GLP_DB, 0, 1 ); 

	printf("Non trivial variables: %d\n", constraint_col); 
	/* Outputs: Non trivial variables: 10990072 */

	/* Solve */
	printf("Loading matrix.\n");
	glp_load_matrix(mip, constraint_col, ia, ja, ar);
	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	printf("Solving. (Please wait)\n");

	int err = glp_intopt(mip, &parm);
	if(err){
		printf("Failed with error: %d\n", err);
		return 0;
	}

	double z = glp_mip_obj_val(mip);
	printf("Solution found! Objective value (not the same as score): %g\n", z);


	FILE *fp = fopen("raw_solution.txt", "w");
	assert(fp);

	for( int i = 1; i <= num_variables ; i++ ){
		double val = glp_mip_col_val(mip, i);
		if ( val > 0.9 ) {
			fprintf(fp, "%s\n", glp_get_col_name(mip, i));
		}
	}
	fclose(fp);

	glp_delete_prob(mip);

	return 0;
}
