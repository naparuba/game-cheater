#ifndef INTERFACE_H
#define INTERFACE_H

typedef struct data_page{
  GtkWidget *pLabel1;
  GtkWidget *pButtonSearch;
  GtkWidget *pButtonNewSearch;
  GtkWidget *pBoxCentral;
  GtkWidget *pEntry;
  GtkWidget *pWindow;
  GtkWidget *pCombo;
  pile_addr_p pile_addr;
  
  //  int pid;
} data_page_t,* data_page_p;

#endif
