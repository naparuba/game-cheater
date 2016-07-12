/************************
 *****Fichier proc.c******
 ************************/


/*Inclusion fichier associe*/
#include "proc.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*Constantes*/
#define TAILLE_TAMPON 		512
#define DECALAGE_PRIORITE 	60


/*Definition structure ListProc*/
struct ListProc {
  Proc leProcCourant ;
  ListProc laListProcSuivante ;
};


/*Creation d'une liste vide*/
ListProc nouvelleListProc ()
{
  return NULL ;
}


/*Retourne le processus du noeud*/
Proc obtenirProc (ListProc laListProc)
{
  return laListProc->leProcCourant ;
}


/*Retourne la liste suivante*/
ListProc obtenirListProcSuivante (ListProc laListProc)
{
  return laListProc->laListProcSuivante ;
}


/*Definit le processus*/
void fixerProc (ListProc* laListProc, Proc leProc)
{
  (*laListProc)->leProcCourant = leProc ;
}


/*Definit la liste suivante*/
void fixerListProcSuivante (ListProc* laListProc, ListProc laListeSuivante)
{
  (*laListProc)->laListProcSuivante = laListeSuivante ;
}


/*Teste une liste*/
int estVide (ListProc laListProc)
{
  return laListProc == NULL ;
}


/*Teste l'egalite de deux processus*/
int sontProcIdentiques (Proc processus1, Proc processus2)
{
  return processus1.pid == processus2.pid ;
}


/*Ajoute un processus en debut de liste*/
void ajouterListProc (ListProc* laListProc, Proc leProc)
{
  ListProc nouvelleListProc = (ListProc)malloc(sizeof(struct ListProc)) ;
  nouvelleListProc->leProcCourant = leProc ;
  nouvelleListProc->laListProcSuivante = *laListProc ;
  *laListProc = nouvelleListProc ;
}


/*Inverse une liste*/
void inverserListProc (ListProc* laListProc)
{
  ListProc laListProcInverse = nouvelleListProc ();
  while (!estVide(*laListProc))
    {
      ajouterListProc (&laListProcInverse, obtenirProc (*laListProc)) ;
      *laListProc = obtenirListProcSuivante (*laListProc) ;
    }
  *laListProc = laListProcInverse ;
}


/*Desalloue la memoire de la liste*/
void detruireListProc (ListProc* laListProc)
{
  if(!estVide(*laListProc))
    {
      ListProc temp = obtenirListProcSuivante (*laListProc) ;
      detruireListProc (&temp) ;
      free (*laListProc) ;
      *laListProc = NULL ;
    }
}


/*Retourne le processus lu dans /proc*/
Proc lireInfoProc (int pidProcLu)
{
  Proc processusLu ;
  int fd ;
  char nomFichier [80], tampon [TAILLE_TAMPON] ;
  sprintf (nomFichier, "/proc/%d/stat", pidProcLu) ;
  fd = open (nomFichier, O_RDONLY, 0) ;
  read (fd, tampon, TAILLE_TAMPON) ;
  sscanf (tampon, "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu"
	  "%ld %ld %ld %ld %*ld %ld %lu %lu %ld %lu %lu %lu %lu %lu"
	  "%lu %lu %lu %lu %lu %lu %lu %lu %d %d",
	  &processusLu.pid, processusLu.comm, &processusLu.state, &processusLu.ppid,
	  &processusLu.pgrd, &processusLu.session, &processusLu.tty_nr, &processusLu.tpgid,
	  &processusLu.flags, &processusLu.minflt, &processusLu.cminflt, &processusLu.majflt,
	  &processusLu.cmajflt, &processusLu.utime, &processusLu.stime, &processusLu.cutime,
	  &processusLu.cstime, &processusLu.priority, &processusLu.nice, &processusLu.itrealvalue,
	  &processusLu.starttime, &processusLu.vsize, &processusLu.rss, &processusLu.rlim,
	  &processusLu.startcode, &processusLu.endcode, &processusLu.startstack, &processusLu.kstkesp,
	  &processusLu.kstkeip, &processusLu.signal, &processusLu.blocked, &processusLu.sigignore,
	  &processusLu.sigcatch, &processusLu.wcham, &processusLu.nswap, &processusLu.cnswap,
	  &processusLu.exit_signal, &processusLu.processor) ;
  close (fd) ;
  return processusLu ;
}


/*Retourne la liste de tous les processus*/
ListProc  creerListeTousProc ()
{
  struct dirent *ent ;
  DIR *proc ;
  int i = 0 ;
  ListProc listeTousProc = nouvelleListProc () ;
  proc = opendir ("/proc") ;
  while (ent = readdir (proc)){
    //    printf("On rajoute un element\n");
    if (isdigit (ent->d_name[0]))
      ajouterListProc (&listeTousProc, lireInfoProc (atoi (ent->d_name))) ;
  }
  close (proc) ;
  inverserListProc (&listeTousProc) ;
  return listeTousProc ;
}


