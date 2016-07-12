#include <sys/ptrace.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
  long res = 0;
  int pid = 13723;
  void * addr = 0xbffff988;
  
  //on s'attache au processus NULL et 0 sont ignores
  if ((res = ptrace(PTRACE_ATTACH, pid, 0, 0)) < 0) {
    perror("ptrace");
    return 0;
  }
  wait4(pid, 0, 0, 0);
  printf("Attache: %i = %li \n", pid, res);


  res = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
  
  printf("Res = %i \n", res);

  //on se detache
  if((res = ptrace(PTRACE_DETACH,pid,0,0)) < 0){
    perror("ptrace");
    return 0;
  }
  printf("Detache: %i = %li \n", pid, res);
    
  return 0;
}
