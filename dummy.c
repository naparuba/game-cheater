#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void fonction(){
  char buffer[1000] =  "bibi";
  char* buffer2 = (char*)malloc(1000);
  int i=0;
  for(i=0;i<1000;i++)
    buffer2[i]='\0';
  strcpy(buffer2,"toto");
  
  printf("I: %i = %p \n",i,&i);
  printf("Buffer: %s = %p \n",buffer, buffer);
  printf("Buffer2: %s = %p \n",buffer2,buffer2);
  
  i=0;
  while(1){
    sleep(5);
    i++;
    printf(" Occurence = %i sur %p\n",i,&i);
    
    //fflush(stdout);
  }
}

int main(){
  
  fonction();

  printf("On quitte \n");
  return 0;
}

