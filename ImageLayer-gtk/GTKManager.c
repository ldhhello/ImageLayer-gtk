//
//  GTKManager.c
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#include "GTKManager.h"

#include <stdio.h>
#include <stdlib.h>

int GTKManager_transparent_color = 0;

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
    
    manager->title = " ";
    
    return manager;
}

void GTKManager_delete(GTKManager* manager)
{
    free(manager);
}

void GTKManager_set_title(GTKManager* manager, const char* title)
{
    manager->title = title;
    
    if(manager->started)
        gtk_window_set_title(GTK_WINDOW(manager->window), title);
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
    gtk_window_set_title(GTK_WINDOW(manager->window), manager->title);
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

cairo_surface_t* LoadImage(void* hinstance, char* filename, int type, int cx, int cy, int fu_load)
{
    if(type == IMAGE_PNG)
        return cairo_image_surface_create_from_png(filename);
    else if(type == IMAGE_BITMAP)
        return GTKManager_load_bitmap(filename, GTKManager_transparent_color);
    
    return NULL;
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

GTKManager* GTKManager_default_manager;
void GTKManager_set_default(GTKManager* manager)
{
    GTKManager_default_manager = manager;
}
bool _kbhit()
{
    return GTKManager_kbhit(GTKManager_default_manager);
}
int _getch()
{
    return GTKManager_getch(GTKManager_default_manager);
}

void GTKManager_set_default_transparent_color(int color)
{
    GTKManager_transparent_color = color;
}


// bitmap reading
// https://learn.microsoft.com/ko-kr/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure

// RGBRGBRGB~~~ 순서로 적혀있음!
// 이 함수 반환값은 나중에 무조건 free 해 줘야 함
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr;  //our file pointer
    BITMAPFILEHEADER bitmapFileHeader;  //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    unsigned char tempRGB;  //our swap variable

    //open file in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a .BMP file by checking bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);

    //move file pointer to the beginning of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
    
    int X = bitmapInfoHeader->biWidth, Y = bitmapInfoHeader->biHeight;
    
    int bytes = bitmapInfoHeader->biBitCount / 8;
    int sz = bitmapInfoHeader->biSizeImage;//bytes * X * Y;
    int res_size = 3 * X * Y;
    
    if(sz == 0) // 가끔 size가 0인 비트맵이 있는 듯...
        sz = bytes * X * Y;

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(sz);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,sz,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }
    
    unsigned char* res = malloc(res_size);
    
    int stride = ((((X * bytes*8) + 31) & ~31) >> 3) / bytes;
    
    for(int y=0; y<Y; y++)
    {
        for(int x=0; x<X; x++)
        {
            int rev_x = X - x - 1;
            int rev_y = Y - y - 1;
            
            int imageIdx = (y*stride + x) * bytes;
            int res_idx = (rev_y*X + x) * 3;
            
            int b = bitmapImage[imageIdx];
            int g = bitmapImage[imageIdx + 1];
            int r = bitmapImage[imageIdx + 2];
            
            res[res_idx] = r;
            res[res_idx+1] = g;
            res[res_idx+2] = b;
        }
    }

    //close file and return bitmap image data
    fclose(filePtr);
    free(bitmapImage);
    return res;
}

cairo_surface_t* GTKManager_load_bitmap(char* filename, int transparent_color)
{
    BITMAPINFOHEADER header;
    unsigned char* data = LoadBitmapFile(filename, &header);
    
    int transparent_r = transparent_color & 255;
    int transparent_g = (transparent_color >> 8) & 255;
    int transparent_b = (transparent_color >> 16) & 255;
    
    int X = header.biWidth, Y = header.biHeight;
    
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, X);
    
    unsigned char* res_data = malloc(stride * Y * 4);
    memset(res_data, 0, stride * Y * 4);
    
    for(int y=0; y<Y; y++)
    {
        for(int x=0; x<X; x++)
        {
            int idx = y*X + x;
            int res_idx = y*stride/4 + x;
            
            int r = data[3*idx], g = data[3*idx + 1], b = data[3*idx + 2];
            
            res_data[4*res_idx] = b;
            res_data[4*res_idx + 1] = g;
            res_data[4*res_idx + 2] = r;
            res_data[4*res_idx + 3] = 255;
            
            // transparent
            if(r == transparent_r && g == transparent_g && b == transparent_b)
            {
                res_data[4*res_idx] = 0;
                res_data[4*res_idx + 1] = 0;
                res_data[4*res_idx + 2] = 0;
                res_data[4*res_idx + 3] = 0;
            }
        }
    }
    
    free(data);
    
    // free(res_data); 얘를 free 안하면 메모리 누수가 생기고 free 하면 문제가 생기는데
    // 일단 이대로 냅두고 더 알아봐야 할 듯
    // 일반적으로 비트맵을 처음에 다 로드하는 방식이면 메모리 누수가 생겨도 별로 상관없다!
    
    return cairo_image_surface_create_for_data(res_data, CAIRO_FORMAT_ARGB32, X, Y, stride);
}
