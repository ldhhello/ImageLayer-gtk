//
//  ImageFading.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#ifndef ImageFading_h
#define ImageFading_h

#define FADING_DELAY 20

extern void _renderAndFadeIn(ImageLayer* self, void (*applyToBackDC)());
extern void _renderAndFadeOut(ImageLayer* self, void (*applyToBackDC)());

#endif /* ImageFading_h */