/*Retourne le processus correspondant a un pid*/
Proc pidEnProc (int pidProc, ListProc listeProc)
{
  if (!estVide(listeProc))
    {
      Proc processusCourant = obtenirProc(listeProc) ;
      if (processusCourant.pid == pidProc)
	return processusCourant ;
      else
	return pidEnProc (pidProc, obtenirListProcSuivante(listeProc)) ;
    }
  else
    return PROCESSUS_ERREUR ;
}


/*Retourne le pere d'un processus*/
Proc obtenirPere (Proc leProcFils, ListProc listeProc)
{
  return pidEnProc (leProcFils.ppid, listeProc) ;
}


/*Retourne le processus correspondant a un nom*/
Proc nomEnProc (char* nomProc, ListProc listeProc)
{
  if (!estVide(listeProc))
    {
      Proc processusCourant = obtenirProc(listeProc) ;
      if (!strncmp (nomProc, processusCourant.comm + 1, strlen (processusCourant.comm) - 2))
	return processusCourant ;
      else
	return nomEnProc (nomProc, obtenirListProcSuivante(listeProc)) ;
    }
  else
    return PROCESSUS_ERREUR ;
}


/*Retourne la liste des processus peres d'un processus*/
ListProc creerListProcPere (Proc leProcFils, ListProc listeProc)
{
  ListProc laListProcPere = nouvelleListProc () ;
  Proc leProcPere = obtenirPere (leProcFils, listeProc) ;
  while (leProcPere.ppid != 0)
    {
      leProcPere = obtenirPere (leProcFils, listeProc) ;
      ajouterListProc (&laListProcPere, leProcPere) ;
      leProcFils = leProcPere ;
    }
  return laListProcPere ;
}

void ajouterProcMemePere (Proc leProcPere, ListProc listeProc, ListProc* laListProcMemePere)
{
  Proc leProcFils ;
  ListProc listeBis = listeProc ;
  while (!estVide (listeBis))
    {
      leProcFils = obtenirProc (listeBis) ;
      if (leProcFils.ppid == leProcPere.pid)
	{
	  ajouterListProc (laListProcMemePere, leProcFils) ;
	  ajouterProcMemePere (leProcFils, listeProc, laListProcMemePere) ;
	}
      listeBis = obtenirListProcSuivante (listeBis) ;
    }
}


/*Retourne la liste des processus fils d'un processus*/
ListProc creerListProcFils (Proc leProcPere, ListProc listeProc)
{
  ListProc laListProcPere = nouvelleListProc () ;
  ajouterProcMemePere (leProcPere, listeProc, &laListProcPere) ;
  inverserListProc (&laListProcPere) ;
  return laListProcPere ;
}


/*Formate le nom du programme*/
char* formaterNomCommande (char* nomCommande)
{
  char* nomCommandeFormate ;
  nomCommandeFormate = nomCommande + 1 ;
  nomCommandeFormate [strlen (nomCommande) - 2] = '\0' ;
  return nomCommandeFormate ;
}


/*Affiche processus*/
void afficherInfoProc (Proc processusAffiche, int modeAffichage)
{
  if(modeAffichage == AFFICHAGE_LONG)
    printf ("%d\t%d\t%c\t%ld\t%ld\t%s\n", processusAffiche.pid, processusAffiche.ppid,
	    processusAffiche.state, processusAffiche.priority + DECALAGE_PRIORITE, processusAffiche.nice,
	    formaterNomCommande (processusAffiche.comm)) ;
  else
    printf ("%d\t%s\n", processusAffiche.pid, formaterNomCommande (processusAffiche.comm)) ;
}


/*Affiche entete de listing*/
void afficherEnTeteListProc (int modeAffichage)
{
  if (modeAffichage == AFFICHAGE_LONG)
    printf ("PID\tPPID\tETAT\tPRI\tNICE\tCMD\n") ;
  else
    printf ("PID\tCMD\n") ;
}


/*Affiche liste processus*/
void afficherListProc (ListProc listeProc, int modeAffichage)
{
  if (!estVide(listeProc))
    {
      afficherInfoProc(obtenirProc(listeProc), modeAffichage) ;
      afficherListProc(obtenirListProcSuivante(listeProc), modeAffichage);
    }
}


//Creer les doubles tableaux avec les noms des process
//et les pid
int creerDoubleTabProc(ListProc listeProc, char ** tab_commande,int * tab_pid, int pos)
{
  if(!estVide(listeProc)){
    Proc c = obtenirProc(listeProc);
    tab_pid[pos]=c.pid;
    char* stemp=formaterNomCommande(c.comm);
    tab_commande[pos]=(char*)malloc(strlen(stemp)+1);
    strncpy(tab_commande[pos],stemp,strlen(stemp));
    
    return creerDoubleTabProc(obtenirListProcSuivante(listeProc), tab_commande, tab_pid, pos+1)+1;
  }else{
    return 0;
  }
}


/*Affichage complet avec entete*/
void afficherListProcAvecEntete (ListProc listeProc, int modeAffichage)
{
  if (!estVide (listeProc))
    {
      afficherEnTeteListProc (modeAffichage) ;
      afficherListProc (listeProc, modeAffichage) ;
    }
}

