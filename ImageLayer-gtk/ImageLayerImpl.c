//
//  ImageLayerImpl.c
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#include "ImageLayerImpl.h"
#include "ImageFading.h"
#include <cairo.h>

#include "GTKManager.h"

double RESOLUTION_MULTIPLIER;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;

double get_scale()
{
    return 2;
}

Size get_bitmap_size(cairo_surface_t* image)
{
    int width = cairo_image_surface_get_width(image);
    int height = cairo_image_surface_get_height(image);
    
    Size res;
    res.width = width;
    res.height = height;
    
    return res;
}

cairo_t* _create_new_back_ctx()
{
    cairo_surface_t* surface_ctx = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WINDOW_WIDTH, WINDOW_HEIGHT);
    cairo_t* res = cairo_create(surface_ctx);
    
    cairo_surface_destroy(surface_ctx);
    
    return res;
}

// global initialize
void ImageLayer_initialize()
{
    RESOLUTION_MULTIPLIER = get_scale();
    
    WINDOW_WIDTH = (int)(CONSOLE_WIDTH * DEFAULT_RESOLUTION_SCALE * RESOLUTION_MULTIPLIER);
    WINDOW_HEIGHT = (int)(CONSOLE_HEIGHT * 2 * DEFAULT_RESOLUTION_SCALE * RESOLUTION_MULTIPLIER);
}

void _initialize(ImageLayer* self, GTKManager* manager)
{
    //self->surface_ctx = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WINDOW_WIDTH, WINDOW_HEIGHT);
    //self->buffer_ctx = cairo_create(self->surface_ctx);
    
    self->console_ctx = manager->main_buffer;
    self->gtk_manager = manager;
    
    self->bufferDC = self->console_ctx;
}

void putBitmapToBackDC(cairo_t* backDC, Image image, UINT transparentColor)
{
    // 일단 transparentColor는 나중에..
    
    cairo_surface_t* bitmap;
    
    int no_delete = 0;
    
    if (image.fileName[0] != 0)
        bitmap = (HBITMAP)LoadImage(NULL, image.fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        //(NULL, image.fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    else
    {
        bitmap = image.bitmap;
        no_delete = 1;
    }
    
    double scale = image.scale * RESOLUTION_MULTIPLIER;
    if (scale == 0) scale = DEFAULT_RESOLUTION_SCALE * RESOLUTION_MULTIPLIER;

    int x = (int)(image.x * RESOLUTION_MULTIPLIER);
    int y = (int)(image.y * RESOLUTION_MULTIPLIER);
    
    const Size bitmapSize = get_bitmap_size(bitmap);
    const int width = (int)(bitmapSize.width * scale);
    const int height = (int)(bitmapSize.height * scale);
    
    if (image.isCenter)
    {
        x -= width / 2;
        y -= height / 2;
    }
    
    GTKManager_draw_image(backDC, x, y, width, height, bitmap);
    
    if(!no_delete)
        cairo_surface_destroy(bitmap);
}

cairo_t* getRenderedBackDC(ImageLayer* self)
{
    cairo_t* backDC = self->console_ctx;
    
    // 화면 초기화
    cairo_set_source_rgb(backDC, 0, 0, 0);
    cairo_paint(backDC);
    
    for (int i = 0; i < self->imageCount; i++) {
        if (!self->images[i].isHide)
            putBitmapToBackDC(backDC, self->images[i], self->transparentColor);
    }
    
    return backDC;
}

// 이 함수 별로 필요 없을 듯
void applyToDC(cairo_t* dstDC, cairo_t* srcDC) {
    //BitBlt(dstDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
        //srcDC, 0, 0, SRCCOPY);
}

void _renderAll(ImageLayer* self)
{
    cairo_t* backDC = getRenderedBackDC(self);
    if (self->applyToDC != NULL) self->applyToDC(backDC);
    applyToDC(self->console_ctx, backDC);
    //DeleteDC(backDC);
    //cairo_destroy(backDC); // 임시로 console_ctx임
    
    GTKManager_redraw(self->gtk_manager);
}

// 얘는 더블 버퍼링을 GTKManager 상에서 지원해서 startRender는 redraw 빼고 다 구현하면 된다!
void _startRender(ImageLayer* self)
{
    cairo_t* backDC = getRenderedBackDC(self);
    if (self->applyToDC != NULL) self->applyToDC(backDC);
    applyToDC(self->console_ctx, backDC);
}

void _endRender(ImageLayer* self)
{
    GTKManager_redraw(self->gtk_manager);
}

// _appendImage, _eraseImage, _clearImage : ImageLayer를 스택처럼 쓸수 있게 해주는 함수
// _appendImage는 이미지레이어의 images 배열 맨 뒤에 이미지를 하나 추가한다
// _eraseImage는 하나 없애고, _clearImage는 모두 없앤다!
// 이걸 제대로 사용하기 위해서는 images 배열에 여유 공간이 충분히 있어야 한다 (특히 appendImage는 images 배열 크기보다 imageCount가 작아야 한다)
void _appendImage(ImageLayer* self, Image im, int redraw)
{
    self->images[self->imageCount] = im;
    self->imageCount++;

    if (redraw)
        self->renderAll(self);
}

void _eraseImage(ImageLayer* self, int redraw)
{
    self->imageCount--;

    if(redraw)
        self->renderAll(self);
}

void _clearImage(ImageLayer* self, int redraw)
{
    self->imageCount = 0;

    if (redraw)
        self->renderAll(self);
}

void _fade(cairo_t* ctx, int alpha)
{
    // 기존 ImageLayer에서의 alpha는 높을수록 잘보이지만 여기 alpha는 낮을수록 잘보임!
    alpha = 255 - alpha;
    
    cairo_set_source_rgba(ctx, 0, 0, 0, (double)alpha / 255);
    cairo_paint(ctx);
}

// in ImageFading.h
void _renderAndFadeIn(ImageLayer* self, void (*applyToBackDC)())
{
    for (int alpha = 0; alpha <= 255; alpha += 17)
    {
        _startRender(self);
        _fade(self->console_ctx, alpha);
        _endRender(self);
        
        msleep(FADING_DELAY);
    }
}

void _renderAndFadeOut(ImageLayer* self, void (*applyToBackDC)())
{
    for (int alpha = 255; alpha >= 0; alpha -= 17)
    {
        _startRender(self);
        _fade(self->console_ctx, alpha);
        _endRender(self);
        
        msleep(FADING_DELAY);
    }
}

void _renderAndFade_value(ImageLayer* self, void(*applyToBackDC)(), int isFadeIn, int value)
{
    if (isFadeIn) {
        for (int alpha = value; alpha <= 255; alpha += 17)
        {
            _startRender(self);
            _fade(self->console_ctx, alpha);
            _endRender(self);
            
            msleep(FADING_DELAY);
        }
    }
    else {
        for (int alpha = 255; alpha >= value; alpha -= 17)
        {
            _startRender(self);
            _fade(self->console_ctx, alpha);
            _endRender(self);
            
            msleep(FADING_DELAY);
        }
    }
}
