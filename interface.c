#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "pile_addr.h"
#include "interface.h"
#include "proc.h"

#define NO_VALUE 0
#define ONE_VALUE_FOUND 1
#define MULTIPLE_VALUE_FOUND 2



#define TAILLE_TABLEAU 10000
#define TAILLE_ADDR 10000
#define TAILLE_BUFFER_LABEL 1000

data_page_p dp;//Page principale

pile_addr_p pile_addr;

int nbenregistrements = 0;

int * tableau_addr_segment;
int nbcouple = 0;
int pid;


//groso modo 1G
#define TAILLE_MAX_SEGMENT 1000000000
#define QUOTIENT_TOO_BIG 16 //on ne prendra que 2*1/QUOTIENT du segment

int resolve_string(const char *str, int pid, void *base, void* end, int* nb_val,char* buffer,int flag_first_pass);
int resolve_value(long val, int pid, void *base, void* end, int* nb_val,char* buffer, int flag_first_pass);
void set_value(long val, void * addr);

void OnButtonSearch(GtkWidget *pButton, gpointer data);
void OnButtonNewSearch(GtkWidget *pButton, gpointer data);
void OnButtonVal(GtkWidget *pButton, gpointer data);
void OnButtonSting(GtkWidget *pButton, gpointer data);
void OnSelectCombo(GtkWidget *pCombo, gpointer data);
void input_callback(gpointer data,gint source,GdkInputCondition condition );

static void sigquit(int signum);

static void sigquit(int signum){
  printf("SIGINT, SIGTERM or SIGQUIT catched, Bye Bye\n");
  
  exit(0);
}

void interface_init(data_page_p dp){
  GtkWidget *pLabel1 = gtk_label_new(g_strdup_printf("Test"));
  GtkWidget *pButtonSearch = gtk_button_new_with_label("Search");
  GtkWidget *pButtonSelect = gtk_button_new_with_label("Select");
  GtkWidget *pBoxCentral = gtk_vbox_new(FALSE,0);
  GtkWidget *pEntry = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY (pEntry), 50);


  GtkWidget *pButtonNewSearch = gtk_button_new_with_label("New search");
  GtkWidget *pCombo = gtk_combo_new(); // cree une liste deroulante
  
  
  
  dp->pLabel1 = pLabel1;
  dp->pButtonSearch = pButtonSearch;
  dp->pBoxCentral = pBoxCentral;
  dp->pEntry = pEntry;
  dp->pCombo = pCombo;
  dp->pButtonNewSearch = pButtonNewSearch;

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(pCombo)->entry), "Liste Proc");


  g_signal_connect (G_OBJECT (GTK_COMBO (pCombo)->entry), "activate",
		    G_CALLBACK (OnSelectCombo), (gpointer) NULL);
  //g_signal_connect(G_OBJECT(pCombo), "activate", G_CALLBACK(OnSelectCombo), NULL);
  
  
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pLabel1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pCombo, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pEntry, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pButtonSearch, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pButtonSelect, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pBoxCentral), pButtonNewSearch, FALSE, FALSE, 0);
  
  g_signal_connect(G_OBJECT(pButtonSearch), "clicked", G_CALLBACK(OnButtonSearch), NULL);

  g_signal_connect(G_OBJECT(pButtonSelect), "clicked", G_CALLBACK(OnSelectCombo), NULL);

  g_signal_connect(G_OBJECT(pButtonNewSearch), "clicked", G_CALLBACK(OnButtonNewSearch), NULL);
  
  gtk_widget_show_all(dp->pBoxCentral);
}


