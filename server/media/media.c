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
	
	printf("Champions number: %.0f\n", count);
	printf("Worst_case: %.0f\n", worst_case);
	printf("Media: %f ms\n", media);
	printf("(Worst_case + Media)/2: %f ms\n\n", n);
	
}

int main ()
{
	printf("\nConsumer1\n");
	calculate_media("out_c1.txt");
	printf("\nConsumer2\n");
	calculate_media("out_c2.txt");
	return 0;
	
}
