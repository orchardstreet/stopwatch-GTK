/* stopwatch.c, a GUI stopwatch in C 

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

To compile, run: 
apk add gtk+3.0-dev gcc 
gcc stopwatch.c `pkg-config --cflags --libs gtk+-3.0` -o stopwatch 
*/

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <time.h>
#include <assert.h>
#define MAX_SECONDS 100000000000UL /* program will run for maximum of 3,215 years */

/* Init global variables */
enum CONTROL_CODES {START_B, STOP_B, RESET_B};

GtkWidget *stopwatch_label;
GThread *stopwatch_iter_thread;

unsigned char thread_active = 0;
unsigned char stop = 0;
unsigned char reset = 0;
unsigned long count = 0;
char stopwatch_str[256];

int
idle_change_stopwatch_text(gpointer data_ptr)
{
	char *data = (char *)data_ptr;
	gtk_label_set_text(GTK_LABEL(stopwatch_label),data);
	return 0;
}

void *
stopwatch()
{
	/* setting thread_active to 0 should only ever come right before g_thread_exit */
	/* Init variables */
	time_t the_time, updated_time;
	unsigned long difference;
	unsigned int hours, minutes, seconds;

	/* Get current time */
	the_time = time(NULL);
#ifdef DEBUG
	printf("stopwatch thread started\n");
#endif

	/* Loop and pause for 1ms over and over
	 * Check for updated unix epoch time in seconds after each pause
	 * and update stopwatch accordingly in seconds. 
	 * If the unix epoch time shifts back for some reason
	 * exit as fatal error.  If the stopwatch count is about to exceed MAX_SECONDS
	 * for some reason, then kill the thread, fart out an error
	 * and reset the count variable to 0 */
	while(1) {
		updated_time = time(NULL);
		if(updated_time != the_time) {
			if(updated_time < the_time) {
				fprintf(stderr,"time went back for some reason, exiting...\n");
				exit(EXIT_FAILURE);
			}
			difference = updated_time - the_time;
			if(difference > MAX_SECONDS - count) {
				count = 0;
				fprintf(stderr,"About to hit max time of %lu seconds, stopping...\n",MAX_SECONDS);
				thread_active = 0;
				g_thread_exit(0);
			}
			the_time = updated_time;
			/* update the count */
			count = count + difference;
			hours = count / 3600;
			minutes = count % 3600;
			seconds = minutes % 60;
			minutes = minutes / 60;
			sprintf(stopwatch_str,"%08u:%02u:%02u",hours,minutes,seconds);
			g_idle_add(idle_change_stopwatch_text,stopwatch_str);
		}
		usleep(100000);
		if(stop) {
			thread_active = 0;
			g_thread_exit(0);
		}
		if(reset) {
			count = 0;
			g_idle_add(idle_change_stopwatch_text,"00000000:00:00");
			thread_active = 0;
			g_thread_exit(0);
		}
	}

}

void 
stopwatch_control(GtkWidget *widget, gpointer status_ptr)
{

	unsigned char status = *((unsigned char *)status_ptr);

	if (status == START_B) {
		if(!thread_active) {
			thread_active = 1;
			reset = 0;
			stop = 0;
			stopwatch_iter_thread = g_thread_new(NULL,(GThreadFunc)stopwatch,NULL);
		}
		return;
	}
		
	if (status == STOP_B) {
		if(thread_active)
			stop = 1;
		return;
	}

	if (status == RESET_B) {
		if(!thread_active) {
			count = 0;
			gtk_label_set_text(GTK_LABEL(stopwatch_label),"00000000:00:00");
		} else {
			reset = 1;
		}
		return;
	}
}

int 
main(int argc, char *argv[])
{

	GtkWidget *window, *main_box, *stopwatch_frame, *main_grid, *start_button,*stop_button,*reset_button;
	unsigned char start_status = START_B;
	unsigned char stop_status = STOP_B;
	unsigned char reset_status = RESET_B;

	assert(sizeof(time_t) == 8);
	assert(sizeof(unsigned long) == 8);
	assert(sizeof(unsigned int) == 4);

	gtk_init(&argc,&argv);

	/* Setup main window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"stopwatch");
	/*gtk_window_set_default_size(GTK_WINDOW(window),400,600);*/
	/* Destroy window on close */
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	/* Setup main box */
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_set_margin_start(main_box,20);
	gtk_widget_set_margin_end(main_box,20);
	gtk_widget_set_margin_top(main_box,13);
	gtk_widget_set_margin_bottom(main_box,13);
	gtk_container_add(GTK_CONTAINER(window),main_box);

	/* Setup stopwatch frame */
	stopwatch_frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(main_box),stopwatch_frame,0,0,0);

	/* Create countdown label */
	stopwatch_label = gtk_label_new("00000000:00:00");
	gtk_container_add(GTK_CONTAINER(stopwatch_frame),stopwatch_label);

	/* Setup main grid */
	main_grid = gtk_grid_new();
	gtk_box_pack_start(GTK_BOX(main_box),main_grid,0,0,0);
	gtk_grid_set_column_spacing(GTK_GRID(main_grid),12);
	gtk_widget_set_margin_top(main_grid,10);
	gtk_widget_set_halign(main_grid,GTK_ALIGN_CENTER);

	/* Start button */
	start_button =  gtk_button_new_with_label("start");
	gtk_grid_attach(GTK_GRID(main_grid),start_button,0,0,1,1);
	g_signal_connect (start_button, "clicked", G_CALLBACK (stopwatch_control), &start_status);

	/* Stop button */
	stop_button = gtk_button_new_with_label("stop");
	gtk_grid_attach(GTK_GRID(main_grid),stop_button,1,0,1,1);
	g_signal_connect (stop_button, "clicked", G_CALLBACK (stopwatch_control), &stop_status);

	/* Reset button */
	reset_button = gtk_button_new_with_label("reset");
	gtk_grid_attach(GTK_GRID(main_grid),reset_button,2,0,1,1);
	g_signal_connect (reset_button, "clicked", G_CALLBACK (stopwatch_control), &reset_status);

	/* Show all and go to main loop */
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
