//
//  ImageLayerInterface.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#ifndef ImageLayerInterface_h
#define ImageLayerInterface_h

#include "ImageLayerDefinition.h"
#include "GTKManager.h"

typedef struct {
    char* fileName;
    int x, y;
    double scale;
    int isHide;

    cairo_surface_t* bitmap;
    int isCenter;
} Image;

typedef struct _ImageLayer {
    Image* images;
    int imageCount;
    UINT transparentColor;

    //HWND _windowHandle;
    //HDC _consoleDC;

    //HDC bufferDC;
    
    cairo_t* console_ctx;
    
    GTKManager* gtk_manager;
    void* padding2;
    
    //cairo_surface_t* surface_ctx;
    //cairo_t* buffer_ctx;

    void (*initialize)(struct _ImageLayer*, GTKManager*);
    void (*renderAll)(struct _ImageLayer*);
    
    // 원래 void (*applyToBackDC)(HDC) 였는데 HDC 쓰임을 잘 모르겠어서 일단 지움
    // NULL 넣으면 일단 동작하게 만들 예정
    void (*fadeIn)(struct _ImageLayer*, void(*applyToBackDC)());
    void (*fadeOut)(struct _ImageLayer*, void(*applyToBackDC)());
    void (*applyToDC)();

    // ImageLayer 외의 다른 작업 시 화면 깜박임 현상 방지를 위한 더블 버퍼링 관련 코드!!
    void (*startRender)(struct _ImageLayer*);
    void (*endRender)(struct _ImageLayer*);

    void (*appendImage)(struct _ImageLayer*, Image im, int redraw);
    void (*eraseImage)(struct _ImageLayer*, int redraw);
    void (*clearImage)(struct _ImageLayer*, int redraw);
} ImageLayer;

#endif /* ImageLayerInterface_h */
