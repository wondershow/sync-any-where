/***
For GSU CS6620 course project.
This is the main GUI of SyncAnywhere
how to compile:
gcc GUITutor.c -o GUITutor `pkg-config --cflags --libs gtk+-2.0`

TO save time:
rm -rf GUITutor; gcc GUITutor.c -o GUITutor `pkg-config --cflags --libs gtk+-2.0`; ./GUITutor

***/

#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"
#include "baselib.h"
#include "../SyncIPCMsg.h"
#include "IPCAgent.c"

GtkWidget * g_head_label;
GtkTextBuffer * g_unsync_tv_buffer; // to display unsynced file names
GtkTextBuffer * g_sync_tv_buffer;   // to displaysynced file names
gint g_num_unsync_files;
UnsyncFileList *g_usync_list_head; // a linked list for those unsynced files 

//send_msg_to_daemon
static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    gtk_main_quit ();
    return FALSE;
}

static gboolean add_entry_unsync_list(gchar *filepath)
{
	gchar *filename;
	gchar **plugins = NULL;
	gint row, col,total_line_num;
	GtkTextIter iter;
	UnsyncFileList *new_unsync_entry,*last_unsync_entry;

	plugins = g_strsplit (filepath, "/", 0);
	for (; NULL != *plugins; plugins++) 
	{
		filename = *plugins;
	}
	
	add_file_to_sync_repos(filepath);
	
	gtk_text_buffer_get_iter_at_offset(g_unsync_tv_buffer, &iter, 0);
	g_num_unsync_files++;
	gtk_text_buffer_insert(g_unsync_tv_buffer, &iter, g_strconcat(filename,"\n",NULL), -1);
	
	if(g_usync_list_head->next == NULL) // The list is empty add to the first item	
	{
		g_usync_list_head->file_path = filepath;
		g_usync_list_head->file_name = filename;
		g_usync_list_head->next = g_usync_list_head;
	}
	else // to add a new entry at the end of list.
	{
		last_unsync_entry = g_usync_list_head;
		while(last_unsync_entry->next != g_usync_list_head)
				last_unsync_entry = last_unsync_entry->next;
		
		
		new_unsync_entry = (UnsyncFileList *) g_malloc(sizeof(UnsyncFileList));
		new_unsync_entry->file_path = filepath;
		new_unsync_entry->file_name = filename;
		new_unsync_entry->next = g_usync_list_head;
		last_unsync_entry->next = new_unsync_entry;
	}
	return TRUE;
}

static void
cb_clicked( GtkButton *button,
            GtkWidget *text )
{
	gchar            *string; 
	static GtkWidget *dialog = NULL;
	gint              response;


	gchar *filepath;
	gint row, col,total_line_num;
	GtkTextIter iter;

   
	if( ! dialog )
	{
	dialog = gtk_file_chooser_dialog_new(
		  "Save to file", NULL,
		  GTK_FILE_CHOOSER_ACTION_OPEN,
		  GTK_STOCK_SAVE, GTK_RESPONSE_OK,
		  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		  NULL);
	}

	response = gtk_dialog_run( GTK_DIALOG( dialog ) );
	gtk_widget_hide( dialog );

	

	if( response != GTK_RESPONSE_OK )
	return;

	//buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW( text ) );
	//g_object_get( G_OBJECT( buffer ), "text", &string, NULL );

	filepath = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( dialog ) );
	//g_file_set_contents( filename, string, -1, NULL );
	
	add_entry_unsync_list(filepath);

	//gtk_statusbar_pop(statusbar, 0); 
	/*
	gtk_text_buffer_get_iter_at_mark(g_unsync_tv_buffer,
	&iter, gtk_text_buffer_get_insert(g_unsync_tv_buffer));

	
	col = gtk_text_iter_get_line_offset(&iter);

	msg = g_strdup_printf("Col %d Ln %d", col+1, row+1);

	//gtk_statusbar_push(statusbar, 0, msg);
	gtk_label_set_text(g_head_label,msg);
	g_free(msg);*/
}


