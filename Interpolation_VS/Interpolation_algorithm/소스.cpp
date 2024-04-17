#include <stdio.h>
#include <process.h>
#include <Windows.h>
#include <string.h>
#include <tchar.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <malloc.h>
#include "include/tiff.h"
#include "include/tiffconf.h"
#include "include/tiffio.h"
#include "include/tiffvers.h"

#define MY
//#define JWJI
//#define STRETCH
//#define NEAREST
#define BILINEAR
//#define BICUBIC

//#define ERR

double total_Time_CPU = 0;
//double total_Time_GPU = 0;
LARGE_INTEGER beginClock, endClock, clockFreq;
LARGE_INTEGER tot_beginClock, tot_endClock, tot_clockFreq;

float xres, yres;
double xres_new = 333.0;
double yres_new = 333.0;

void Stretch();				// stretching
void Nearest_Neighbor();	// 최단입점 보간법
void Bilinear();			// 쌍선형 보간법
void Bicubic();				// Spline 보간법

int main(void)
{
	QueryPerformanceFrequency(&tot_clockFreq);	// 시간을 측정하기위한 준비

#ifdef STRETCH
	total_Time_CPU = 0;
	QueryPerformanceCounter(&tot_beginClock); // 시간측정 시작
	Stretch();
	QueryPerformanceCounter(&tot_endClock);
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("Total processing Time of Stretch : %lf Sec\n", total_Time_CPU);
	printf("\n");
#endif
	
#ifdef NEAREST
	total_Time_CPU = 0;
	QueryPerformanceCounter(&tot_beginClock); // 시간측정 시작
	Nearest_Neighbor();
	QueryPerformanceCounter(&tot_endClock);
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("Total processing Time of Nearest Neighbor : %lf Sec\n", total_Time_CPU);
	printf("\n");
#endif
	
#ifdef BILINEAR
	total_Time_CPU = 0;
	QueryPerformanceCounter(&tot_beginClock); // 시간측정 시작
	//while (1)
	//{
		//printf("xres : %lf, yres : %lf\n", xres_new, yres_new);
		Bilinear();
		//xres_new += 1.0;
		//yres_new += 1.0;
	//}
	QueryPerformanceCounter(&tot_endClock);
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("Total processing Time of Bilinear : %lf Sec\n", total_Time_CPU);
	printf("\n");
#endif

#ifdef BICUBIC
	total_Time_CPU = 0;
	QueryPerformanceCounter(&tot_beginClock); // 시간측정 시작
	Bicubic();
	QueryPerformanceCounter(&tot_endClock);
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("Total processing Time of Bicubic : %lf Sec\n", total_Time_CPU);
	printf("\n");
#endif

	return 0;
}
#ifdef STRETCH
void Stretch()
{
	//double rate = res_new / xres;
	double rate_x;	// resample x 비율
	double rate_y;	// resample y 비율
	//int bpl_old, bpl_new, pad_old, pad_new;
	double xpix_new, ypix_new;
	int width_old, height_old;
	int width_new, height_new;
	uint32 min_color;
	//long pix_size;
	//unsigned char trash[3] = { 0 }; // 쓰레기 값
	BYTE* pix_old; // 원본 이미지
	BYTE* pix_new; // 리샘플링 이미지

	TIFF* input = TIFFOpen("images/input.tif", "rb");
	//TIFF* input2= TIFFOpen("input.tif", "rb");
	TIFF* output = TIFFOpen("images/output.tif", "wb");

	if (input == NULL)
	{
		printf("File not found!!\n");
		system("pause");
		exit(0);
	}

	TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width_old);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height_old);
	//TIFFGetField(input, TIFFTAG_ICCPROFILE, &count, &icc_profile);
	TIFFGetField(input, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(input, TIFFTAG_YRESOLUTION, &yres);
	TIFFGetField(input, TIFFTAG_PHOTOMETRIC, &min_color);


	if (xres_new >= xres)
	{
		rate_x = xres_new / (double)xres;
		xpix_new = (double)width_old * rate_x;
		if (yres_new >= yres)
		{
			rate_y = yres_new / (double)yres;
			ypix_new = (double)height_old * rate_y;
		}
		else if (yres_new < yres)
		{
			rate_y = (double)yres / yres_new;
			ypix_new = (double)height_old * (1 / rate_y);
		}
	}
	else if (xres_new < xres)
	{
		rate_x = (double)xres / xres_new;
		xpix_new = (double)width_old * (1 / rate_x);
		if (yres_new >= yres)
		{
			rate_y = yres_new / (double)yres;
			ypix_new = (double)height_old * rate_y;
		}
		else if (yres_new < yres)
		{
			rate_y = (double)yres / yres_new;
			ypix_new = (double)height_old * (1 / rate_y);
		}
	}

	//xpix_new = (double)width_old * rate_x;
	//ypix_new = (double)height_old * rate_y;

	if (xpix_new - (int)xpix_new >= 0.5)
		width_new = (int)xpix_new + 1;
	else
		width_new = (int)xpix_new;

	if (ypix_new - (int)ypix_new >= 0.5)
		height_new = (int)ypix_new + 1;
	else
		height_new = (int)ypix_new;

	printf("rate_x : %lf , rate_y : %lf\n", rate_x, rate_y);
	printf("xpix_new - : %lf , ypix_new - : %lf\n", xpix_new - (int)xpix_new, ypix_new - (int)ypix_new);
	printf("xpix_new : %lf , ypix_new : %lf\n", xpix_new, ypix_new);
	printf("width_old : %d , height_old : %d\n", width_old, height_old);
	printf("width_new : %d , height_new : %d\n", width_new, height_new);
	
	TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width_new);
	TIFFSetField(output, TIFFTAG_IMAGELENGTH, height_new);
	//TIFFGetField(input, TIFFTAG_ICCPROFILE, &count, &icc_profile);
	TIFFSetField(output, TIFFTAG_XRESOLUTION, (float)xres_new);
	TIFFSetField(output, TIFFTAG_YRESOLUTION, (float)yres_new);
	TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(output, TIFFTAG_ROWSPERSTRIP, height_new);
	TIFFSetField(output, TIFFTAG_PHOTOMETRIC, min_color);
	TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	
	pix_old = (BYTE *)calloc(width_old, sizeof(BYTE));
	pix_new = (BYTE *)calloc(width_new, sizeof(BYTE));

	
	// 소장님 알고리즘
