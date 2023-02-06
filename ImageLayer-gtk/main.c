#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

#include "ImageLayer.h"
#include "GTKManager.h"

cairo_surface_t* image;

typedef unsigned int COLORREF;

// 얘를 쓰면 DT_LEFT, DT_RIGHT 같은걸 지정해서 가운데정렬, 왼쪽정렬, 오른쪽정렬 이런걸 할수있다!
void printText(cairo_t* cr, int left, int top, int right, int bottom, char* fontName, int size, COLORREF textColor, int align, const char* text)
{
    cairo_select_font_face(cr, fontName,
          CAIRO_FONT_SLANT_NORMAL,
          CAIRO_FONT_WEIGHT_NORMAL);
    
    cairo_set_font_size(cr, size);
    cairo_set_source_rgb(cr, 0, 0, 0);
    
    cairo_move_to(cr, left, top);
    cairo_show_text(cr, text);
}

Image test_image[1000];

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    
    image = GTKManager_load_bitmap("bongwan_1.bmp",
                                   RGB(29, 222, 38));//LoadImage("/Users/donghyun/Downloads/알로에.png");
    //cairo_surface_t* image2 = LoadImage("/Users/donghyun/Documents/스크린샷, 2022-11-20 오후 2.14.12.png");
    cairo_surface_t* image2 = GTKManager_load_bitmap("background.bmp",
                                                     RGB(29, 222, 38));
    
    ImageLayer_initialize();
    
    GTKManager* manager = GTKManager_create();
    
    ImageLayer image_layer = DEFAULT_IMAGE_LAYER;
    image_layer.initialize(&image_layer, manager);
    
    image_layer.images = test_image;
    
    Image background = { "", 0, 0, 1, 0, image2 };
    
    Image a = { "", 500, 500, 1, 0, image };
    Image b = { "", 200, 500, 1.5, 0, image };
    Image c = { "", 900, 200, 2, 0, image, true };
    
    _appendImage(&image_layer, background, true);
    _appendImage(&image_layer, a, true);
    _appendImage(&image_layer, b, true);
    _appendImage(&image_layer, c, true);
    
    image_layer.renderAll(&image_layer);
    
    GTKManager_start(manager);
    
    while(true)
    {
        if(GTKManager_kbhit(manager))
        {
            int ch = GTKManager_getch(manager);
            
            if(ch == 224)
                continue;
            
            if(ch == GTKMANAGER_UP)
            {
                test_image[2].y -= 3;
            }
            else if(ch == GTKMANAGER_LEFT)
            {
                test_image[2].x -= 3;
            }
            else if(ch == GTKMANAGER_DOWN)
            {
                test_image[2].y += 3;
            }
            else if(ch == GTKMANAGER_RIGHT)
            {
                test_image[2].x += 3;
            }
            
            if(ch == ' ')
            {
                image_layer.fadeOut(&image_layer, NULL);
                msleep(1000);
                image_layer.fadeIn(&image_layer, NULL);
            }
            else
                image_layer.renderAll(&image_layer);
        }
        
        msleep(10);
    }
    
    while(true) sleep(1000);
    
    /*for(int i=0; i<1000; i++)
    {
        test_image[1].x += 2;
        test_image[1].y += 1;
        test_image[2].x += 1;
        test_image[3].scale = 2 + (i%50)*0.005;
        
        
        if(i%2 == 0)
            image_layer.fadeIn(&image_layer, NULL);
        else
            image_layer.fadeOut(&image_layer, NULL);
        
        usleep(1000000);
    }*/
    
    /*cairo_t* cr = manager->main_buffer;
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 10*2, 10*2); // a / SCREEN_RESOLUTION
    cairo_line_to(cr, 100*2, 100*2);
    
    cairo_stroke(cr);
    
    DrawImage(cr, 50*2, 50*2, 0, 0, image);
    DrawImage(cr, 30*2, 30*2, 40*2, 50*2, image);
    
    cairo_set_source_rgb(cr, 1, 0.5, 0); // (bit / 255)
    
    cairo_move_to(cr, 100*2, 10*2);
    cairo_line_to(cr, 10*2, 100*2);
    
    cairo_stroke(cr);
    
    printText(cr, 100, 100, 1000, 1000, "SF Pro", 50, 0, 0, "Hello world");*/
    
    //GTKManager_start(manager);
    //gtk_main();

    return 0;
}
