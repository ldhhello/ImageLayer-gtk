//
//  GTKManager.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#ifndef GTKManager_h
#define GTKManager_h

#include "ImageLayerDefinition.h"
#include <stdbool.h>

#define KEYBOARD_QUEUE_SIZE 2000

typedef struct
{
    cairo_surface_t* main_buffer_surface;
    cairo_t* main_buffer;
    
    GtkWidget *window;
    GtkWidget *darea;
    
    bool redraw_finished;
    bool started;
    
    int screen_width, screen_height;
    
    int keyboard_queue[KEYBOARD_QUEUE_SIZE];
    int queue_s, queue_e;
} GTKManager;

extern GTKManager* GTKManager_create();
extern void GTKManager_delete(GTKManager* manager);

extern void GTKManager_start(GTKManager* manager);

extern void GTKManager_draw_image(cairo_t *ctx,int x,int y,int w,int h,cairo_surface_t *img);

extern gboolean GTKManager_on_draw(GtkWidget *widget, cairo_t *cr, GTKManager* manager);
extern void GTKManager_on_quit(/* 여기 머 들어가는지 자세히 모르겠다 일단 보류 */);
extern gboolean GTKManager_on_key_press(GtkWidget *widget, GdkEventKey *event, GTKManager* manager);

extern void GTKManager_redraw(GTKManager* manager);

extern bool GTKManager_kbhit(GTKManager* manager);
extern int GTKManager_getch(GTKManager* manager);

// for windows LoadImage compatiblity
// 일단 이거 구현은 나중에 수정
extern cairo_surface_t* LoadImage(char* filename);

extern int msleep(long msec);

#endif /* GTKManager_h */
