/************************
 *****Fichier proc.h******
 ************************/


/*Inclusion conditionnelle du fichier*/

#ifndef __PROC__
#define __PROC__


/*Inclusion fonctions*/
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

/*Constantes*/

#define AFFICHAGE_COURT 	0
#define AFFICHAGE_LONG 		1

#define NB_PROC                 255


/*Definition structure Proc*/

typedef struct {
  int 			pid ;
  char 			comm [20] ;
  char 			state ;
  int 			ppid ;
  int 			pgrd ;
  int 			session ;
  int 			tty_nr ;
  int 			tpgid ;
  unsigned long 		flags ;
  unsigned long 		minflt ;
  unsigned long 		cminflt ;
  unsigned long 		majflt ;
  unsigned long 		cmajflt ;
  unsigned long 		utime ;
  unsigned long 		stime ;
  long int 		cutime ;
  long int 		cstime ;
  long int 		priority ;
  long int 		nice ;
  long int 		itrealvalue ;
  unsigned long 		starttime ;
  unsigned long 		vsize ;
  long int 		rss ;
  unsigned long 		rlim ;
  unsigned long 		startcode ;
  unsigned long 		endcode ;
  unsigned long 		startstack ;
  unsigned long 		kstkesp ;
  unsigned long 		kstkeip ;
  unsigned long 		signal ;
  unsigned long 		blocked ;
  unsigned long 		sigignore ;
  unsigned long 		sigcatch ;
  unsigned long 		wcham ;
  unsigned long 		nswap ;
  unsigned long 		cnswap ;
  int 			exit_signal ;
  int 			processor ;
} Proc ;


/*Definition structure ListProc*/
typedef struct ListProc * ListProc ;


/*Definition PROCESSUS_ERREUR*/
static Proc PROCESSUS_ERREUR = {0} ;


/*Creation d'une liste vide*/
ListProc nouvelleListProc () ;


/*Retourne le processus du noeud*/
Proc obtenirProc (ListProc laListProc) ;


/*Retourne la liste suivante*/
ListProc obtenirListProcSuivante (ListProc laListProc) ;


/*Definit le processus*/
void fixerProc (ListProc* laListProc, Proc leProc) ;


/*Definit la liste suivante*/
void fixerListProcSuivante (ListProc* laListProc, ListProc laListeSuivante) ;


/*Teste une liste*/
int estVide (ListProc laListProc) ;


/*Teste l'egalite de deux processus*/
int sontProcIdentiques (Proc processus1, Proc processus2) ;


/*Ajoute un processus en debut de liste*/
void ajouterListProc (ListProc* laListProc, Proc leProc) ;


/*Inverse une liste*/
void inverserListProc (ListProc* laListProc) ;


/*Desalloue la memoire de la liste*/
void detruireListProc (ListProc* laListProc) ;


/*Retourne la liste de tous les processus*/
ListProc  creerListeTousProc () ;


/*Retourne le processus correspondant a un pid*/
Proc pidEnProc (int pidProc, ListProc listeProc) ;


/*Retourne le pere d'un processus*/
Proc obtenirPere (Proc leProcFils, ListProc listeProc) ;


/*Retourne le processus correspondant a un nom*/
Proc nomEnProc (char* nomProc, ListProc listeProc) ;


/*Retourne la liste des processus peres d'un processus*/
ListProc creerListProcPere (Proc leProcFils, ListProc listeProc) ;


/*Retourne la liste des processus fils d'un processus*/
ListProc creerListProcFils (Proc leProcPere, ListProc listeProc) ;


/*Affichage complet avec entete*/
void afficherListProcAvecEntete (ListProc listeProc, int modeAffichage); 

void afficherListProc (ListProc listeProc, int modeAffichage);

/*creer les tableaux des commande-pid*/
int creerDoubleTabProc(ListProc listeProc, char ** tab_commande,int * tab_pid, int pos);


/* creer from rien les commandes-pid 
   0 = tab_commande
   1 = tab_pid
*/
void **creerDoubleTabProcFromScratch();

int* lireProcMapsDuPid(int pid);

int* cutIntoLignes(char* buf);

int* getAllAddr(char** buf);


int* cleanAllAddr(int* tab);

#endif
