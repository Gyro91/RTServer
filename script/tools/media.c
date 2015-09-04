#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


void calculate_media(char *path)
{
	long double n, sum = 0, count = 0,
		  worst_case = 0, media;
	

	
	FILE *f;
	
	f = fopen(path, "r");
	if(f == NULL){
		perror("Impossibile aprire il file\n");
		exit(1);
	}
	
	// Reading champions until EOF
	
	while(!feof(f)){
		fscanf(f, "%llf\n", &n);
		count++;
		sum += n;
		
		if( worst_case < n )
			worst_case = n;

	}
	
	fclose(f);
	
	// Calculating media
	media = sum / count;
	
	
	printf("%llf", media);
	
	
		
}

int main (int argc, char **argv)
{
	calculate_media(argv[1]);
	return 0;
	
}