#ifdef JWJI
	int wErr = 0;
	int hErr = 0;
	for (int row_new = 0, row_old = 0; row_new < height_new; row_new++)
	{
		TIFFReadScanline(input, pix_old, row_old, 0);
		hErr += height_old;
		if (hErr >= height_new)
		{
			row_old += (hErr / height_new);
			hErr %= height_new;
		}
		for (int col_new = 0, col_old = 0; col_new < width_new; col_new++)
		{
			pix_new[col_new] = pix_old[col_old];
			wErr += width_old;
			if (wErr >= width_new)
			{
				col_old += (wErr / width_new);
				wErr %= width_new;
			}
		}
		TIFFWriteScanline(output, pix_new, row_new, 0);
	}
#endif
	
	// 내 알고리즘
#ifdef MY	
	int decimal_x = roundf((rate_x - (int)rate_x) * 100);
	int decimal_y = roundf((rate_y - (int)rate_y) * 100);
	//double decimal_x = rate_x - (int)rate_x;
	//double decimal_y = rate_y - (int)rate_y;
	printf("decimal_x : %d, decimal_y : %d\n", decimal_x, decimal_y);

	//double err_h = 0;
	//double err_w = 0;

	if (xres_new >= xres)
	{
		if (yres_new >= yres)
		{
			for (int row_new = 0, row_old = 0, count_y = 0, err_h = 0; row_new < height_new; row_new++)
			{
				if (count_y % (int)rate_y == 0)
				{
					err_h += decimal_y;
					TIFFReadScanline(input, pix_old, row_old, 0);
					row_old++;		
					
					if (row_old >= height_old)
						row_old--;
						
				}
				count_y++;

				for (int col_new = 0, col_old = 0, count_x = 0, err_w = 0; col_new < width_new; col_new++)
				{
					pix_new[col_new] = pix_old[col_old];
					if (err_w >= 100)
					{
						err_w -= 100;
						pix_new[++col_new] = pix_old[col_old];
					}

					count_x++;
					if (count_x % (int)rate_x == 0)
					{
						err_w += decimal_x;
						col_old++;
						
						if (col_old >= width_old)
							col_old--;
							
					}
				}
				TIFFWriteScanline(output, pix_new, row_new, 0);

				if (err_h >= 100)
				{
					err_h -= 100;
					TIFFWriteScanline(output, pix_new, ++row_new, 0);
				}

			}
		}
		else if (yres_new < yres)
		{
			for (int row_new = 0, count_y = 0, err_h = 0; row_new < height_new; row_new++)
			{
				TIFFReadScanline(input, pix_old, count_y, 0);
				for (int col_new = 0, col_old = 0, count_x = 0, err_w = 0; col_new < width_new; col_new++)
				{
					pix_new[col_new] = pix_old[col_old];
					if (err_w >= 100)
					{
						err_w -= 100;
						pix_new[++col_new] = pix_old[col_old];
					}

					count_x++;
					if (count_x % (int)rate_x == 0)
					{
						err_w += decimal_x;
						col_old++;
						
						if (col_old >= width_old)
							col_old--;
							
					}
				}
				TIFFWriteScanline(output, pix_new, row_new, 0);
				err_h += decimal_y;
				if (err_h >= 100)
				{
					err_h -= 100;
					count_y++;
				}
				count_y += (int)rate_y;
			}
		}
	}
	else if (xres_new < xres)
	{
		
		if (yres_new >= yres)
		{
			for (int row_new = 0, row_old = 0, count_y = 0, err_h = 0; row_new < height_new; row_new++)
			{
				if (count_y % (int)rate_y == 0)
				{
					err_h += decimal_y;
					TIFFReadScanline(input, pix_old, row_old, 0);
					row_old++;
					
					if (row_old >= height_old)
						row_old--;
						
				}
				count_y++;

				for (int col_new = 0, count_x = 0, err_w = 0; col_new < width_new; col_new++)
				{
					pix_new[col_new] = pix_old[count_x];
					err_w += decimal_x;
					if (err_w >= 100)
					{
						err_w -= 100;
						count_x++;
					}
					count_x += (int)rate_x;
				}
				TIFFWriteScanline(output, pix_new, row_new, 0);

				if (err_h >= 100)
				{
					err_h -= 100;
					TIFFWriteScanline(output, pix_new, ++row_new, 0);
				}

			}
		}
		else if (yres_new < yres)
		{
			for (int row_new = 0, count_y = 0, err_h = 0; row_new < height_new; row_new++)
			{
				TIFFReadScanline(input, pix_old, count_y, 0);

				for (int col_new = 0, count_x = 0, err_w = 0; col_new < width_new; col_new++)
				{
					pix_new[col_new] = pix_old[count_x];
					err_w += decimal_x;
					if (err_w >= 100)
					{
						err_w -= 100;
						count_x++;
					}
					count_x += (int)rate_x;
				}

				TIFFWriteScanline(output, pix_new, row_new, 0);
				err_h += decimal_y;
				if (err_h >= 100)
				{
					err_h -= 100;
					count_y++;
				}
				count_y += (int)rate_y;
			}
		}
	}

