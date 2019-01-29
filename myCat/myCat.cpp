#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    if(argc == 1){
      char str[20];
      //read user input until EOF is entered
      while (scanf("%s\n", str) != EOF) {
      }

    }else{
      //Read through all file arguments
      for (int i = 1; i < argc; i++){
          FILE *fptr;
          fptr = fopen(argv[i], "r");
          if(fptr != NULL){
            char line[80];
            //print current documents contents
            while(fscanf(fptr, "%s\n", line) != EOF){
              printf("%s ", line);
            }
            printf("\n");
            fclose(fptr);
          //Failed to open file
          }else{
            printf("Error reading %s.\n", argv[i]);
          }

      }

    }

}
