#include "avg_happiness.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE 256

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

	printf("Score of file '%s': %.10g\n", argv[1], avg_happiness(predval));
	free(predval);
	return 0;
}
