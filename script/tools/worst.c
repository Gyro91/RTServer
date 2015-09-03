#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


void calculate_media(char *path)
{
	float n, worst_case = 0;
	
	FILE *f;
	
	f = fopen(path, "r");
	if(f == NULL){
		perror("Impossibile aprire il file\n");
		exit(1);
	}
	
	// Reading champions until EOF
	
	while(!feof(f)){
		fscanf(f, "%f\n", &n);
		
		if( worst_case < n )
			worst_case = n;

	}
	
	fclose(f);
	
	
	printf("%f", worst_case);
	
	
		
}

int main (int argc, char **argv)
{
	calculate_media(argv[1]);
	return 0;
	
}
