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

#define KEYBOARD_QUEUE_SIZE 4000

#define GTKMANAGER_LEFT 75
#define GTKMANAGER_RIGHT 77
#define GTKMANAGER_DOWN 80
#define GTKMANAGER_UP 72

// 여기서 만드는 창의 크기는 ImageLayerDefinition 의 CONSOLE_WIDTH, CONSOLE_HEIGHT 이 담당함

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
    
    const char* title;
} GTKManager;

extern GTKManager* GTKManager_create();
extern void GTKManager_delete(GTKManager* manager);

extern void GTKManager_set_title(GTKManager* manager, const char* title);
extern void GTKManager_start(GTKManager* manager);

extern void GTKManager_draw_image(cairo_t *ctx,int x,int y,int w,int h,cairo_surface_t *img);

extern gboolean GTKManager_on_draw(GtkWidget *widget, cairo_t *cr, GTKManager* manager);
extern void GTKManager_on_quit(/* 여기 머 들어가는지 자세히 모르겠다 일단 보류 */);
extern gboolean GTKManager_on_key_press(GtkWidget *widget, GdkEventKey *event, GTKManager* manager);

extern void GTKManager_redraw(GTKManager* manager);

extern bool GTKManager_kbhit(GTKManager* manager);
extern int GTKManager_getch(GTKManager* manager);

// for windows LoadImage compatiblity
// 일단 이 함수는 Filename, type를 제외한 모든 것을 무시한다
extern cairo_surface_t* LoadImage(void* hinstance, char* filename, int type, int cx, int cy, int fu_load);

extern int msleep(long msec);

// 함수 인자로 GTKManager 객체를 받지 않는 호환성 함수들을 호출할 기본 GTKManager 객체를 설정한다!
// _kbhit 과 _getch 는 기본 설정된 GTKManager에서 입력을 처리한다
extern void GTKManager_set_default(GTKManager* manager);
extern bool _kbhit();
extern int _getch();

// 시스템 전역에서 사용될 투명 색을 지정한다!
extern void GTKManager_set_default_transparent_color(int color);

// Bitmap reading

struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} __attribute__((packed));
typedef struct tagBITMAPFILEHEADER BITMAPFILEHEADER;

struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //specifies height in pixels
    WORD biPlanes;  //specifies the number of color planes, must be 1
    WORD biBitCount;  //specifies the number of bits per pixel
    DWORD biCompression;  //specifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
} __attribute__((packed));
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;

extern unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

extern cairo_surface_t* GTKManager_load_bitmap(char* filename, int transparent_color);

#endif /* GTKManager_h */
