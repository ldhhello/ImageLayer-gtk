#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

#include <pango/pangocairo.h>
//#include <pango/pangoft2.h>

#include "ImageLayer.h"
#include "GTKManager.h"

cairo_surface_t* image;

// 얘를 쓰면 DT_LEFT, DT_RIGHT 같은걸 지정해서 가운데정렬, 왼쪽정렬, 오른쪽정렬 이런걸 할수있다!
// fontName에는 문자열 리터럴이 들어와야 한다고 하자!! (그래야 비교가 편함ㅋㅋ)
// DT_WORDBREAK 이게 정확히 머하는지 기억이 안난다 나중에 구현
void printText(cairo_t* cr, int left, int top, int right, int bottom, char* fontName, int size, int textColor, int align, const char* text)
{
    // 문자열 리터럴로 "강원교육튼튼" 이 들어와야 작동한다
    if(fontName == "강원교육튼튼")
        fontName = "GangwonEduPower";
    
    int pango_align = PANGO_ALIGN_LEFT;
    
    if(align & DT_LEFT)
        pango_align = PANGO_ALIGN_LEFT;
    else if(align & DT_CENTER)
        pango_align = PANGO_ALIGN_CENTER;
    else if(align & DT_RIGHT)
        pango_align = PANGO_ALIGN_RIGHT;

    PangoFontDescription *pFontDescription = pango_font_description_new();

    // Set up the font description
    pango_font_description_set_size(pFontDescription, size * PANGO_SCALE);
    pango_font_description_set_family(pFontDescription, fontName);
    pango_font_description_set_weight(pFontDescription, PANGO_WEIGHT_NORMAL);
    pango_font_description_set_style(pFontDescription, PANGO_STYLE_NORMAL);
    pango_font_description_set_variant(pFontDescription, PANGO_VARIANT_NORMAL);
    pango_font_description_set_stretch(pFontDescription, PANGO_STRETCH_NORMAL);
    
    // Create a pango layout
    PangoLayout *pLayout = pango_cairo_create_layout(cr);
    pango_layout_set_text(pLayout, text, -1);

    // Set up the pango layout
    pango_layout_set_alignment(pLayout, pango_align);
    
    // http://www.manpagez.com/html/pango/pango-1.36.8/pango-Fonts.php
    // 10 pixel <=> 10 * PANGO_SCALE
    
    pango_layout_set_width(pLayout, (right - left) * PANGO_SCALE);
    pango_layout_set_wrap(pLayout, PANGO_WRAP_WORD);
    pango_layout_set_font_description(pLayout, pFontDescription);
    pango_layout_set_auto_dir(pLayout, TRUE);
    
    // Set up the cairo context for drawing the text
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    //cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);

    // Move to the top left coordinate before drawing the text
    cairo_move_to(cr, left, top);

    // Draw the layout text
    pango_cairo_show_layout(cr, pLayout);

    // Clean up
    g_object_unref(pLayout);
    pango_font_description_free(pFontDescription);
}

Image test_image[1000];

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    
    ImageLayer_initialize();
    
    GTKManager* manager = GTKManager_create();
    
    ImageLayer image_layer = DEFAULT_IMAGE_LAYER;
    image_layer.initialize(&image_layer, manager);
    
    GTKManager_set_default(manager);
    GTKManager_set_default_transparent_color(RGB(29, 222, 38));
    
    GTKManager_start(manager);
    
    image = LoadImage(NULL, "bongwan_1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    cairo_surface_t* image2 = LoadImage(NULL, "background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
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
    
    while(true)
    {
        if(_kbhit())
        {
            int ch = _getch();
            
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
                //image_layer.fadeOut(&image_layer, NULL);
                _renderAndFade_value(&image_layer, NULL, false, 90);
                Sleep(1000);
                //image_layer.fadeIn(&image_layer, NULL);
                _renderAndFade_value(&image_layer, NULL, true, 90);
            }
            else if(ch == VK_RETURN)
            {
                int center_x = WINDOW_WIDTH / 4;
                int center_y = WINDOW_HEIGHT / 4;
                //Image im = { "", center_x, center_y, 16, 0, bitmap_system_msg, true };

                //image_layer.appendImage(&image_layer, im, false);

                //image_layer.startRender(&image_layer);
                _renderAndFade_value(&image_layer, NULL, false, 90);

                printText(image_layer.bufferDC, 0, center_y - 30, center_x * 2, center_y + 400,
                    "강원교육튼튼", 54, RGB(255, 255, 255), DT_CENTER | DT_WORDBREAK, "휴 이제야 마음이 놓이네!");
                image_layer.endRender(&image_layer);
                
                //sleep_(1500);
                Sleep(1500);

                    //image_layer.endRender(&image_layer);
                _renderAndFade_value(&image_layer, NULL, true, 90);

                image_layer.renderAll(&image_layer);
            }
            else
                //image_layer.renderAll(&image_layer);
            {
                image_layer.startRender(&image_layer);
                printText(image_layer.bufferDC, 100, 100, 1000, 1000, "강원교육튼튼", 50, 0, DT_LEFT, "휴, 이제야 마음이 놓이네!");
                image_layer.endRender(&image_layer);
            }
        }
        
        Sleep(10);
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