/* Make a new hbox filled with button-labels. Arguments for the 
 * variables we're interested are passed in to this function. 
 * We do not show the box, but do show everything inside. */
static GtkWidget *set_menu_box(  gboolean homogeneous,
                            gint     spacing,
                            gboolean expand,
                            gboolean fill,
                            guint    padding ) 
{
    GtkWidget *vbox1;
    GtkWidget *hbox2;
    GtkWidget *button;
    GtkWidget *label;
    char padstr[80];
    GSList *group;
    GtkWidget *entry;
    
    /**The main vertical box to hold menu items*/
    vbox1 = gtk_vbox_new (FALSE, spacing);



    hbox2 = gtk_hbox_new (FALSE, spacing);
    label = gtk_label_new (NULL);     
    gtk_label_set_markup (GTK_LABEL (label), "");
    gtk_box_pack_start( GTK_BOX( hbox2 ), label, FALSE,TRUE, 0 );
    gtk_widget_show (label);





    //File browser button
    //button = gtk_file_chooser_button_new; 
    button = gtk_button_new_from_stock( "Choose File To Sync" );
    g_signal_connect( G_OBJECT( button ), "clicked",
                 	 G_CALLBACK( cb_clicked ), NULL );
    gtk_widget_set_size_request(button,120,-1);
    gtk_box_pack_start( GTK_BOX( hbox2 ), button, FALSE,TRUE, 0 );
    gtk_widget_show (button);
     
    gtk_box_pack_start( GTK_BOX( vbox1 ), hbox2, FALSE,TRUE, 0 );
    gtk_widget_show (hbox2);
	

    //HBox holds tcp radio button groups and tcp port
    hbox2 = gtk_hbox_new (FALSE, spacing);
    
    button = gtk_radio_button_new_with_label (NULL, "TCP");//TCP Radio button
    gtk_box_pack_start (GTK_BOX (hbox2), button, TRUE, TRUE, 0);
    gtk_widget_show (button);
     
    label = gtk_label_new (NULL);//Label for tcp port 
    
    gtk_label_set_markup (GTK_LABEL (label), "<small>TCP Port</small>");
    gtk_box_pack_start (GTK_BOX (hbox2), label, TRUE, TRUE, 0);  
    gtk_widget_show (label); 

    entry = gtk_entry_new ();// TCP port input
    gtk_entry_set_max_length (GTK_ENTRY (entry), 0);
    gtk_entry_set_text (GTK_ENTRY (entry), "50001");
    gtk_widget_set_size_request(entry,40,-1);
    gtk_box_pack_start (GTK_BOX (hbox2), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);

    gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0); // Add to the larger vertical box
    gtk_widget_show (hbox2);


    ///HBox holds udp radio button groups.
    hbox2 = gtk_hbox_new (FALSE, spacing);
     
    group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)); //UDP radio button
    button = gtk_radio_button_new_with_label (group, "UDP");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
    gtk_box_pack_start (GTK_BOX (hbox2), button, TRUE, TRUE, 0);
    gtk_widget_show (button);
     
    label = gtk_label_new (NULL);//Label for tcp port 
    gtk_label_set_markup (GTK_LABEL (label), "<small>UDP Port</small>");
    gtk_box_pack_start (GTK_BOX (hbox2), label, TRUE, TRUE, 0);  
    gtk_widget_show (label);

    entry = gtk_entry_new ();// UDP port input
    gtk_entry_set_max_length (GTK_ENTRY (entry), 0);
    gtk_entry_set_text (GTK_ENTRY (entry), "50001");
    gtk_widget_set_size_request(entry,40,-1);
    gtk_box_pack_start (GTK_BOX (hbox2), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);

    gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0); // Add to the larger vertical box
    gtk_widget_show (hbox2);	

    return vbox1;
}

