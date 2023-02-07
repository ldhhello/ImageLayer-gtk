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

// 어차피 무시하는 값이라 아무렇게나
#define GetModuleHandle(k) (k)

// 받은 값 그대로 리턴하게 설정
#define MAKEINTRESOURCE(k) (k)

#define IMAGE_BITMAP 0
#define IMAGE_PNG 1000 // not defined in windows

#define LR_LOADFROMFILE 1 // 리소스에서 로드가 0이던데 잘 모르겠다 나중에~~

#define Sleep(ms) msleep(ms) // defined in GTKManager.h

// 2^n 꼴
#define DT_LEFT 1
#define DT_CENTER 2
#define DT_RIGHT 4
#define DT_WORDBREAK 8

// in ImageLayerImpl.c
extern double RESOLUTION_MULTIPLIER;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

#endif /* ImageLayerDefinition_h */