#endif
	free(pix_old);
	free(pix_new);
	TIFFClose(input);
	TIFFClose(output);
}
#endif

#ifdef NEAREST
void Nearest_Neighbor()
{

}
#endif

#ifdef BILINEAR
void Bilinear()
{
	//double rate = res_new / xres;
	double rate_x;	// resample x 비율
	double rate_y;	// resample y 비율

	//int bpl_old, bpl_new, pad_old, pad_new;
	double xpix_new, ypix_new;
	int width_old, height_old;
	int width_new, height_new;
	uint32 min_color;
	//long pix_size;
	//unsigned char trash[3] = { 0 }; // 쓰레기 값
	BYTE* pix_old = NULL; // 원본 이미지
	BYTE* pix_new = NULL; // 리샘플링 이미지
	BYTE* pix_array = NULL; // Bilinear 연산하는 배열.
	BYTE* xpix_check = NULL;	// 원본 픽셀이 들어가는 곳을 체크하는 메모리.(x축)
	BYTE* ypix_check = NULL;	// 원본 픽셀이 들어가는 곳을 체크하는 메모리.(y축)

	//BYTE* pix_check; // 원본 픽셀이 들어가는 곳을 체크하는 배열.

	TIFF* input = TIFFOpen("images/input.tif", "rb");
	TIFF* output = TIFFOpen("images/output.tif", "wb");

	if (input == NULL)
	{
		printf("in File not found!!\n");
		//system("pause");
		exit(0);
	}

	if (output == NULL)
	{
		printf("out File not found!!\n");
		//system("pause");
		exit(0);
	}

	TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width_old);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height_old);
	//TIFFGetField(input, TIFFTAG_ICCPROFILE, &count, &icc_profile);
	TIFFGetField(input, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(input, TIFFTAG_YRESOLUTION, &yres);
	TIFFGetField(input, TIFFTAG_PHOTOMETRIC, &min_color);
	
	if (min_color > 1)
		min_color = 1;
	printf("min_color : %lf\n", min_color);

	rate_x = xres_new / (double)xres;
	rate_y = yres_new / (double)yres;

	xpix_new = (double)width_old * rate_x;
	ypix_new = (double)height_old * rate_y;

	/*
	if (xres_new >= xres)
	{
		rate_x = xres_new / (double)xres;
		xpix_new = (double)width_old * rate_x;
		if (yres_new >= yres)
		{
			rate_y = yres_new / (double)yres;
			ypix_new = (double)height_old * rate_y;
		}
		else if (yres_new < yres)
		{
			rate_y = (double)yres / yres_new;
			ypix_new = (double)height_old * (1 / rate_y);
		}
	}
	else if (xres_new < xres)
	{
		rate_x = (double)xres / xres_new;
		xpix_new = (double)width_old * (1 / rate_x);
		if (yres_new >= yres)
		{
			rate_y = yres_new / (double)yres;
			ypix_new = (double)height_old * rate_y;
		}
		else if (yres_new < yres)
		{
			rate_y = (double)yres / yres_new;
			ypix_new = (double)height_old * (1 / rate_y);
		}
	}
	*/
	//xpix_new = (double)width_old * rate_x;
	//ypix_new = (double)height_old * rate_y;

	//width_new = (int)xpix_new;
	//height_new = (int)ypix_new;
	
	if (xpix_new - (int)xpix_new >= 0.5)
		width_new = (int)xpix_new + 1;
	else
		width_new = (int)xpix_new;

	if (ypix_new - (int)ypix_new >= 0.5)
		height_new = (int)ypix_new + 1;
	else
		height_new = (int)ypix_new;
	
	printf("rate_x : %lf , rate_y : %lf\n", rate_x, rate_y);
	//printf("xpix_new - : %lf , ypix_new - : %lf\n", xpix_new - (int)xpix_new, ypix_new - (int)ypix_new);
	printf("xpix_new : %lf , ypix_new : %lf\n", xpix_new, ypix_new);
	printf("width_old : %d , height_old : %d\n", width_old, height_old);
	printf("width_new : %d , height_new : %d\n", width_new, height_new);

	TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width_new);
	TIFFSetField(output, TIFFTAG_IMAGELENGTH, height_new);
	//TIFFGetField(input, TIFFTAG_ICCPROFILE, &count, &icc_profile);
	TIFFSetField(output, TIFFTAG_XRESOLUTION, (float)xres_new);
	TIFFSetField(output, TIFFTAG_YRESOLUTION, (float)yres_new);
	TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(output, TIFFTAG_ROWSPERSTRIP, height_new);
	TIFFSetField(output, TIFFTAG_PHOTOMETRIC, min_color);
	TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	pix_old = (BYTE *)calloc(width_old * 2, sizeof(BYTE));	// 원본 이미지의 메모리, 2 line씩 읽으므로 X2 해준다.
	pix_new = (BYTE *)calloc(width_new, sizeof(BYTE));		// 리샘플링한 이미지의 메모리
	xpix_check = (BYTE *)calloc(width_new, sizeof(BYTE));	// 원본 픽셀이 들어가는 위치를 체크 (x축)
	ypix_check = (BYTE *)calloc(height_new, sizeof(BYTE));	// 원본 픽셀이 들어가는 위치를 체크 (y축)
	//pix_check = (BYTE *)calloc(width_new, sizeof(BYTE));	// 원본 픽셀이 들어가는 위치를 체크
	//pix_array = (BYTE *)calloc(width_new * ((int)rate_y + 2), sizeof(BYTE));	// 최대 2 line이 더 추가된다.

	// 소장님 알고리즘
