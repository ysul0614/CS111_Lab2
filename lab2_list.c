#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "SortedList.h"

///STRUCT DEFINITIONS///

typedef struct pthArg{
  SortedList_t *list;
  int nIter;
}pthArg;

///GLOBAL VARIABLES
int doSync = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int spinLock = 0;

//FUNCTIONS
int setYieldType(char *opt);
void setName(char **name);
int setSyncType(char *opt);
void syncLock();
void syncUnlock();
void setChar(SortedListElement_t slArr[], char chArr[][5], int arrSize);

///MAIN///
int main(int argc, char*argv[]){

  //getopt related
  struct option longopts[] = {
    {"threads", required_argument, NULL, 't'},
    {"iterations", required_argument, NULL, 'i'},
    {"yield", required_argument, NULL, 'y'},
    {0,0,0,0},
  };

  //number variables
  opt_yield= 0;
  int nThreads = 1;
  int nIter = 1;
  char *name = "add-none";
  //time holding structs
  struct timespec start;
  struct timespec end;

  //getopts temporary variables
  int opt;
  int longindex;
  int temp;

  //pthread related variables
  pthArg args;

  //ACTUAL CODE

  //set the values for these variables using getopt_long
  while((opt = getopt_long(argc,argv, "t:i:y:", longopts, &longindex)) != -1){
    switch(opt){
      //THREAD
    case 't':
      temp = atoi(optarg);
      if(temp < 1){
	fprintf(stderr, "Number of threads must be at least 1. Skipping this option.\n");
	continue;
      }
      nThreads = temp;
      break;

      //ITERATIONS
    case 'i':
      temp = atoi(optarg);
      if(temp < 1){
	fprintf(stderr, "Number of iterations must be at least 1. Skipping this option.\n");
	continue;
      }
      nIter = temp;
      break;
      //YIELD
    case 'y':
      if(setYieldType(optarg) != 0){
	continue;
      }
      setName(&name);
      break;
      //SYNC
    case 's':
      if(setSyncType(optarg) != 0){
	fprintf(stderr, "Not a valid sync argument. Skipping this option.\n");
      }
      setName(&name);
      break;

      //UNRECOGNIZED OPTION (DEFAULT)
    default:
      fprintf(stderr, "%s is not an option. Skipping this option.\n", argv[optind - 1]);
      continue;
    }
  }

  //initialize empty list
  SortedList_t list;
  list.key = NULL;
  list.next = NULL;
  list.prev = NULL;

  //Create element container
  SortedListElement_t slArr[nThreads * nIter];

  //Create key container and insert
  char chArr[nThreads * nIter][5];
  setChar(slArr, chArr, nThreads * nIter);

  //Create thread ID container
  pthread_t thArr[nThreads];





}

int setYieldType(char *opt){
  if (opt == NULL){
    fprintf(stderr, "Argument is null.\n");
    return 1;
  }

  //iterate each character and add that option to opt_yield
  int isValid = 0;
  int i;
  for (i = 0; opt[i] != '\0'; i++){
    if(opt[i] == 'i'){
      opt_yield |= INSERT_YIELD;
      isValid = 1;
    }
    else if(opt[i] == 'd'){
      opt_yield |= DELETE_YIELD;
      isValid = 1;
    }
    else if(opt[i] == 'l'){
      opt_yield |= LOOKUP_YIELD;
      isValid = 1;
    }
  }

  //if at least one of the options are valid, return 0. Else, return 1 (none is valid)
  if(isValid == 1){
  return 0;
  }
  else{
    return 1;
  }
}

void setName (char **name){
  if(opt_yield == INSERT_YIELD){
    if(doSync == 0){
      *name = "list-i-none";
    }
    else if(doSync == 1){
      *name = "list-i-m";
    }
    else if(doSync == 2){
      *name = "list-i-s";
    }
  }

  else if(opt_yield == DELETE_YIELD){
    if(doSync == 0){
      *name = "list-d-none";
    }
    else if(doSync == 1){
      *name = "list-d-m";
    }
    else if(doSync == 2){
      *name = "list-d-s";
    }
  }

  else if(opt_yield == INSERT_YIELD | DELETE_YIELD){
    if(doSync == 0){
      *name = "list-id-none";
    }
    else if(doSync == 1){
      *name = "list-id-m";
    }
    else if(doSync == 2){
      *name = "list-id-s";
    }
  }

  else if(opt_yield == LOOKUP_YIELD){
    if(doSync == 0){
      *name = "list-l-none";
    }
    else if(doSync == 1){
      *name = "list-l-m";
    }
    else if(doSync == 2){
      *name = "list-l-s";
    }
  }

  else if(opt_yield == INSERT_YIELD | LOOKUP_YIELD){
    if(doSync == 0){
      *name = "list-il-none";
    }
    else if(doSync == 1){
      *name = "list-il-m";
    }
    else if(doSync == 2){
      *name = "list-il-s";
    }
  }

  else if(opt_yield == INSERT_YIELD | DELETE_YIELD | LOOKUP_YIELD){
    if(doSync == 0){
      *name = "list-idl-none";
    }
    else if(doSync == 1){
      *name = "list-idl-m";
    }
    else if(doSync == 2){
      *name = "list-idl-s";
    }
  }

  else if(opt_yield == 0){
    if(doSync == 0){
      *name = "list-none-none";
    }
    else if(doSync == 1){
      *name = "list-none-m";
    }
    else if(doSync == 2){
      *name = "list-none-s";
    }
  }

}

int setSyncType(char *opt){

  if(strcmp(opt, "m") == 0){
    doSync = 1;
  }
  else if(strcmp(opt, "s") == 0){
    doSync = 2;
  }
  else{
    return 1;
  }
  return 0;

}

void syncLock(){

  //if mutex option selected, attempt to lock
  if(doSync == 1){
    if(pthread_mutex_lock(&m)){
      fprintf(stderr, "Error locking the critical section.\n");
      exit(1);
    }
  }
  
  else if(doSync == 2){
    while(__sync_lock_test_and_set(&spinLock, 1)){
      continue;
    }
  }
}

void syncUnlock(){
  //if mutex selected, attempt to unlock
  if(doSync == 1){
    if(pthread_mutex_unlock(&m)){
      fprintf(stderr, "Error unlocking the critical section.\n");
      exit(1);
    }
  }
  
  else if(doSync == 2){
    __sync_lock_release(&spinLock);
  }
  
}

void setChar(SortedListElement_t slArr[], char chArr[][5] int arrSize){
  char chList[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  int i,j;
  for (i = 0; i < arrSize; i++){
    for(j = 0; j < 4; j++){
      chArr[i][j] = chList[rand() % 62];
    }
    chArr[i][4] = '\0';
    slArr[i].key = &(chArr[i]);
  }

}
