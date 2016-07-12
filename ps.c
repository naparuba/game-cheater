/************************
 *****Fichier ps2.c*******
 ************************/


/*Inclusion librairie*/
#include "proc.h"


int nb_processus = 0;


/*Affichage de la syntaxe*/
int afficherSyntaxe ()
{
  printf ("Erreur syntaxe :\n"
	  "\tps2 <-c | -l> <-a | -b>\n"
	  "\tps2 <-c | -l> <-p | -f> <PID | NOM>\n"
	  "Options :\n"
	  "\t-c : format d'affichage court\n"
	  "\t-l : format d'affichage long\n"
	  "\t-a : listing de tous les processus\n"
	  "\t-b : listing des processus fils du bash courant\n"
	  "\t-p : listing des processus pere du processus specifie\n"
	  "\t-f : listing des processus fils du processus specifie\n"
	  "\tPID : pid du processus\n"
	  "\tNOM : nom du processus\n") ;
  return EXIT_FAILURE ;
}


/*Programme principal*/
int main (int argc, char** argv)
{
  int modeAffichage ;
  ListProc listeTousProc = creerListeTousProc (), listeProcAffiche ;
  
  char ** tab_commande = (char**)malloc(250*sizeof(char*));
  int * tab_pid = (int*)malloc(250*sizeof(int));
  
  modeAffichage = AFFICHAGE_COURT ;
  listeProcAffiche = listeTousProc ;
  
  int nb_entree;
  nb_entree = creerDoubleTabProc(listeProcAffiche,tab_commande,tab_pid,0);

  
  printf("Nombre d'entrée: %d\n",nb_entree);
  
  int i = 0;
  for(i=0;i<nb_entree;i++)
    printf("%d %d %s\n",i,tab_pid[i],tab_commande[i]);
  
  int * tab = lireProcMapsDuPid(2393);
  
  return 0;
}