/****
    Set the layout of file browser part, it is in the right middle of the main UI
*/
static GtkWidget *set_file_panel( gboolean homogeneous,
                            gint     spacing,
                            gboolean expand,
                            gboolean fill,
                            guint    padding ) 
{
	GtkWidget *vbox,*vbox1,*separator;
	GtkWidget *button;
	GtkWidget *label,*txtView;
	GtkWidget *view;
	GtkTextIter iter;

	char padstr[80];

	//HBox to hold both synchronized and unsynced file viewers
	vbox = gtk_vbox_new (FALSE, spacing);

	//UnSynched file lister
	vbox1 = gtk_vbox_new (FALSE, spacing);

	label = gtk_label_new ("UnSynchronized files:");//label
	gtk_label_set_markup(label, "<span font_style='italic' font='15' color='brown'>UnSynchronized files:</span>");
	gtk_box_pack_start (GTK_BOX (vbox1), label, expand, fill, padding);
	gtk_widget_show (label);



	view = gtk_text_view_new();
       //gtk_text_view_set_editable(view,FALSE);
	g_unsync_tv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	/*gtk_text_buffer_create_tag(buffer, "gap",
	"pixels_above_lines", 200, NULL);
	gtk_text_buffer_create_tag(buffer, "lmarg", 
	"left_margin", 5, NULL);

	gtk_text_buffer_create_tag(buffer, "blue_fg", 
	"foreground", "blue", NULL); 
	gtk_text_buffer_create_tag(buffer, "gray_bg", 
	"background", "gray", NULL); 
	gtk_text_buffer_create_tag(buffer, "italic", 
	"style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag(buffer, "bold", 
	"weight", PANGO_WEIGHT_BOLD, NULL); */

	gtk_text_buffer_get_iter_at_offset(g_unsync_tv_buffer, &iter, 0);
	
	
	gtk_text_buffer_insert(g_unsync_tv_buffer, &iter, "\n", -1);	
	gtk_text_buffer_insert(g_unsync_tv_buffer, &iter, "\n", -1);
	gtk_text_buffer_insert(g_unsync_tv_buffer, &iter, "\n", -1);
	gtk_text_buffer_insert(g_unsync_tv_buffer, &iter, "\n", -1);
	/*
	gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, 
	"Colored Text\n", -1, "blue_fg", "lmarg",  NULL);
	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Text with colored background\n", -1, "lmarg", "gray_bg", NULL);

	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Text in italics\n", -1, "italic", "lmarg",  NULL);

	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Bold text\n", -1, "bold", "lmarg",  NULL);*/
	
	gtk_box_pack_start(GTK_BOX(vbox1), view, TRUE, TRUE, 0);
	gtk_widget_show (view);
     
	gtk_box_pack_start(GTK_BOX(vbox), vbox1, TRUE, TRUE, 0);     
	gtk_widget_show (vbox1);
     

     
     
	//A line to seperate the unsynced and synced files
	separator = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 5);
	gtk_widget_show (separator);

     



	//UnSynched file lister
	vbox1 = gtk_vbox_new (FALSE, spacing);

	label = gtk_label_new ("Synchronized files:");//label
	gtk_label_set_markup(label, "<span font_style='italic' font='15' color='brown'>Synchronized files:</span>");
	gtk_box_pack_start (GTK_BOX (vbox1), label, expand, fill, padding);
	gtk_widget_show (label);

       
	view = gtk_text_view_new();
	gtk_text_view_set_editable(view,FALSE);
	g_sync_tv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	/*gtk_text_buffer_create_tag(buffer, "gap",
	"pixels_above_lines", 200, NULL);
	gtk_text_buffer_create_tag(buffer, "lmarg", 
	"left_margin", 5, NULL);

	gtk_text_buffer_create_tag(buffer, "blue_fg", 
	"foreground", "blue", NULL); 
	gtk_text_buffer_create_tag(buffer, "gray_bg", 
	"background", "gray", NULL); 
	gtk_text_buffer_create_tag(buffer, "italic", 
	"style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag(buffer, "bold", 
	"weight", PANGO_WEIGHT_BOLD, NULL); */

	gtk_text_buffer_get_iter_at_offset(g_sync_tv_buffer, &iter, 0);
	
	gtk_text_buffer_insert(g_sync_tv_buffer, &iter, "\n", -1);	
	gtk_text_buffer_insert(g_sync_tv_buffer, &iter, "\n", -1);
	gtk_text_buffer_insert(g_sync_tv_buffer, &iter, "\n", -1);
	gtk_text_buffer_insert(g_sync_tv_buffer, &iter, "\n", -1);

	/*
	gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, 
	"Colored Text\n", -1, "blue_fg", "lmarg",  NULL);
	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Text with colored background\n", -1, "lmarg", "gray_bg", NULL);

	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Text in italics\n", -1, "italic", "lmarg",  NULL);

	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, 
	"Bold text\n", -1, "bold", "lmarg",  NULL);*/
	
	gtk_box_pack_start(GTK_BOX(vbox1), view, TRUE, TRUE, 0);
	gtk_widget_show (view);
     
	gtk_box_pack_start(GTK_BOX(vbox), vbox1, TRUE, TRUE, 0);     
	gtk_widget_show (vbox1);
     
	gtk_box_pack_start(GTK_BOX(vbox), vbox1, TRUE, TRUE, 0);     
	gtk_widget_show (vbox1);
    
	return vbox;
}


