#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;
int main(int argc,char *argv[]){

    if(argc == 1){
      printf("Exiting: Incorrect Argument Count\n");
      return(-1);
    }

    vector<int> lineCounts;
    vector<int> wordCounts;
    vector<int> charCounts;

    for (int i = 1; i < argc; i++){
      FILE *fptr;
      fptr = fopen(argv[i], "r");
      //Failed to open file
      if(fptr == NULL){
        printf("Error reading %s.\n", argv[i]);
      }else{
        char line[256];
        //Count each line in the doc
        int lines = 0;
        int words = 0;
        while(fgets(line, 256, fptr) != NULL){
            lines ++;
            //Count each word in the line split by a space
            char * wordToken = strtok(line, " ");
            while(wordToken != NULL){
              words ++;
              wordToken = strtok(NULL, " ");
            }
        }
        lineCounts.push_back(lines);
        wordCounts.push_back(words);
        charCounts.push_back(ftell(fptr));

        printf("\n");
      }
      fclose(fptr);
    }//End For Loop

    if(argc == 2){
      printf("Lines:%d \tWords:%d \tChars:%d \t%s\n",
            lineCounts[0], wordCounts[0], charCounts[0], argv[1]);
      exit(0);
    }

    //Aquire Totals
    int totalLines = 0;
    int totalWords = 0;
    int totalChars = 0;
    for (int i = 0; i < lineCounts.size(); i++) {
      totalLines += lineCounts[i];
      totalWords += wordCounts[i];
      totalChars += charCounts[i];
      printf("Lines:%d \tWords:%d \tChars:%d \t%s\n",
            lineCounts[i], wordCounts[i], charCounts[i], argv[i+1]);
    }
    printf("Lines:%d \tWords:%d \tChars:%d \tTotals\n", totalLines, totalWords, totalChars);

    exit(0);
}