#ifdef JWJI
	
	// x축 원본픽셀 자리 할당
	// 처음과 끝 지점엔 원본픽셀이 항상 들어간다.
	xpix_check[0] = 1;
	xpix_check[width_new - 1] = 1;

	for (int col = 1, wErr = 0; col < width_new - 1; col++)
	{
		wErr += (width_old - 1);
		if (wErr >= (width_new - 1))
		{
			xpix_check[col] = 1;
			wErr %= width_new;
		}
	}

	// y축 원본픽셀 자리 할당
	// 끝 지점엔 원본픽셀이 항상 들어간다.
	ypix_check[0] = 1;
	ypix_check[height_new - 1] = 1;

	for (int row = 1, hErr = 0; row < height_new - 1; row++)
	{
		hErr += (height_old - 1);
		if (hErr >= (height_new - 1))
		{
			ypix_check[row] = 1;
			hErr %= height_new;
		}
	}

#ifdef ERR
	//printf("xres : %d, yres : %d\n", resample_xcount, resample_ycount);
	for (int i = 0, yi = 0; i < height_new; i++)
	{
		if (ypix_check[i] != xpix_check[i])
		{
			printf("오류1");
		}
	}
	printf("\n");
#endif

	// 원본 픽셀이 들어가는 라인들 간격의 최댓 값
	int max_interval = 0;

	// xpix_check 값에 각각 원본픽셀 원소간의 간격 값을 넣어준다.
	for (int i = 0, xi = 0; i < width_new; i++)
	{
		if (xpix_check[i] != 0)
		{
			xpix_check[i] = i - xi;
			xi = i;
		}
	}

	// ypix_check 값에 각각 원본픽셀 라인간의 간격 값을 넣어준다.
	for (int i = 0, yi = 0; i < height_new; i++)
	{
		if (ypix_check[i] != 0)
		{
			ypix_check[i] = i - yi;
			yi = i;
			if (ypix_check[i] > max_interval)
				max_interval = ypix_check[i];
		}
	}

	// Bilinear 연산을 할 블록 단위 메모리 할당
	// 원본픽셀이 들어있는 라인간의 최대 간격만큼 메모리 할당
	pix_array = (BYTE *)calloc(width_new * (max_interval + 1), sizeof(BYTE));

	if (pix_array == NULL)
	{
		printf("memory alloc failed!!\n");
		exit(0);
	}