int main(int argc, char **argv){
  GtkWidget *pWindow;

  int i;
  
  gtk_init(&argc,&argv);

  if(argc < 2 || argc %2 != 0){
    printf("Pas assez d'arguments: interface pid");
    return 0;    
  }
  
  pid = atoi(argv[1]);
  nbcouple = (argc - 2)/2;
  
  //printf("On a %i couples\n",nbcouple);
  printf("On surveille le pid %i\n",pid);
  
  pile_addr_p pile_addr = pile_addr_create();
  
  tableau_addr_segment = (int*)malloc(TAILLE_ADDR);
  
  for(i=0;i<nbcouple;i++){
    //printf("%i -> %i et %i -> %i \n",2*i,2*i-2,2*i+1,2*i -1);
    tableau_addr_segment[2*i]=strtoul(argv[2*i +2],0,16);
    tableau_addr_segment[2*i + 1]=strtoul(argv[2*i +3],0,16);
  }
  
  //for(i=0;i<nbcouple;i++){
  //printf("%i: %p - %p \n",i,(void*)tableau_addr_segment[2*i],(void*)tableau_addr_segment[2*i+1]);
  //}
  
  
  //reservation memoire des donnees des pages
  dp = (data_page_p)malloc(sizeof(data_page_t));
  dp->pile_addr = pile_addr;
  pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pWindow), "Funky Cheater");
  gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 200);
  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  dp->pWindow = pWindow;
  signal(SIGINT, sigquit);
  signal(SIGTERM, sigquit);
  signal(SIGQUIT, sigquit);
  interface_init(dp);
  gtk_container_add (GTK_CONTAINER (pWindow), dp->pBoxCentral);
  printf("Initialisation finie\n");
  gtk_widget_show_all(pWindow);
  gtk_main();
  
  return EXIT_SUCCESS;
}


void OnSelectCombo(GtkWidget *pCombo, gpointer data){
  printf("Bouton Select \n");
  printf("Selection = %s\n",gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(dp->pCombo)->entry)));
  //gtk_entry_get_text(GTK_ENTRY(dp->pEntry));
  gchar *string;
  
  string = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (dp->pCombo)->entry));

  sscanf(string,"%d",&pid);

  printf("pid = %d\n",pid);


  int* listeAddr = lireProcMapsDuPid(pid);
  
}

void OnButtonNewSearch(GtkWidget *pButton, gpointer data){
  GList *list = NULL;
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
  
  
  for(i=0; i<nb_entree; i++)
    list = g_list_append(list, g_strdup_printf("%d %s",tab_pid[i],tab_commande[i]));
  
  gtk_combo_set_popdown_strings( GTK_COMBO(dp->pCombo), list) ; // met la liste dans la conbo box
  
}

