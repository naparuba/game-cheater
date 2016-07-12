#ifndef PILE_ADDR_H
#define PILE_ADDR_H

// Définition des structure policy et policy_p

typedef struct pile_addr
{
  void * addr;
  struct pile_addr* prec;
}pile_addr_t,* pile_addr_p;


pile_addr_p pile_addr_create(void);

pile_addr_p pile_addr_add(pile_addr_p p,void*a);

pile_addr_p pile_addr_remove(pile_addr_p p);

void* pile_addr_get(pile_addr_p p);

pile_addr_p pile_addr_next(pile_addr_p p);

int pile_addr_is_void(pile_addr_p p);

void pile_addr_print(pile_addr_p p);

int pile_addr_size(pile_addr_p p);

void pile_addr_flush(pile_addr_p p);

pile_addr_p pile_addr_remove_from_addr(pile_addr_p p, void * a);


#endif // PILE_ADDR_H
