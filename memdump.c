/*
** memdump.c for 
** 
** Comments : a string resolver. It is basic, slow, beta, and just provided
** 'as is' with no warranty as code example.
** 
** Written by Clad Strife
** on Fri Mar 18 18:33:38 2005 - Paris
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

//groso modo 1G
#define TAILLE_MAX_SEGMENT 1000000000
#define QUOTIENT_TOO_BIG 16 //on ne prendra que 2*1/QUOTIENT du segment

void resolve_string(const char *str, int pid, void *base, void* end, int* nb_val);

int main(int ac, char **av)
{
  void*base;
  void*end;
  void*result;
  int pid;
  int nb_couples = 0;
  int i = 0;
  int nb_val = 0;
  /*
  ** We need 4 params :
  ** [progname] string PID 0x[base_address]
  */
  if (ac < 5 || ac % 2 == 0) {
    fprintf(stderr, 
	    "Usage :\n%s string PID 0x[base_address] 0x[end_address]\n", av[0]);
    return (1);
  }
  /*
  ** We init parameters
  */
  
  //marche aussi en stroll mais c'est moins beau car le compilo crie au meutre

  pid = atoi(av[2]);

  printf("Cherche dans le pid = %i \n",pid);
  
  /*
  ** Attach processus
  */
  if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0) {
    perror("ptrace");
    return;
  }
  wait4(pid, 0, 0, 0);
  
  nb_couples = (ac - 3)/2;
  printf("Nombre de couples = %i\n",nb_couples);
  
  for(i=1;i<=nb_couples;i++){
    base = (void *) strtoul(av[2*i + 1], 0, 16);
    end = (void*) strtoul(av[2*i + 2], 0, 16);
    
    if(errno == ERANGE)
      printf("Probleme de conversion\n");
    
    printf("Base = %p --> ",/*av[2*i + 1],*/base);
    printf("End = %p \n",/*av[2*i + 2],*/end);
    
    /*
    ** We call the resolver
    */
    
    resolve_string(av[1], pid, base, end, &nb_val);
    
    printf("Nombre Valeurs: %i\n",nb_val);
    
    //resolve_string(av[1], pid, (void*)0xbffff000);
    /*
    ** End of game.
    */
    
  }
  
  
  ptrace(PTRACE_DETACH, pid, 0, 0);
  return (0);
}


/*
** This function is based on ptrace(). It looks each byte of the memory for
** a string matching *str.
** It will print error messages on error while attaching but not if reading
** memory fails.
*/
void resolve_string(const char *str, int pid, void *base, void* end, int* nb_val)
{
  long*res;
  int length;
  int i;
  int j;
  int inc = sizeof(long);
  int flag = 0;/* disabled */
  int flag_toobig = 0;
  
  //  [--------------------------------------------] = L>1Go
  //                        |
  //                        v
  //  [----]oooooooooooooooooooooooooooooooooo[----] = 2 * 1/QUOTIENT_TOO_BIG eme * L
  // base big_end                      big_start  end
  
  void * big_end = NULL, * big_start = NULL;
  void * debut = base;
  unsigned long taille_segment = (ulong)(end - base);
  printf("Taille du segment: %lu\n",taille_segment);
  //Si on a un segment de plus d'1G on a un probleme
  
  if(taille_segment > TAILLE_MAX_SEGMENT){
    flag_toobig = 1;
    big_end = base + (taille_segment / QUOTIENT_TOO_BIG);
    big_start = end - (taille_segment / QUOTIENT_TOO_BIG);
  }
  
  /*
  ** length % inc should be equal to 0.
  */
  length = strlen(str) + 1;
  if (length % inc) {
    length += inc - (length % inc);
  }
  if ((res = malloc(length)) == 0) {
    perror("malloc");
    exit(1);
  }
  /*
  ** _Ugly_ memory parsing.
  */
  //printf ("Searching... from %p\n",base);
  while (1) {
    for (i = 0, j = 0; i < length; i += inc, j++) {
      void*tmpbase;
      
      tmpbase = (void *) ((long) base + i);
      //      if((ulong)tmpbase % 1000 == 0)
      //printf("tmp = %p\n",tmpbase);
      /*
      ** Read memory
      */
      if(tmpbase > end){
	printf("On est trop loin pos=%p end=%p\n",tmpbase,end);
	free(res);
	*nb_val = tmpbase - debut;
	return;
      }
      if((res[j] = ptrace(PTRACE_PEEKDATA, pid, tmpbase, 0)) == (-1)){
	
	
	/*
	** Error ?
	*/
	if (errno) {
	  if(errno == EFAULT)
	    printf("Efault \n");
	  printf("Arret sur %p \n",tmpbase);
	  free(res);
	  *nb_val = tmpbase - debut;
	  return;
	}
      }
    }
    /*
    ** Compare data with requested string
    */
    if (!strcmp((char *) res, str)) {
      printf("[%s] found in processus %d at : %p.\n",
	     str, pid, base);
      flag = 1;
    }
    /*
    ** Look next bytes
    */
    base = (void *) ((unsigned int) base + 1);
    //on verifie que si on est dans le cas big:
    //si base entre bg_end et big_start -> base = big_start
    if(flag_toobig == 1 && base > big_end && base < big_start){
      printf("On passe le point big_end\n");
      base = (void*)big_start;
    }
  }
  return;
}

