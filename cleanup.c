#include <stdio.h>
#include <string.h>
#include <assert.h>

#define n_children 1000000
#define n_gift_types 1000

int main(int argc, char *argv[] )
{
    if ( argc != 2 ){
        printf("Usage: %s <solution_filname>\n", argv[0]);
        return -1;
    }
    FILE *fp = fopen(argv[1], "r");

    if( !fp ){
        fprintf( stderr, "Cannot open file %s for reading.\n", argv[1]);
        return -1;
    }

    char buffer[256];
    int answer[n_children];
    int gifts[n_gift_types];
    int unassigend = n_children;

    /* initialize */
    for ( int child_id = 0; child_id < n_children; child_id++ )
        answer[child_id] = -1;
    for ( int gift_id = 0; gift_id < n_gift_types; gift_id++ )
        gifts[gift_id] = 1000;
    
    while( fgets( buffer, sizeof(buffer), fp )){
        if ( strncmp( buffer, "ChildId", strlen("ChildId")) == 0 ) continue;
        int child_id, gift_id;
        if( 2!=sscanf(buffer, "%d,%d", &child_id, &gift_id )) {
            fprintf(stderr, "Cannot read ids form line: '%s'\n", buffer);
            return -1;
        }
        if(answer[child_id] != -1 ){
            printf("child_id %d has already got a gift.\n", child_id);
            /* return -1; */
        }
        answer[child_id] = gift_id;
        unassigend--;
        gifts[gift_id]--;
    }
    fclose(fp);

    for ( int i = 0; i < 10; i++ )
        printf("%d\n", answer[i]);
    
    /* Twins? */

    printf("There are %d children that got no gift yet\n", unassigend); 

    for ( int child_id = 0; child_id < n_children; child_id++ )
        if( answer[child_id] == -1 ){
            /* printf("child_id %d has no gift yet.\n", child_id); */
            for ( int gift_id = 0; gift_id < n_gift_types; gift_id++ ){
                if( gifts[gift_id] > 0 ){
                    answer[child_id] = gift_id;
                    unassigend--;
                    gifts[gift_id]--;
                }
            }
        }
    
    /* Check */

    printf("There are %d children that got no gift yet\n", unassigend); 
    for ( int gift_id = 0; gift_id < n_gift_types; gift_id++ ){
        if( gifts[gift_id] != 0 ){
            printf("What? I still have %d gift(s) left of type %d?\n", gifts[gift_id], gift_id);
            return -1;
        }
    }
    for ( int child_id = 0; child_id < n_children; child_id++ )
        if( answer[child_id] == -1 ){
            printf("What? Child %d has still not got any gift?\n", child_id);
            return -1;
        }
    
    /* Write solution */
    fp = fopen("submission.csv", "w");
    if( !fp ){
        fprintf(stderr, "Cannot open file 'submission.csv' for writing.\n");
        return -1;
    }

    fprintf(fp, "ChildId,GiftId\n");

    for ( int child_id = 0; child_id < n_children; child_id++ )
        fprintf( fp, "%d,%d\n", child_id, answer[child_id]);
    fclose(fp);

    return 0;
}

