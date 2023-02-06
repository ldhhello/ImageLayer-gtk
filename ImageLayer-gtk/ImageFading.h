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

// used in DisasterEscape, not in original ImageLayer
extern void _renderAndFade_value(ImageLayer* self, void(*applyToBackDC)(), int isFadeIn, int value);

#endif /* ImageFading_h */
