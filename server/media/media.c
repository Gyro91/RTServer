#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


void calculate_media(char *path)
{
	float n, sum, count, worst_case = 0;
	
	float media;
	
	FILE *f;
	
	f = fopen(path, "r");
	if(f == NULL){
		perror("Impossibile aprire il file\n");
		exit(1);
	}
	
	// Reading champions until EOF
	
	while(!feof(f)){
		fscanf(f, "%f\n", &n);
		count++;
		sum += n;
		
		if( worst_case < n )
			worst_case = n;
		//if(count == 2)
		//	break;	
	}
	
	fclose(f);
	
	// Calculating media
	media = sum / count;
	
	// Parameter deadline/period
	n = ( media + worst_case ) / 2;
	
	printf("Sum %f\n",sum); 
	printf("Champions number: %f\n", count);
	printf("Worst_case: %f\n", worst_case);
	printf("Media: %f\n", media);
	printf("Runtime: %f\n", n);
	
}

int main ()
{
	calculate_media("out_c1.txt");
	calculate_media("out_c2.txt");
	return 0;
	
}