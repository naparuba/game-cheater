#include <stdlib.h>
#include <stdio.h>

#include "pile_addr.h"


//O(1)
pile_addr_p pile_addr_create(void){
  return NULL;
}

//O(1)
pile_addr_p pile_addr_add(pile_addr_p p, void* a){
  pile_addr_p tmp;
  if ((tmp=(pile_addr_p)malloc(sizeof(pile_addr_t)))==NULL)
    {
      printf("impossible d'allouer la memoire");
      return (pile_addr_p)EXIT_FAILURE;
    }
  tmp->addr=a;
  tmp->prec=p;
  return p=tmp;
}

//O(1)
pile_addr_p pile_addr_remove(pile_addr_p p){
  pile_addr_p tmp;
  if (!pile_addr_is_void(p))
    {
      tmp=p->prec;
      free(p);
      return p=tmp;
    }else{
      printf("impossible de pile_addr_remove une pile vide");
      return (pile_addr_p)EXIT_FAILURE;
    }
}

//O(1)
void* pile_addr_get(pile_addr_p p){
  if (!pile_addr_is_void(p))
    return p->addr;
  return NULL;
}

//O(1)
pile_addr_p pile_addr_next(pile_addr_p p){
  if(!pile_addr_is_void(p))
    return p->prec;
  return NULL;
}

//O(1)
int pile_addr_is_void(pile_addr_p p){
  return p==NULL;
}

//O(Taille p)
void pile_addr_print(pile_addr_p p){
  pile_addr_p p_temp;
  p_temp = p;
  while(!pile_addr_is_void(p_temp))
    {
      printf("Addr: %p\n",pile_addr_get(p_temp));
      p_temp=pile_addr_next(p_temp);
    }
  return;
}

//O(Taille p)
int pile_addr_size(pile_addr_p p){
  pile_addr_p p_temp;
  int i=0;
  p_temp=p;
  while(!pile_addr_is_void(p_temp))
    {
      i++;
      p_temp=pile_addr_next(p_temp);
    }
  return i;
}

//O(Taille p)
void pile_addr_flush(pile_addr_p p){
  while(!pile_addr_is_void(p))
    {p=pile_addr_remove(p);}
  return;
}



//O(Taille p)
pile_addr_p pile_addr_remove_from_addr(pile_addr_p p, void* addr){
  pile_addr_p res = p;
  pile_addr_p p_temp = p;
  pile_addr_p p_prec = NULL;
  pile_addr_p p_temp2 = NULL;
  
  while(!pile_addr_is_void(p_temp))
    {
      if( p_temp->addr == addr){//si on chope le bon element
	//on verifie si on est ds le permeier element
	if(p_temp == p){// si on est ds le permier element
	  //on free et renvoi le deuxieme element comme nouvelle pile
	  res = pile_addr_next(p_temp);
	  free(p_temp);
	  return res;
	}else{//on est pas ds le premier
	  //donc on libère
	  p_temp2 = pile_addr_next(p_temp);
	  free(p_temp);
	  p_prec->prec = p_temp2; 
	  return res;
	}
      }else{
	p_prec = p_temp;
	p_temp = pile_addr_next(p_temp);
      }
    }
  return res;
}
