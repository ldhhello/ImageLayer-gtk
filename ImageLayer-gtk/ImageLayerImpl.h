//
//  ImageLayerImpl.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#include "ImageLayer.h"

#ifndef ImageLayerImpl_h
#define ImageLayerImpl_h

typedef struct {
    int width, height;
} Size;

// macOS 같은 환경에서는 DPI 조절이 의미없을 것 같기 때문에 1.0만 반환하게 만들 예정
extern double get_scale();
extern Size get_bitmap_size(cairo_surface_t* image);

extern void ImageLayer_initialize();
extern void _initialize(ImageLayer* self, GTKManager* manager);

extern void _renderAll(ImageLayer* self);
extern void _startRender(ImageLayer* self);
extern void _endRender(ImageLayer* self);

// _appendImage, _eraseImage, _clearImage : ImageLayer를 스택처럼 쓸수 있게 해주는 함수
// _appendImage는 이미지레이어의 images 배열 맨 뒤에 이미지를 하나 추가한다
// _eraseImage는 하나 없애고, _clearImage는 모두 없앤다!
// 이걸 제대로 사용하기 위해서는 images 배열에 여유 공간이 충분히 있어야 한다 (특히 appendImage는 images 배열 크기보다 imageCount가 작아야 한다)
extern void _appendImage(ImageLayer* self, Image im, int redraw);
extern void _eraseImage(ImageLayer* self, int redraw);
extern void _clearImage(ImageLayer* self, int redraw);

#endif /* ImageLayerImpl_h */
