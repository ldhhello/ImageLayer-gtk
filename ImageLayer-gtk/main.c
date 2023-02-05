#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

#include "ImageLayer.h"
#include "GTKManager.h"

cairo_surface_t* image;

void DrawImage(cairo_t *ctx,int x,int y,int w,int h,cairo_surface_t *img){
    /*cairo_save(ctx);
    cairo_translate(ctx, x, y);
    cairo_pattern_t *pattern=cairo_pattern_create_for_surface(img);
    cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
    cairo_set_source(ctx, pattern);
    cairo_rectangle(ctx, 0, 0, w?w:cairo_image_surface_get_width(img), h?h:cairo_image_surface_get_height(img));
    cairo_fill(ctx);
    cairo_pattern_destroy(pattern);
    cairo_restore(ctx);*/
    
    if((w||h) || (w==cairo_image_surface_get_width(img)&&h==cairo_image_surface_get_height(img))){
            cairo_save(ctx);
            cairo_translate(ctx, x, y);
            cairo_scale(ctx, ((double)w)/((double)cairo_image_surface_get_width(img)), ((double)h)/((double)cairo_image_surface_get_height(img)));
            cairo_pattern_t *pattern=cairo_pattern_create_for_surface(img);
            cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
            cairo_set_source(ctx, pattern);
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

/*bool _kbhit()
{
    if(!gtk_events_pending())
        return false;
    
    gtk_main_iteration();
    
    if(last_key != -1)
    {
        //int r = last_key;
        //last_key = -1;
        return true;
    }
    
    return false;
}
int _getch()
{
    if(last_key != -1)
        return last_key;
    
    return -1; // todo
}*/

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
    
    image = LoadImage("/Users/donghyun/Downloads/알로에.png");
    cairo_surface_t* image2 = LoadImage("/Users/donghyun/Documents/스크린샷, 2022-11-20 오후 2.14.12.png");
    
    ImageLayer_initialize();
    
    GTKManager* manager = GTKManager_create();
    
    ImageLayer image_layer = DEFAULT_IMAGE_LAYER;
    image_layer.initialize(&image_layer, manager);
    
    image_layer.images = test_image;
    
    Image background = {"", 0, 0, 1, 0, image2};
    
    Image a = { "", 50, 5, 1, 0, image };
    Image b = { "", 20, 50, 1.5, 0, image };
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
            bool is_space = (GTKManager_getch(manager) == ' ');
            
            test_image[1].isHide = !test_image[1].isHide;
            
            if(is_space)
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
