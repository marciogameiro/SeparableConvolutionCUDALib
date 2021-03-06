/*
 * Copyright 1993-2013 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */


#include "separableConvolution.h"
#include <stdlib.h> //malloc, free
#include <string.h> //memcpy
#include <stdio.h>

void convolutionX(
    float *h_Dst,
    float *h_Src,
    float *h_Kernel,
    int imageW,
    int imageH,
    int imageD,
    int kernelR,
    int outofbounds,
    float outofboundsvalue
)
{
	for (int z = 0; z < imageD; z++)
		for (int y = 0; y < imageH; y++)
			for (int x = 0; x < imageW; x++)
			{
				float sum = 0;

				for (int k = -kernelR; k <= kernelR; k++)
				{
					int d = x + k;

					if ( d < 0 )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[z * imageW * imageH + y * imageW ] * h_Kernel[kernelR - k];
					}
					else if ( d >= imageW )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[z * imageW * imageH + y * imageW + imageW - 1 ] * h_Kernel[kernelR - k];
					}
					else
					{
						sum += h_Src[z * imageW * imageH + y * imageW + d] * h_Kernel[kernelR - k];
					}
				}

				h_Dst[z * imageW * imageH + y * imageW + x] = sum;
			}
}

void convolutionY(
    float *h_Dst,
    float *h_Src,
    float *h_Kernel,
    int imageW,
    int imageH,
    int imageD,
    int kernelR,
    int outofbounds,
    float outofboundsvalue
)
{
	for (int z = 0; z < imageD; z++)
		for (int y = 0; y < imageH; y++)
			for (int x = 0; x < imageW; x++)
			{
				float sum = 0;

				for (int k = -kernelR; k <= kernelR; k++)
				{
					int d = y + k;

					if ( d < 0 )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[z * imageW * imageH + x] * h_Kernel[kernelR - k];
					}
					else if ( d >= imageH )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[z * imageW * imageH + (imageH - 1) * imageW + x] * h_Kernel[kernelR - k];
					}
					else
					{
						sum += h_Src[z * imageW * imageH + d * imageW + x] * h_Kernel[kernelR - k];
					}
				}

				h_Dst[z * imageW * imageH + y * imageW + x] = sum;
			}
}

void convolutionZ(
    float *h_Dst,
    float *h_Src,
    float *h_Kernel,
    int imageW,
    int imageH,
    int imageD,
    int kernelR,
    int outofbounds,
    float outofboundsvalue
)
{
	for (int z = 0; z < imageD; z++)
		for (int y = 0; y < imageH; y++)
			for (int x = 0; x < imageW; x++)
			{
				float sum = 0;

				for (int k = -kernelR; k <= kernelR; k++)
				{
					int d = z + k;

					if ( d < 0 )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[y * imageW + x] * h_Kernel[kernelR - k];
					}
					else if ( d >= imageD )
					{
						if ( outofbounds == 1 )
							sum += outofboundsvalue * h_Kernel[kernelR - k];
						else if ( outofbounds == 2 )
							sum += h_Src[(imageD-1) * imageW * imageH + y * imageW + x] * h_Kernel[kernelR - k];
					}
					else
					{
						sum += h_Src[d * imageW * imageH + y * imageW + x] * h_Kernel[kernelR - k];
					}
				}

				h_Dst[z * imageW * imageH + y * imageW + x] = sum;
			}
}

extern "C" void convolutionCPU( float *image, float *kernelX, float *kernelY, float *kernelZ, int kernelRX, int kernelRY, int kernelRZ, int imageW, int imageH, int imageD, int outofbounds, float outofboundsvalue )
{
	float *h_Buffer;

	h_Buffer  = (float *)malloc(imageW * imageH * imageD * sizeof(float));

	//for ( int i = 0; i < kernelRX; ++i )
	//	fprintf(stderr, "kernel x: %.5f\n", kernelX[ i ] );

    convolutionX( h_Buffer, image, kernelX, imageW, imageH, imageD, kernelRX/2, outofbounds, outofboundsvalue );

	//for ( int i = 0; i < kernelRY; ++i )
	//	fprintf(stderr, "kernel y: %.5f\n", kernelY[ i ] );

    convolutionY( image, h_Buffer, kernelY, imageW, imageH, imageD, kernelRY/2, outofbounds, outofboundsvalue );

	//for ( int i = 0; i < kernelRZ; ++i )
	//	fprintf(stderr, "kernel z: %.5f\n", kernelZ[ i ] );

	convolutionZ( h_Buffer, image, kernelZ, imageW, imageH, imageD, kernelRZ/2, outofbounds, outofboundsvalue );

	memcpy( image, h_Buffer, imageW * imageH * imageD * sizeof(float) );

	free(h_Buffer);
}
