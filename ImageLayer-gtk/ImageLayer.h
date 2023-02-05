//
//  ImageLayer.h
//  ImageLayer-gtk
//
//  Created by 이동현 on 2023/02/01.
//

#pragma once

#include "ImageLayerInterface.h"
#include "ImageLayerImpl.h"
#include "ImageFading.h"

#ifndef IMAGE_LAYER_H
#define IMAGE_LAYER_H

static const ImageLayer DEFAULT_IMAGE_LAYER = { NULL, 0, RGB(0,0,0) , NULL, NULL, NULL, _initialize, _renderAll, _renderAndFadeIn, _renderAndFadeOut, NULL,
        _startRender, _endRender, _appendImage, _eraseImage, _clearImage };

#endif
