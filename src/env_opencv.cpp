/***************************************************************************
 *  src/env_opencv.cpp
 *  --------------------
 *
 *  Copyright (c) 2011, Edward T. Kaszubski ( ekaszubski@gmail.com )
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of interaction-ros-pkg nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

#include <envision/env_opencv.h>

env_rgb_pixel* opencv_to_env_rgb_pixel( IplImage * img, env_dims* outdims )
{
    outdims->w = img->width;
    outdims->h = img->height;
    env_rgb_pixel* result = (env_rgb_pixel*) img->imageData;
    return result;
}

IplImage * env_image_to_opencv( env_image * image )
{
    IplImage * result = new IplImage;

    result->nSize = sizeof( IplImage );
    result->ID = 0;
    result->nChannels = 1;
    result->depth = IPL_DEPTH_8U;
    result->dataOrder = 0;
    result->origin = 0;
    result->width = image->dims.w;
    result->height = image->dims.h;
    result->roi = NULL;
    result->maskROI = NULL;
    result->imageId = NULL;
    result->tileInfo = NULL;
    result->imageSize = image->dims.w * image->dims.h;
    result->imageData = (char*) image->pixels;
    result->widthStep = sizeof( char ) * image->dims.w;
    result->imageDataOrigin = result->imageData;

    return result;
}
