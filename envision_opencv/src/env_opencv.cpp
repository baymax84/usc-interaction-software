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
    static unsigned int const channels = 1;

    bool image_state = image != NULL;
    bool pixels_state = image_state && image->pixels != NULL;

    if( pixels_state )
    {
        printf( "Creating new IplImage from env_image %lux%lux%u\n", image->dims.w, image->dims.h, channels );

        // since we're converting from int* to char*, we can't just do a downcast, or we'll have 4x the number of data
        // instead, we need to allocate a new OpenCV image and do a per-pixel copy

        IplImage * result = cvCreateImage( cvSize( image->dims.w, image->dims.h ), IPL_DEPTH_8U, channels );

        const intg32* const sptr = env_img_pixelsw( image );
        for( env_size_t y = 0; y < image->dims.h; ++y )
        {
            for( env_size_t x = 0; x < image->dims.w; ++x )
            {
                ((uchar *)(result->imageData + y*result->widthStep))[x] = sptr[y*image->dims.w + x];
            }
        }

        return result;
    }

    printf( "Error: input image (%u, %u) not properly initialized\n", image_state, pixels_state );

    return NULL;
}