/* creer from rien les commandes-pid 
   0 = tab_commande
   1 = tab_pid
*/
void **creerDoubleTabProcFromScratch(){
  ListProc listeTousProc = creerListeTousProc();
  void** res = (void**)malloc(2*sizeof(void*));
  
  char** tab_commande = (char**)malloc(NB_PROC*sizeof(char*));
  int *tab_pid = (int*)malloc(NB_PROC*sizeof(int));
  
  res[0]=(void*)tab_commande;
  res[1]=(void*)tab_pid;
  
  int nb_entree;
  nb_entree = creerDoubleTabProc(listeTousProc,tab_commande,tab_pid,0);
  
  printf("Nombre d'entrée: %d\n",nb_entree);
  
  int i = 0;
  for(i=0;i<nb_entree;i++)
    printf("%d %d %s\n",i,tab_pid[i],tab_commande[i]);

  return res;
  
}



int* lireProcMapsDuPid(int pid){
  char buffer[255];
  char buffer2[150000];
  int fd = -1;
  int nblus = -1;
  int * res;
  sprintf(buffer,"/proc/%d/maps",pid);
  fd = open(buffer,O_RDONLY);
  
  if(fd < 0){
    printf("Ouverture ratée\n");
  }else{
    printf("Ouverture de %s reussie sur fd=%d\n",buffer,fd);
  }
  
  nblus = read(fd,buffer2,150000);
  close(fd);  
  //printf("%d char lus\n",nblus);
  
  //printf("Buffer2 = %s\n",buffer2);
  printf("Avant cut\n");
  res = cutIntoLignes((char*)buffer2);
  
  return res;
}


int* cutIntoLignes(char* buf){
  int tailleTotale=strlen(buf);
  int i = 0;
  int lastCut = 0;
  
  char ** res = (char**)malloc(255);
  for(i=0;i<255;i++)
    res[i]=NULL;
  int ind = 0;
  
  //  printf("Apres initialisation\n");
  
  for(i=0;i<=tailleTotale;i++){
    if(buf[i] == '\n'){
      //char *temp = (char*)malloc((i-lastCut+1)sizeof(char));
      char * temp = strndup((buf+lastCut),i-lastCut+1);
      lastCut = i;
      //filtre sur lib
      if(strstr(temp,"lib") == NULL)
	res[ind++]=temp;
      //      printf("Ajout de %s sur %d\n",temp,ind - 1);
    }
  }
  
  //  printf("Apres pack1\n");
  
  //for(i=0;i<ind;i++)
  //printf("%d: Nouvelle chaine = %s \n",i,res[i]);
  
  printf("Avant getAllAddr\n");
  int *res2 = getAllAddr(res);

  //printf("Tableau final\n");  
  
  for(i=0;i<255;i++){
    if(res2[i] != 0){
      printf("%i: %x\n",i,res2[i]);      
    }
  }
  
  return res2;
}


int* getAllAddr(char** buf){
  int i = 0;
  char * stemp;
  int ind = 0;
  
  printf("Debut getAllAddr\n");
  
  int * res = (int*)malloc(255*sizeof(int));
  printf("Allocation: %p\n",res);
  for(i=0;i<255;i++)
    res[i]=0;
  
  printf("Avant boucle getalladdr\n");
  for(i=0;i<255;i++){
    if(buf[i]!=NULL){
      stemp = buf[i];
      int a,b;
      sscanf(stemp,"%x-%x",&a,&b);
      
      //printf("%d = Val a=%x, b=%x\n",i,a,b);
      res[ind++]=a;
      res[ind++]=b;
    }
  }
  
  /*
  for(i=0;i<255;i++){
    if(res[i] != 0){
      printf("%i: %x\n",i,res[i]);
    }
  }
  */
  
  printf("Avant cleanAllAddr\n");
  int * res2 = cleanAllAddr(res);

  /*
  for(i=0;i<255;i++){
    if(res2[i] != 0){
      printf("%i: %x\n",i,res2[i]);      
    }
  }
  */
 


  return res2;
}

int* cleanAllAddr(int* tab){
  int i = 0;
  int nb_elements = 0;
  int ind = 0;
  
  int * res = (int*)malloc(255*sizeof(int));
  for(i=0;i<255;i++){
    res[i]=0;
  }
  
  for(i=0;i<255;i++){
    if(tab[i]!=0){
      nb_elements++;      
    }
  }
  
  //printf("nb elements = %d\n",nb_elements-1);
  res[ind++]=tab[0];
  for(i=0;i<nb_elements-1;i++){
    int val = tab[i];
    if(i != 0 && i != nb_elements-1){
      //printf("On accède a %d %d\n",i+1,i-1);
      if(val == tab[i+1] || val == tab[i-1]){
	//printf("On enleve la valeur %x en %d \n",val,i);
      }else{
	//printf("On ajoute %x en %d\n",val,i);
	res[ind++]=val;
      }
    }
  }
  
  res[ind++]=tab[nb_elements-1];
  
  return res;
}

