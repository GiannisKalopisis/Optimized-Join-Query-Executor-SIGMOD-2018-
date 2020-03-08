#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define NUMBER (0xFFFFFFFF)   // 2^32 - 1

int main(int argc, char **argv){

    if (argc < 5){
        printf("Not enough arguments!\n");
        exit(EXIT_FAILURE);
    }

    int i = 1;
    int j = 0;
    srand(time(NULL));

    while (i < argc){
        if (atoi(argv[i]) <= 0){
            printf("Invalide no positive argument!\n");
            exit(EXIT_FAILURE);
        }
        //printf("%s \n", argv[i]);
        i++;
    }

    printf("This script makes 2 arrays with size: [%s, %s] and [%s, %s].\n", argv[1], argv[2], argv[3], argv[4]);
    printf("Values are between: [0, %u].\n", NUMBER );

    FILE *fR = fopen("R_file.txt", "w+");
    for (i = 0; i < atoi(argv[1]); i++){
        for (j = 0; j < atoi(argv[2]); j++){
            fprintf(fR, "%lu\t", (uint64_t) (rand() % NUMBER) );
        }
        fprintf(fR, "\n");
    }


    FILE *fS = fopen("S_file.txt", "w+");
    for (i = 0; i < atoi(argv[3]); i++){
        for (j = 0; j < atoi(argv[4]); j++){
            fprintf(fS, "%lu\t", (uint64_t) (rand() % NUMBER) );
        }
        fprintf(fS, "\n");
    }

    fclose(fR);
    fclose(fS);
    
    return 0;
}