/* Make a new hbox filled with button-labels. Arguments for the 
 * variables we're interested are passed in to this function. 
 * We do not show the box, but do show everything inside. */
static GtkWidget *set_body_box( gboolean homogeneous,
                            gint     spacing,
                            gboolean expand,
                            gboolean fill,
                            guint    padding ) 
{
    GtkWidget *box,*vbox2;
    GtkWidget *button;
    GtkWidget *separator;
    GtkWidget *vbox;
    char padstr[80];
    
    /* Create a new hbox with the appropriate homogeneous
     * and spacing settings */
    box = gtk_hbox_new (FALSE, spacing);
    
    /* Create a series of buttons with the appropriate settings
    button = gtk_button_new_with_label ("gtk_box_pack");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button); */
    vbox = set_menu_box(TRUE, 20, expand, fill, padding);
    gtk_box_pack_start (GTK_BOX (box), vbox, FALSE, TRUE, 5);
    gtk_widget_set_size_request(vbox,200,-1);
    gtk_widget_show(vbox);
    

    
    separator = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX (box), separator, FALSE, FALSE,0);
    gtk_widget_show (separator); 

    //file browser panel
    vbox2 = set_file_panel(FALSE, 0, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (box), vbox2, FALSE, TRUE,5);
    gtk_widget_show (vbox2);
    gtk_widget_set_size_request(vbox2,400,-1);     
    
    return box;
}


/* Make a new hbox filled with button-labels. Arguments for the 
 * variables we're interested are passed in to this function. 
 * We do not show the box, but do show everything inside. */
static GtkWidget *set_header( gboolean homogeneous,
                            gint     spacing,
                            gboolean expand,
                            gboolean fill,
                            guint    padding ) 
{
    GtkWidget *box;
    GtkWidget *button;
    GtkWidget *label;
    char padstr[80];
     
    /* Create a new hbox with the appropriate homogeneous
     * and spacing settings */
    box = gtk_hbox_new (FALSE, spacing);
    
    g_head_label = gtk_label_new ("SyncAnywhere!");
    gtk_widget_set_name (g_head_label,"label_head");
     
    gtk_label_set_markup(g_head_label, "<span font_style='italic' font='25' color='brown'>SyncAnywhere</span>");

    gtk_box_pack_start (GTK_BOX (box), g_head_label, expand, fill, padding);
    gtk_widget_show (g_head_label);
    

    button = gtk_button_new_with_label ("Quit");
    /* Setup the signal to terminate the program when the button is clicked */
    g_signal_connect_swapped (button, "clicked",
			      	  G_CALLBACK (gtk_main_quit),
			         NULL);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button,50,10);
    gtk_widget_show (button);
    return box;
}