void OnButtonSearch(GtkWidget *pButton, gpointer data){
  const gchar * string = gtk_entry_get_text(GTK_ENTRY(dp->pEntry));
  long val = atol(string);
  void*base;
  void*end;
  int i = 0;
  int nb_val = 0;
  
  int nb_res = 0;
  char* buffer = (char*)malloc(10000);
  char* piti_buffer = (char*)malloc(10000);
  
  buffer[0]='\0';

  int flag_first_pass = 1;
  

  printf("on cherche la valeur %li\n",val);

  /*
  ** Attach processus
  */
  if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0) {
    perror("ptrace");
    return;
  }
  wait4(pid, 0, 0, 0);
  
  printf("Nombre de couples = %i\n",nbcouple);
  
  //on initialise le buffer a vide
  strcat(buffer,"");

  if(pile_addr_size(dp->pile_addr) != 0){
    printf("On n est plus dans la premiere passe\n");
    flag_first_pass = 0;
  }
  
  if(pile_addr_size(dp->pile_addr) == 1){
    void* addr = pile_addr_get(dp->pile_addr);
    set_value(val,addr);
    ptrace(PTRACE_DETACH, pid, 0, 0);
    return;
  }
  
  if(flag_first_pass == 1){//si premiere passe on parcoure la totalite
    printf("On est dans la première passe\n");
    for(i=0;i<nbcouple;i++){
      base = (void *) tableau_addr_segment[2*i];
      end = (void*) tableau_addr_segment[2*i + 1];
      
      if(errno == ERANGE)
	printf("Probleme de conversion\n");
      
      printf("Base = %p --> ",/*av[2*i + 1],*/base);
      printf("End = %p \n",/*av[2*i + 2],*/end);
      
      sprintf(piti_buffer,"Base = %p --> ",base);
      strcat(buffer,piti_buffer);
      
      sprintf(piti_buffer,"End = %p \n",end);
      strcat(buffer,piti_buffer);
      

      /*
      ** We call the resolver
      */
      
      //      nb_res += resolve_string((char*)string, pid, base, end, &nb_val,buffer , flag_first_pass);
      nb_res += resolve_value(val, pid, base, end, &nb_val,buffer , flag_first_pass);
      
      
      sprintf(piti_buffer,"Nombre Valeurss:%i\n",nb_val);
      //char * strcat(char *destination, const char *source); 
      
      //plantage
      //strcat(buffer,piti_buffer);
      
      printf("Nombre Valeurs: %i\n",nb_val);
      
      //resolve_string(av[1], pid, (void*)0xbffff000);
      /*
      ** End of game.
      */
      
    }
  }else{//si seconde passe, on ne parcoure que les valeurs necessaires
    printf("Mode sedonde passe\n");
    pile_addr_p nouvelle_pile = pile_addr_create();
    pile_addr_p p_temp;
    void * addr_temp=NULL;
    p_temp=dp->pile_addr;
    while(!pile_addr_is_void(p_temp))
      {
	long res;
	//char buffer3[1000];
	addr_temp = pile_addr_get(p_temp);
	//printf("on tente de relire l addresse %p\n",addr_temp);
	//printf("On cherche %i caracteres \n",strlen(string));
	//for(k=0;k<strlen(string);k++)
	//  buffer3[k] = ptrace(PTRACE_PEEKDATA, pid, addr_temp + k, 0);
	res = ptrace(PTRACE_PEEKDATA, pid, addr_temp, 0);
	//buffer3[strlen(string)]='\0';
	//printf("res = %li \n",res);
	
	if (val == res){
	  //la valeur corespond, on la rajoute dans la nouvelle_pile
	  nouvelle_pile = pile_addr_add(nouvelle_pile,addr_temp);
	  //printf("Valeure %p conservee \n",addr_temp);	  
	}
	p_temp=pile_addr_next(p_temp);
      }
    if(pile_addr_size(nouvelle_pile)==0){
      printf("Desole plus de valeurs...\n");
    }
    if(pile_addr_size(nouvelle_pile)==1){
      printf("Une Seule valeur trouvee\n");
      //dp->pButtonSearch = gtk_button_new_with_label("Set");
      gtk_button_set_label(GTK_BUTTON(dp->pButtonSearch),"Set");
      //      gtk_widget_show_all(dp->pWindow);
    }
    printf("Toujours %i valeurs\n",pile_addr_size(nouvelle_pile));
    //on met la nouvelle pile a la place de l ancienne
    dp->pile_addr = nouvelle_pile;
  }
  //long res2 = ptrace(PTRACE_PEEKDATA, pid,0xbffc8ca8 , 0);
  //printf("Res2: = %li \n",res2);

  ptrace(PTRACE_DETACH, pid, 0, 0);

  sprintf(piti_buffer,"Nombre Valeurss trouvees:%i\n",nb_res);
  //char * strcat(char *destination, const char *source); 
  
  //palntage
  //strcat(buffer,piti_buffer);

  
  //gtk_label_set_text(dp->pLabel1,buffer);
}