#ifdef ERR
	for (int i = 0, yi = 0; i < height_new; i++)
	{
		printf("%d, ", ypix_check[i]);
	}
	printf("끝\n");
#endif

	// Bilinear 시작
	// xi는 실제로 데이터가 들어가고 있는 위치 인덱스 
	int xi = 0;

	int line = 0; // file write 되고 있는 line 위치

	for (int row_new = 1, row_old = 0; row_new < height_new; row_new++)
	{
		// 원본 픽셀이 들어가는 자리 일 때 (y축)
		if (ypix_check[row_new] != 0)
		{
			// 원본파일에서 2라인 읽어들인다.
			TIFFReadScanline(input, pix_old, row_old, 0);
			TIFFReadScanline(input, pix_old + width_old, row_old + 1, 0);
			// 읽은 후에 원본이미지 라인 1 증가
			row_old++;

			// pix_array의 x축 연산 (맨 윗줄, 맨 아랫줄)
			xi = 0;
			for (int col_new = 0, col_old = 0; col_new < width_new; col_new++)
			{
				// 원본 픽셀이 들어가는 자리 일 때 (x축)
				if (xpix_check[col_new] != 0)
				{
					for (int t = 0; t < xpix_check[col_new]; t++)
					{
						pix_array[xi] =
							(pix_old[col_old + 1] - pix_old[col_old]) * t / xpix_check[col_new] + pix_old[col_old];
						pix_array[ypix_check[row_new] * width_new + xi] =
							(pix_old[width_old + col_old + 1] - pix_old[width_old + col_old]) * t / xpix_check[col_new] + pix_old[width_old + col_old];
						xi++;
					}
					col_old++;	// 원본 이미지 인덱스 증가
				}
			}
			// pix_array의 x축 마지막 픽셀 넣어준다
			pix_array[xi] = pix_old[width_old - 1];		// 맨 윗줄
			pix_array[ypix_check[row_new] * width_new + xi] = pix_old[width_old + width_old - 1];	// 맨 아랫줄

			// pix_array의 y축 연산
			for (int col = 0; col < width_new; col++)
			{
				for (int row = 0; row <= ypix_check[row_new]; row++)
				{
					pix_array[row * width_new + col] =
						(pix_array[ypix_check[row_new] * width_new + col] - pix_array[col])
						* row / ypix_check[row_new] + pix_array[col];
				}
			}

			// file write
			//printf("filewirte \n");
			for (int i = 0; i < ypix_check[row_new]; i++)
			{
				for (int col = 0; col < width_new; col++)
				{
					pix_new[col] = pix_array[i * width_new + col];
				}
				TIFFWriteScanline(output, pix_new, line, 0);
				line++;
			}

		}
	}
	// 마지막 line wirte
	for (int col = 0; col < width_new; col++)
	{
		pix_new[col] = pix_array[ypix_check[height_new - 1] * width_new + col];
	}
	TIFFWriteScanline(output, pix_new, line, 0);

#endif

	// 내 알고리즘
