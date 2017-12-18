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

/* I've counted in advance */
#define num_variables 10990072

const uint16_t wishlist[n_children][10] = {
#include "child_wishlist.h"
};

const int32_t childlist[1000][n_child_pref] = {
#include "gift_goodkids.h"
};

int main(void)
{
  glp_prob *mip = glp_create_prob();
  glp_set_prob_name(mip, "Santa's Gift Matching Problem.");
  glp_set_obj_dir(mip, GLP_MAX);


  glp_add_rows(mip, n_gift_type);
  glp_add_cols(mip, n_children);


  /* Allocate matrix memory */
  int *ia = malloc( (num_variables + 1) * sizeof(int));
  int *ja = malloc( (num_variables + 1) * sizeof(int));
  double *ar = malloc( (num_variables + 1) * sizeof(double)); 

  assert( ia );
  assert( ja );
  assert( ar );

  int count_non_trivial = 0;
  for( int gift_id = 0; gift_id < n_gift_type; gift_id++ ){
      if( !((gift_id+1) % 10) ) printf("Setting up problem %3d%% completed\n", (gift_id+1) / 10);
      fflush(stdout);
      glp_set_row_bnds( mip, gift_id + 1, GLP_DB, 0, 1000 ); // sum x <= 1000, try with (GLP_FX, 0, 1000) if this does not work. 
      for( int child_id = 0; child_id < n_children; child_id++ ){
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
          if (child_happiness + gift_happiness == -2 )
              continue;
          count_non_trivial++;

          int ival = child_happiness*100 + gift_happiness;
          double val = ival / 2000.0;

          char varname[30];
          sprintf( varname, "x%d,%d", gift_id,child_id);
          glp_set_col_name(mip, count_non_trivial, varname);
          glp_set_col_kind(mip, count_non_trivial, GLP_BV );
          glp_set_obj_coef(mip, count_non_trivial, val);

          ia[count_non_trivial] = gift_id;
          ja[count_non_trivial] = child_id; 
          ar[count_non_trivial] = 1.0;
      }
  }

  /* printf("Non trivial variables: %d\n", count_non_trivial); */
  /* Outputs: Non trivial variables: 10990072 */

  /* Solve */
  printf("Loading matrix.\n");
  glp_load_matrix(mip, count_non_trivial, ia, ja, ar);
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
  double x1 = glp_mip_col_val(mip, 1);
  double x2 = glp_mip_col_val(mip, 2);
  double x3 = glp_mip_col_val(mip, 3);
  double x4 = glp_mip_col_val(mip, 4);
  printf("\nz = %g; x1 = %g; x2 = %g; x3 = %g, x4 = %g\n", z, x1, x2, x3, x4);


  glp_delete_prob(mip);
  return 0;
}
