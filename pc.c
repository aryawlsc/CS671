/*
*   Parallel Password Cracker      
*/
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define _XOPEN_SOURCE

void strip(char*);
int dictionary_words(FILE*);


int main(int argc, char *argv[]){
  //Parameters error handling
  if(argc!=4){
        printf("Parallel Password Cracker\nUSAGE: ./pc <PATH TO DICTIONARY> '$6$<SALT>$' '<FULL HASH>'\n");
        exit(-1);
  }

  //To calculate the total time taken by the program
  struct timeval start, end;
  double delta;
  gettimeofday(&start, NULL);

  // clock_t start, end;
  // double cpu_time_used;

  // start = clock();

  int found=0;  //0 = password not found ; 1 = password found
  // We make maximum use of input buffer, therefore we try to take as many
  // as size of buffer character count
  char word[BUFSIZ],salt[BUFSIZ], password[BUFSIZ]; // Input buffer size
  FILE *words = fopen(argv[1],"r");     //Open dictionary file
  //File error handling
  if(words==NULL){
        printf("Cannot open dictionary file.\n");
        exit(-1);
  }

  // Now store salt and full hashed password
  strcpy(salt,argv[2]);
  strcpy(password,argv[3]);
  int size = dictionary_words(words); // Number of words in dictionary
  if(fseek(words,0,SEEK_SET)==-1)       exit(-1);       //seek error handling
  //Parallel Region
  #pragma omp parallel for private(word) shared(found) schedule(dynamic)
  for(int i=0;i<size;i++){
        if(fgets(word,BUFSIZ,words) != NULL){
                strip(word);
                if(found==1){
                  gettimeofday(&end, NULL);
                  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
                  printf("\nTime is : %f\n",delta);

                  exit(1);
                }
                printf("[*] THREAD %i TRYING: %s\n",omp_get_thread_num(),word);
                char *hash = (char*)crypt(word,salt);
                if(strcmp(hash,password) == 0){
                        printf("[+] PASSWORD FOUND: %s\n",word);
                        found=1;
                }
        }
  }

  fclose(words);
  if(found==0)  printf("[-] PASSWORD NOT FOUND, EXITING...\n");
  gettimeofday(&end, NULL);
  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("\nTime is : %f\n",delta);


  return 0;
}

void strip(char *word){
  int word_length;
  word_length=strlen(word);
  if(word[word_length-1] == '\n')
    word[word_length-1] = '\0';
}

int dictionary_words(FILE *file){
        if(file==NULL)  return -1;
        char ch;
        int lines = 0;
        while (ch != EOF){
                ch = fgetc(file);
                if(ch == '\n')
                  lines++;
        }
        return lines;
}