/* Make a new hbox filled with button-labels. Arguments for the 
 * variables we're interested are passed in to this function. 
 * We do not show the box, but do show everything inside. */
static GtkWidget *set_status_bar( gboolean homogeneous,
                            gint     spacing,
                            gboolean expand,
                            gboolean fill,
                            guint    padding ) 
{
    GtkWidget *box;
    GtkWidget *button;
    GtkWidget *label;
    char padstr[80];
    
    /* Create a new hbox with the appropriate homogeneous
     * and spacing settings */
    box = gtk_hbox_new (FALSE, spacing);
    
    label = gtk_label_new ("SyncAnywhere!");
    gtk_label_set_markup(label, "<span font_style='italic' font='25' color='brown'>This is the status bar</span>");

    gtk_box_pack_start (GTK_BOX (box), label, expand, fill, padding);
    gtk_widget_show (label);
    
    return box;

}

int main( int   argc,
          char *argv[]) 
{
	GtkWidget *top_window;
	GtkWidget *button;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *separator;
	GtkWidget *label;
	GtkWidget *quitbox;
	int which;
	



    
	//GTK routine
	gtk_init (&argc, &argv);

	//initialize some global vars
	g_num_unsync_files = 0;
	
	//Initialize for the unsynced file list
       g_usync_list_head = (UnsyncFileList *) g_malloc(sizeof(UnsyncFileList));
	g_usync_list_head->next = NULL;
	
	
	/* Create our window */
	top_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/* You should always remember to connect the delete_event signal
	* to the main window. This is very important for proper intuitive
	* behavior */
	g_signal_connect (top_window, "delete-event",
						G_CALLBACK (delete_event), NULL);
        
	gtk_container_set_border_width (GTK_CONTAINER (top_window), 10);

	/* We create a vertical box (vbox) to pack the horizontal boxes into.
	* This allows us to stack the horizontal boxes filled with buttons one
	* on top of the other in this vbox. */
	box1 = gtk_vbox_new (FALSE, 0);
    
	
	/* Call our make box function - homogeneous = FALSE, spacing = 0,
	 * expand = FALSE, fill = FALSE, padding = 0 */
	box2 = set_header (FALSE, 0, TRUE, TRUE, 0);
	
	gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 20);
	
	gtk_widget_show (box2);
	
	/* Another new separator. */
	separator = gtk_hseparator_new ();
	/* The last 3 arguments to gtk_box_pack_start are:
	 * expand, fill, padding. */
	gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 5);
	gtk_widget_show (separator);
	
	
	/* To set the body part of the UI*/
	box2 = set_body_box(FALSE,0, TRUE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        
	gtk_widget_show (box2);
	
	
	
	/* Another new separator. */
	separator = gtk_hseparator_new ();
	/* The last 3 arguments to gtk_box_pack_start are:
	 * expand, fill, padding. */
	gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
	gtk_widget_show (separator);

	/* Args are: homogeneous, spacing, expand, fill, padding */
	box2 = set_status_bar (TRUE, 0, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_widget_show (box2);
	
	
    
    
	/* Pack the vbox (box1) which now contains all our widgets, into the
	* main window. */
	gtk_window_set_default_size(GTK_WINDOW(top_window),700,700);
	gtk_container_add (GTK_CONTAINER (top_window), box1);


	gtk_widget_show (box1);
	/* Showing the window last so everything pops up at once. */
	gtk_widget_show (top_window);

	/* And of course, our main function. */
	gtk_main ();

	/* Control returns here when gtk_main_quit() is called, but not when 
	* exit() is used. */
	return 0;
}
