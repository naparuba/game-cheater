// combo.c
// exemple de combobox en gtk+ (liste deroulante)
  
#include <stdio.h>
#include <gtk/gtk.h>
  
  
  
int main(int argc, char *argv[])
{
  
  GtkWidget *Dialogue, *Combo, *Bouton;
  GList *list = NULL;
  gint i;
  
  gtk_init(&argc, &argv);
  
  Dialogue = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(Dialogue), "Exemple de combo box");
  
  gtk_widget_show(Dialogue);
  
  for(i=0; i<10; i++)
    list = g_list_append(list, g_strdup_printf("chaine %d", i)); // ajoute une chaine a le liste
  
  
  Combo = gtk_combo_new(); // cree une liste deroulante
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(Dialogue)->vbox), Combo, TRUE, TRUE, 0);
  gtk_combo_set_popdown_strings( GTK_COMBO(Combo), list) ; // met la liste dans la conbo box
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(Combo)->entry), "coucou"); // definit le texte de la conbo box
  
  
  gtk_widget_show(Combo);
  
  Bouton = gtk_button_new_with_label("Fermer");
  gtk_signal_connect_object(GTK_OBJECT(Bouton), "clicked", (GtkSignalFunc)gtk_exit, NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(Dialogue)->action_area), Bouton , TRUE, TRUE, 0);
  
  gtk_widget_show(Bouton);
  
  gtk_main();
     
  return(0);
}