#ifdef MY
	/*
	// 끝지점이 항상 원본픽셀이 들어가기 때문에 개수에서 제외하고 비율을 계산해준다.
	double rate_x2 = (double)(width_new - 1.0) / (width_old - 1);
	double rate_y2 = (double)(height_new - 1.0) / (height_old - 1);

	// 에러값을 위해 비율의 소수점 부분을 정수화
	int decimal_x = roundf((rate_x2 - (int)rate_x2) * 100);
	int decimal_y = roundf((rate_y2 - (int)rate_y2) * 100);

	printf("decimal_x : %d, decimal_y : %d\n", decimal_x, decimal_y);

	// x축 원본 픽셀 자리 구해준다.
	// x축 인덱스
	int xindex = 0;

	// 처음과 끝 지점엔 원본픽셀이 항상 들어간다.
	xpix_check[0] = 1;
	xpix_check[width_new - 1] = 1;

	for (int col = 1, wErr = 0; col < width_old - 1; col++)
	{
		xindex += (int)rate_x2;

		wErr += decimal_x;
		if (wErr >= 100)
		{
			wErr -= 100;
			xindex++;
		}

		if (xindex >= width_new)
			break;
		xpix_check[xindex] = 1;
		
	}

	// y축 원본 픽셀 자리 구해준다.
	// y축 인덱스
	int yindex = 0;

	// 처음과 끝 지점엔 원본픽셀이 항상 들어간다.
	ypix_check[0] = 1;
	ypix_check[height_new - 1] = 1;

	for (int row = 1, hErr = 0; row < height_old - 1; row++)
	{
		yindex += (int)rate_y2;

		hErr += decimal_y;
		if (hErr >= 100)
		{
			hErr -= 100;
			yindex++;
		}

		if (yindex >= height_new)
			break;
		ypix_check[yindex] = 1;
	}
	*/

	// 원본 픽셀의 위치 잡는 알고리즘 > 내가 찾아내고 생각해낸거다!
	double xpix_rate = (double)(width_new - 1) / (width_old - 1);
	double ypix_rate = (double)(height_new - 1) / (height_old - 1);
	//printf("%lf %lf\n", xpix_rate, ypix_rate);

	if (xpix_rate >= 1.0)
	{
		// 가로로 Upscaling 세로로 Upscaling
		if (ypix_rate >= 1.0)
		{
			for (int col = 0; col < width_old; col++)
			{
				if ((col * xpix_rate) - (int)(col * xpix_rate) >= 0.5)
					xpix_check[(int)(col * xpix_rate) + 1] = 1;
				else
					xpix_check[(int)(col * xpix_rate)] = 1;
			}
			for (int row = 0; row < height_old; row++)
			{
				if ((row * ypix_rate) - (int)(row * ypix_rate) >= 0.5)
					ypix_check[(int)(row * ypix_rate) + 1] = 1;
				else
					ypix_check[(int)(row * ypix_rate)] = 1;
			}

#ifdef ERR
			//printf("xres : %d, yres : %d\n", resample_xcount, resample_ycount);
			for (int i = 0, yi = 0; i < height_new; i++)
			{
				if (ypix_check[i] != xpix_check[i])
				{
					printf("오류1");
				}
			}
			printf("\n");
#endif

			// 원본 픽셀이 들어가는 라인들 간격의 최댓 값
			int max_interval = 0;

			// xpix_check 값에 각각 원본픽셀 원소간의 간격 값을 넣어준다.
			for (int i = 0, xi = 0; i < width_new; i++)
			{
				if (xpix_check[i] != 0)
				{
					xpix_check[i] = i - xi;
					xi = i;
				}
			}

			// ypix_check 값에 각각 원본픽셀 라인간의 간격 값을 넣어준다.
			for (int i = 0, yi = 0; i < height_new; i++)
			{
				if (ypix_check[i] != 0)
				{
					ypix_check[i] = i - yi;
					yi = i;
					if (ypix_check[i] > max_interval)
						max_interval = ypix_check[i];
				}
			}

			// Bilinear 연산을 할 블록 단위 메모리 할당
			// 원본픽셀이 들어있는 라인간의 최대 간격만큼 메모리 할당
			pix_array = (BYTE *)calloc(width_new * (max_interval + 1), sizeof(BYTE));

			if (pix_array == NULL)
			{
				printf("memory alloc failed!!\n");
				exit(0);
			}

#ifdef ERR
			for (int i = 0, yi = 0; i < height_new; i++)
			{
				printf("%d, ", ypix_check[i]);
			}
			printf("끝\n");
#endif

			// Bilinear 시작
			// xi는 실제로 데이터가 들어가고 있는 위치 인덱스 
			int xi = 0;

			int line = 0; // file write 되고 있는 line 위치

			for (int row_new = 1, row_old = 0; row_new < height_new; row_new++)
			{
				// 원본 픽셀이 들어가는 자리 일 때 (y축)
				if (ypix_check[row_new] != 0)
				{
					// 원본파일에서 2라인 읽어들인다.
					TIFFReadScanline(input, pix_old, row_old, 0);
					TIFFReadScanline(input, pix_old + width_old, row_old + 1, 0);
					// 읽은 후에 원본이미지 라인 1 증가
					row_old++;

					// pix_array의 x축 연산 (맨 윗줄, 맨 아랫줄)
					xi = 0;
					for (int col_new = 0, col_old = 0; col_new < width_new; col_new++)
					{
						// 원본 픽셀이 들어가는 자리 일 때 (x축)
						if (xpix_check[col_new] != 0)
						{
							for (int t = 0; t < xpix_check[col_new]; t++)
							{
								pix_array[xi] =
									(pix_old[col_old + 1] - pix_old[col_old]) * t / xpix_check[col_new] + pix_old[col_old];
								pix_array[ypix_check[row_new] * width_new + xi] =
									(pix_old[width_old + col_old + 1] - pix_old[width_old + col_old]) * t / xpix_check[col_new] + pix_old[width_old + col_old];
								xi++;
							}
							col_old++;	// 원본 이미지 인덱스 증가
						}
					}
					// pix_array의 x축 마지막 픽셀 넣어준다
					pix_array[xi] = pix_old[width_old - 1];		// 맨 윗줄
					pix_array[ypix_check[row_new] * width_new + xi] = pix_old[width_old + width_old - 1];	// 맨 아랫줄

					// pix_array의 y축 연산
					for (int col = 0; col < width_new; col++)
					{
						for (int row = 0; row <= ypix_check[row_new]; row++)
						{
							pix_array[row * width_new + col] =
								(pix_array[ypix_check[row_new] * width_new + col] - pix_array[col])
								* row / ypix_check[row_new] + pix_array[col];
						}
					}

					// file write
					//printf("filewirte \n");
					for (int i = 0; i < ypix_check[row_new]; i++)
					{
						for (int col = 0; col < width_new; col++)
						{
							pix_new[col] = pix_array[i * width_new + col];
						}
						TIFFWriteScanline(output, pix_new, line, 0);
						line++;
					}

				}
			}
			// 마지막 line wirte
			for (int col = 0; col < width_new; col++)
			{
				pix_new[col] = pix_array[ypix_check[height_new - 1] * width_new + col];
			}
			TIFFWriteScanline(output, pix_new, line, 0);

		}
		// 가로로 Upscaling 세로로 Downscaling
		else
		{

		}
	}
	else
	{
		// 가로로 Downscaling 세로로 Upscaling
		if (ypix_rate >= 1.0)
		{

		}
		// 가로로 Downscaling 세로로 Downscaling
		else
		{

		}
	}


	// upscaling
	/*
	int decimal_x = roundf((rate_x - (int)rate_x) * 100);
	int decimal_y = roundf((rate_y - (int)rate_y) * 100);
	//double decimal_x = rate_x - (int)rate_x;
	//double decimal_y = rate_y - (int)rate_y;
	printf("decimal_x : %d, decimal_y : %d\n", decimal_x, decimal_y);

	int yrate = (int)rate_y;
	int line = 0;
	int s = 0;
	for (int row_old = 0, hErr = 0; row_old < height_old; row_old++)
	{
		// 원본파일에서 2라인 읽어들인다.
		TIFFReadScanline(input, pix_old, row_old, 0);
		if(row_old + 1 < height_old)
			TIFFReadScanline(input, pix_old + width_old, row_old + 1, 0);

		// pix_array의 y축들 bilinear 연산
		for (int col_old = 0, index = 0, wErr = 0; col_old < width_old; col_old++)
		{
			// index = 원본 픽셀이 들어갈 자리의 위치
			if (index < width_new)
			{
				pix_check[index] = 1;
				for (int row_new = 0; row_new <= yrate; row_new++)
				{
					pix_array[row_new * width_new + index] =
						(pix_old[col_old + width_old] - pix_old[col_old]) * row_new / yrate + pix_old[col_old];
				}

				if (wErr >= 100)
				{
					wErr -= 100;
					index += ((int)rate_x + 1);
				}
				else
				{
					index += (int)rate_x;
				}

				wErr += decimal_x;
			}
			else
				break;
		}
		
		// pix_array의 x축들 bilinear 연산
		for (int row_new = 0; row_new <= yrate; row_new++)
		{
			for (int col_new = 0, i = 0; col_new < width_new; col_new++)
			{
				// i = 실제로 데이터가 들어갈 곳의 인덱스
				// s = 현재 원본픽셀 값의 이전 픽셀 값이 들어있는 곳의 인덱스
				if (pix_check[col_new] != 0)
				{
					for (int col_tmp = 0; col_tmp < col_new - s; col_tmp++)
					{
						//pix_new[i] =
							//(pix_new[col_new] - pix_new[s]) * col_tmp / (col_new - s) + pix_new[s];

						pix_array[row_new * width_new + i] =
							(pix_array[row_new * width_new + col_new] - pix_array[row_new * width_new + s]) * col_tmp / (col_new - s) + pix_array[row_new * width_new + s];
						i++;
					}
					s = col_new;
				}
			}
			
			// 끝에 남은 메모리 부분을 원본 이미지의 마지막 픽셀 값(x축)으로 채워준다.
			for (int i = s; i < width_new; i++)
			{
				//pix_new[i] = pix_array[row_new * width_new + s];
				pix_array[row_new * width_new + i] = pix_array[row_new * width_new + s];
			}
			
		}
		
		// 계산이 끝난 pix_array 메모리를 파일에 쓴다.
		// TIFFWirteScanline
		for (int row = 0; row < yrate; row++)
		{
			// pix_array의 값들을 pix_new 에 옮기기
			for (int col = 0; col < width_new; col++)
			{
				pix_new[col] = pix_array[row * width_new + col];
			}
			//printf("%d\n", line);
			TIFFWriteScanline(output, pix_new, line, 0);
			// 파일 라인 수 1증가
			line++;
		}
		// 에러값 계산
		if (hErr >= 100)
		{
			hErr -= 100;
			yrate = (int)rate_y + 1;
		}
		else
		{
			yrate = (int)rate_y;
		}
		hErr += decimal_y;
	}
	// 끝에 남은 메모리 부분을 원본 이미지의 마지막 어레이 값(y축)으로 채워준다.
	for (int row = line; row < height_new; row++)
	{
		TIFFWriteScanline(output, pix_new, line, 0);
		// 파일 라인 수 1증가
		line++;
	}
	*/
	/*
	// upscaling
	int decimal_x = roundf((rate_x - (int)rate_x) * 100);
	int decimal_y = roundf((rate_y - (int)rate_y) * 100);
	//double decimal_x = rate_x - (int)rate_x;
	//double decimal_y = rate_y - (int)rate_y;
	printf("decimal_x : %d, decimal_y : %d\n", decimal_x, decimal_y);

	int yrate = (int)rate_y;
	int line = 0;
	int s = 0;
	for (int row_old = 0, hErr = 0; row_old < height_old -1; row_old++)
	{
		// 원본파일에서 2라인 읽어들인다.
		TIFFReadScanline(input, pix_old, row_old, 0);
		TIFFReadScanline(input, pix_old + width_old, row_old + 1, 0);

		// pix_array의 y축들 bilinear 연산
		for (int col_old = 0, index = 0, wErr = 0; col_old < width_old; col_old++)
		{
			// index = 원본 픽셀이 들어갈 자리의 위치
			if (index < width_new)
			{
				pix_check[index] = 1;
				for (int row_new = 0; row_new < yrate; row_new++)
				{
					pix_array[row_new * width_new + index] =
						(pix_old[col_old + width_old] - pix_old[col_old]) * row_new / yrate + pix_old[col_old];
				}
				if (wErr >= 100)
				{
					wErr -= 100;
					index += ((int)rate_x + 1);
				}
				else
				{
					index += (int)rate_x;
				}

				wErr += decimal_x;
			}
		}

		// pix_array의 x축들 bilinear 연산
		for (int row_new = 0; row_new < yrate; row_new++)
		{
			for (int col_new = 0; col_new < width_new; col_new++)
			{
				// 원본 픽셀 값이 들어있는 곳
				if (pix_check[col_new] != 0)
				{
					// 파일에 쓰기 위해 1라인씩 데이터를 계산하고 옮긴다.
					// 1라인 사이사이에 원본 픽셀 값을 넣어준다.
					pix_new[col_new] = pix_array[row_new * width_new + col_new];
				}
			}
			for (int col_new = 0, i = 0; col_new < width_new; col_new++)
			{
				// i = 실제로 데이터가 들어갈 곳의 인덱스
				// s = 현재 원본픽셀 값의 이전 픽셀 값이 들어있는 곳의 인덱스
				if (pix_check[col_new] != 0)
				{
					for (int col_tmp = 0; col_tmp < col_new - s; col_tmp++)
					{
						pix_new[i] =
							(pix_new[col_new] - pix_new[s]) * col_tmp / (col_new - s) + pix_new[s];

						i++;
					}
					s = col_new;
				}
			}
			// 끝에 남은 메모리 부분을 원본 이미지의 마지막 픽셀 값(x축)으로 채워준다.
			for (int i = s; i < width_new; i++)
			{
				pix_new[i] = pix_array[row_new * width_new + s];
			}

			// 계산이 끝난 pix_new 메모리를 파일에 쓴다.
			// TIFFWirteScanline
			TIFFWriteScanline(output, pix_new, line, 0);
			// 파일 라인 수 1증가
			line++;
		}

		if (hErr >= 100)
		{
			hErr -= 100;
			yrate = (int)rate_y + 1;
		}
		else
		{
			yrate = (int)rate_y;
		}
		hErr += decimal_y;
	}
	// 끝에 남은 메모리 부분을 원본 이미지의 마지막 어레이 값(y축)으로 채워준다.
	for (int col_new = 0, col_old = 0, i = 0; col_new < width_new; col_new++)
	{
		// i = 실제로 데이터가 들어갈 곳의 인덱스
		// s = 현재 원본픽셀 값의 이전 픽셀 값이 들어있는 곳의 인덱스
		if (pix_check[col_new] != 0)
		{
			pix_new[col_new] = pix_old[width_old + col_old];
			for (int col_tmp = 0; col_tmp < col_new - s; col_tmp++)
			{
				pix_new[i] =
					(pix_new[col_new] - pix_new[s]) * col_tmp / (col_new - s) + pix_new[s];

				i++;
			}
			s = col_new;
			col_old++;
		}
	}

	// 마지막 끝부분 메모리를 마지막 array 값으로 넣어준다.

	for (int i = line; i < height_new; i++)
	{
		TIFFWriteScanline(output, pix_new, i, 0);
	}
	*/
#endif
	TIFFClose(input);
	TIFFClose(output);
	free(pix_old);
	free(pix_new);
	free(xpix_check);
	free(ypix_check);
	//free(pix_check);
	free(pix_array);
}
#endif 

#ifdef BICUBIC
void Bicubic()
{

}
#endif