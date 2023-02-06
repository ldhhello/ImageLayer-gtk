//
//  ImageLayerDefinition.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#ifndef ImageLayerDefinition_h
#define ImageLayerDefinition_h

#include <stdio.h>
#include <cairo.h>
#include <gtk/gtk.h>

#define CONSOLE_WIDTH 180
#define CONSOLE_HEIGHT 48

#define DEFAULT_RESOLUTION_SCALE 16

typedef unsigned int UINT;
typedef unsigned char BYTE;


// macOS 컴파일러에서 Long이 64비트임....
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;

typedef cairo_surface_t* HBITMAP; // for code compatiblity

// from definition of windows
#define RGB(r, g, b) ((UINT)(((BYTE)(r)|((UINT)((BYTE)(g))<<8))|(((UINT)(BYTE)(b))<<16)))

#define IMAGE_BITMAP 0
#define LR_LOADFROMFILE 0

// in ImageLayerImpl.c
extern double RESOLUTION_MULTIPLIER;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

#endif /* ImageLayerDefinition_h */