/*
** This function is based on ptrace(). It looks each byte of the memory for
** a string matching *str.
** It will print error messages on error while attaching but not if reading
** memory fails.
*/
int resolve_string(const char *str, int pid, void *base, void* end, int* nb_val,char* buffer, int flag_first_pass)
{
  long*res;
  int length;
  int nb_res = 0;
  int i;
  int j;
  int inc = sizeof(long);
  int flag = 0;/* disabled */
  int flag_toobig = 0;
  
  
  
  char* piti_buffer = (char*)malloc(10000);
  
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
	//sprintf(piti_buffer,"On est trop loin pos=%p end=%p\n",tmpbase,end);
	//strcat(buffer,piti_buffer);
	
	//printf("On est trop loin pos=%p end=%p\n",tmpbase,end);
	free(res);
	*nb_val = tmpbase - debut;
	return nb_res;
      }
      if((res[j] = ptrace(PTRACE_PEEKDATA, pid, tmpbase, 0)) == (-1)){
	
	
	/*
	** Error ?
	*/
	if (errno) {
	  if(errno == EFAULT)
	    printf("Efault \n");
	  
	  //sprintf(piti_buffer,"Arret sur %p \n",tmpbase);
	  //strcat(buffer,piti_buffer);
	  
	  //printf("Arret sur %p \n",tmpbase);
	  free(res);
	  *nb_val = tmpbase - debut;
	  return nb_res;
	}
      }
    }
    /*
    ** Compare data with requested string
    */
    if (!strcmp((char *) res, str)) {
      
      //sprintf(piti_buffer,"[%s] found in processus %d at : %p.\n",str, pid, base);
      //strcat(buffer,piti_buffer);
      
      
      //printf("[%s] found in processus %d at : %p.\n",str, pid, base);
      //printf("On a joute %p dans la pile\n",base);
      dp->pile_addr = pile_addr_add(dp->pile_addr,base);
      
      //on augmente le nombre de solutions trouvees
      nb_res++;
      flag = 1;
    }
    /*
    ** Look next bytes
    */
    base = (void *) ((unsigned int) base + 1);
    //on verifie que si on est dans le cas big:
    //si base entre bg_end et big_start -> base = big_start
    if(flag_toobig == 1 && base > big_end && base < big_start){
      strcat(buffer,"On passe le point big_end\n");

      printf("On passe le point big_end\n");
      base = (void*)big_start;
    }
  }
  return nb_res;
}



/*
** This function is based on ptrace(). It looks each byte of the memory for
** a string matching *str.
** It will print error messages on error while attaching but not if reading
** memory fails.
*/
int resolve_value(long val, int pid, void *base, void* end, int* nb_val,char* buffer, int flag_first_pass)
{
  long res;
  int nb_res = 0;
  int flag = 0;/* disabled */
  int flag_toobig = 0;
  
  
  
  char* piti_buffer = (char*)malloc(10000);
  
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

  /*
  ** _Ugly_ memory parsing.
  */
  while (1) {
    void*tmpbase=(void *) ((long) base);
    /*
    ** Read memory
    */
    if(tmpbase > end){
      //sprintf(piti_buffer,"On est trop loin pos=%p end=%p\n",tmpbase,end);
      //strcat(buffer,piti_buffer);
      
      //printf("On est trop loin pos=%p end=%p\n",tmpbase,end);
      //free(res);
      *nb_val = tmpbase - debut;
      return nb_res;
    }
    if((res = ptrace(PTRACE_PEEKDATA, pid, tmpbase, 0)) == (-1)){
      /*
      ** Error ?
      */
      if (errno) {
	if(errno == EFAULT)
	  printf("Efault \n");
	
	//sprintf(piti_buffer,"Arret sur %p \n",tmpbase);
	//strcat(buffer,piti_buffer);
	
	printf("Arret sur %p \n",tmpbase);
	//free(res);
	*nb_val = tmpbase - debut;
	return nb_res;
      }
    }
    /*
    ** Compare data with requested string
    */
    if (res == val){
      //sprintf(piti_buffer,"[%li] found in processus %d at : %p.\n",res, pid, base);
      
      
      //-----> GRO CACA strcat(buffer,piti_buffer);
      
      
      //printf("[%li] found in processus %d at : %p.\n",res, pid, base);
      //printf("On a joute %p dans la pile\n",base);
      dp->pile_addr = pile_addr_add(dp->pile_addr,base);
      //printf("Apres l ajout dans la pile\n");
      //on augmente le nombre de solutions trouvees
      nb_res++;
      flag = 1;
    }
    /*
    ** Look next bytes
    */
    base = (void *) ((unsigned int) base + 1);
    //on verifie que si on est dans le cas big:
    //si base entre bg_end et big_start -> base = big_start
    if(flag_toobig == 1 && base > big_end && base < big_start){
      strcat(buffer,"On passe le point big_end\n");
      
      printf("On passe le point big_end\n");
      base = (void*)big_start;
    }
   
  }
  return nb_res;
}


void set_value(long val, void * addr){
  long res = ptrace(PTRACE_POKEDATA, pid, addr, val);
  if(res == -1)
    printf("Erreur d'ecriture de %li sur l'addresse %p \n",*(&val),addr);
  else
    printf("Ecriture de %li sur %p \n",*(&val),addr);
  
}

