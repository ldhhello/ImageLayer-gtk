//
//  GTKManager.c
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#include "GTKManager.h"

#include <stdio.h>
#include <stdlib.h>

GTKManager* GTKManager_create()
{
    GTKManager* manager = malloc(sizeof(GTKManager));
    
    manager->screen_width = (int)(CONSOLE_WIDTH * DEFAULT_RESOLUTION_SCALE / RESOLUTION_MULTIPLIER);
    manager->screen_height = (int)(CONSOLE_HEIGHT * DEFAULT_RESOLUTION_SCALE * 2 / RESOLUTION_MULTIPLIER);
    
    manager->main_buffer_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, manager->screen_width*2, manager->screen_height*2);
    manager->main_buffer = cairo_create(manager->main_buffer_surface);
    
    manager->redraw_finished = false;
    manager->started = false;
    
    manager->queue_s = manager->queue_e = 0;
    
    return manager;
}

void GTKManager_delete(GTKManager* manager)
{
    free(manager);
}

void GTKManager_start(GTKManager* manager)
{
    manager->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    manager->darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(manager->window), manager->darea);
    
    gtk_widget_add_events(manager->window, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(manager->darea), "draw",
        G_CALLBACK(GTKManager_on_draw), manager);
    
    g_signal_connect(manager->window, "destroy",
        G_CALLBACK(GTKManager_on_quit), manager);
    
    g_signal_connect(manager->window, "key_press_event",
        G_CALLBACK(GTKManager_on_key_press), manager);
    
    gtk_window_set_position(GTK_WINDOW(manager->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(manager->window),
        manager->screen_width, manager->screen_height);
    gtk_window_set_title(GTK_WINDOW(manager->window), "재난 탈출 시뮬레이션");
    //gtk_window_set_keep_above(GTK_WINDOW(manager->window), true);
    gtk_window_present (GTK_WINDOW (manager->window));
    
    gtk_widget_show_all(manager->window);
    
    manager->started = true;
}

void GTKManager_draw_image(cairo_t *ctx,int x,int y,int w,int h,cairo_surface_t *img)
{
    if((w||h) || (w==cairo_image_surface_get_width(img)&&h==cairo_image_surface_get_height(img))){
        cairo_save(ctx);
        cairo_translate(ctx, x, y);
        cairo_scale(ctx, ((double)w)/((double)cairo_image_surface_get_width(img)), ((double)h)/((double)cairo_image_surface_get_height(img)));
        cairo_pattern_t *pattern=cairo_pattern_create_for_surface(img);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
        cairo_set_source(ctx, pattern);
        cairo_pattern_set_filter(cairo_get_source(ctx), CAIRO_FILTER_NEAREST);
        cairo_rectangle(ctx, 0,0, cairo_image_surface_get_width(img), cairo_image_surface_get_height(img));
        cairo_fill(ctx);
        cairo_pattern_destroy(pattern);
        cairo_restore(ctx);
    }else{
        cairo_set_source_surface(ctx, img, x,y);
        cairo_rectangle(ctx, x,y, cairo_image_surface_get_width(img), cairo_image_surface_get_height(img));
        cairo_fill(ctx);
    }
}

void GTKManager_draw_image_scale(cairo_t* ctx, int x, int y, double scale, cairo_surface_t* img)
{
    int w = cairo_image_surface_get_width(img);
    int h = cairo_image_surface_get_height(img);
    
    GTKManager_draw_image(ctx, x, y, w * scale, h * scale, img);
}

gboolean GTKManager_on_draw(GtkWidget *widget, cairo_t *cr, GTKManager* manager)
{
    GTKManager_draw_image_scale(cr, 0, 0, 1/RESOLUTION_MULTIPLIER, manager->main_buffer_surface);
    
    manager->redraw_finished = true;
    return false;
}

void GTKManager_on_quit(/* 여기 머 들어가는지 자세히 모르겠다 일단 보류 */)
{
    exit(0);
    return;
}

bool GTKManager_queue_push(GTKManager* manager, int data)
{
    int next_e = (manager->queue_e+1) % KEYBOARD_QUEUE_SIZE;
    if(next_e == manager->queue_s)
        return false;
    
    manager->keyboard_queue[manager->queue_e] = data;
    manager->queue_e = next_e;
    return true;
}
int GTKManager_queue_pop(GTKManager* manager)
{
    if(manager->queue_s == manager->queue_e)
        return -1;
    
    int next_s = (manager->queue_s+1) % KEYBOARD_QUEUE_SIZE;
    
    int res = manager->keyboard_queue[manager->queue_s];
    manager->queue_s = next_s;
    
    return res;
}
int GTKManager_queue_empty(GTKManager* manager)
{
    return manager->queue_s == manager->queue_e;
}

gboolean GTKManager_on_key_press(GtkWidget *widget, GdkEventKey *event, GTKManager* manager)
{
    if(event->type != GDK_KEY_PRESS)
        return false;
    
    int key = event->keyval;
    
    switch(key)
    {
        case 65362:
            GTKManager_queue_push(manager, 224);
            GTKManager_queue_push(manager, GTKMANAGER_UP);
            break;
        case 65361:
            GTKManager_queue_push(manager, 224);
            GTKManager_queue_push(manager, GTKMANAGER_LEFT);
            break;
        case 65364:
            GTKManager_queue_push(manager, 224);
            GTKManager_queue_push(manager, GTKMANAGER_DOWN);
            break;
        case 65363:
            GTKManager_queue_push(manager, 224);
            GTKManager_queue_push(manager, GTKMANAGER_RIGHT);
            break;
        default:
            GTKManager_queue_push(manager, key);
    }
    
    return FALSE;
}

void GTKManager_redraw(GTKManager* manager)
{
    if(!manager->started)
        return;
    
    int yaman = false;
    manager->redraw_finished = yaman;
    gtk_widget_queue_draw(manager->window);
    
    while(!manager->redraw_finished)
        gtk_main_iteration();
}

bool GTKManager_kbhit(GTKManager* manager)
{
    int cnt = 0;
    while(gtk_events_pending() && GTKManager_queue_empty(manager))
    {
        gtk_main_iteration();
        
        if(cnt > 2000)
            return false;
        
        cnt++;
    }
    
    return !GTKManager_queue_empty(manager);
}

int GTKManager_getch(GTKManager* manager)
{
    while(GTKManager_queue_empty(manager))
        gtk_main_iteration();
    
    return GTKManager_queue_pop(manager);
}

cairo_surface_t* LoadImage(char* filename)
{// bmp는 나중에
    return cairo_image_surface_create_from_png(filename);
}

/* msleep(): Sleep for the requested number of milliseconds. */
// https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}
