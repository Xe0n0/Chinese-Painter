#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <random>

#pragma comment(lib,"highgui.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"cv.lib")

#define margin 20
#define mean 0

int markov(int now, int max, int p_return, int p_inc, int p_dec)
{
	int seed = random() % 10;
	if (now == max || now == -max)
	{
		if (seed < p_return)
		{
			return -1;
		}
	}
	else {
		if (seed < p_inc)
		{
			return 1;
		}
		else if (seed < p_dec + p_inc)
		{
			return -1;
		}
	}
	return 0;
}

void randomSpot(IplImage* image, int num_spot, int len)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(len, 5);
	while (num_spot--)
	{
		int w_start = random() % (image->width - 2 * margin) + margin;
		int h_start = random() % (image->height - 2 * margin) + margin;
		int v = random() % 5;	

		double number = distribution(generator);

		int size = random() % 5 + 1;
		int range = 2 * size + 1;

		for (int i = 0; i < int(number); ++i)
		{
			w_start += size;
			// h_start += markov(h_start, 1, 9, 1, 1);

			for (int j = 0; j < range; ++j)
			{
				
				for (int k = 0; k < range; ++k)
				{
					int h = h_start - size + j;
					int w = w_start - size + k;
					if (h >= image->height || h < 0 || w >= image->width || w < 0)
					{
						continue;
					}
					uchar* ptr = &CV_IMAGE_ELEM(image, uchar, h_start - 1 + j, (w_start - 1 + k) * 3);

					ptr[0] = cvRound(fmin(v + ptr[0], 255));
					ptr[1] = cvRound(fmin(v + ptr[1], 255));
					ptr[2] = cvRound(fmin(v + ptr[2], 255));

				}
			}
			
		}
	}
}

static double distribution[20][margin];

void initial()
{
	// for (int i = 0; i < 20; ++i)
	// {
	// 	double *p = *(distribution + i);
	// 	p = new double[margin];
	// }
	for (int i = 0; i < 20; ++i)
	{
		double *p = *(distribution + i);

		const int nrolls = 1000000;  // number of experiments
		std::default_random_engine generator;
		std::normal_distribution<double> distribution(0, i);


		for (int j = 0; j < nrolls; ++j) {
			double number = distribution(generator);
			if ((number >= 0.0 ) && (number <= margin)) ++p[int(number)];
		}

		for (int j = 0; j < margin; ++j) {
			p[j] /= nrolls;
			// p[i] /= p[0];
			// p[i] = sqrt(p[i]);

			// printf("%f\n", p[i]);
		}
	}
	// for (int i = 0; i < 20; ++i)
	// {
	// 	double *p = distribution[i];

	// 	for (int j = 0; j < margin; ++j)
	// 	{
	// 		printf("%f\n", p[j]);
	// 	}
	// }
	printf("complete distribution generatign\n");
}

double cumulativeNormal(int x, int variance) {

	double *p = distribution[variance];

	return p[(x % margin)];
}

IplImage* calc_with_variance()
{
	IplImage* image = cvCreateImage(cvSize(512,64),IPL_DEPTH_8U,3);
	CvScalar start = CV_RGB(25, 25, 25);
	CvScalar end = CV_RGB(255, 255, 255);
	CvScalar bg = CV_RGB(255, 255, 255);

	cvSet(image, bg);
	double weightB = (end.val[0] - start.val[0]) / image->width;
	double weightG = (end.val[1] - start.val[1]) / image->width;
	double weightR = (end.val[2] - start.val[2]) / image->width;

	for(int i = 0; i < image->width; i++)
	{		
		for(int j = margin;j < image->height - margin; j++)
		{
			double dist = image->width - i;
			uchar* ptr = &CV_IMAGE_ELEM(image, uchar, j, i * 3);
			ptr[0] = cvRound(ptr[0] + weightB*dist);
			ptr[1] = cvRound(ptr[1] + weightG*dist);
			ptr[2] = cvRound(ptr[2] + weightR*dist); 
		}
	}
	// cvNot(image, image);
	return image;
}

void halo(IplImage *image, int variance)
{
	for (int i = 0; i < image->width; ++i)
	{
		uchar* origin = &CV_IMAGE_ELEM(image, uchar, margin, i * 3);
		for (int j = 0; j < margin; ++j)
		{
			double factor = cumulativeNormal(margin - j - 1, variance) / cumulativeNormal(0, variance);
			// printf("%f\n", factor);
			uchar* ptr = &CV_IMAGE_ELEM(image, uchar, j, i * 3);

			ptr[0] = cvRound(255 - (255 - origin[0]) * factor);
			ptr[1] = cvRound(255 - (255 - origin[1]) * factor);
			ptr[2] = cvRound(255 - (255 - origin[2]) * factor);
			// printf("%d %d\n", j, ptr[0]);
			ptr = &CV_IMAGE_ELEM(image, uchar, image->height - 1 - j, i * 3);

			ptr[0] = cvRound(255 - (255 - origin[0]) * factor);
			ptr[1] = cvRound(255 - (255 - origin[1]) * factor);
			ptr[2] = cvRound(255 - (255 - origin[2]) * factor);
		}
	}
}

int main(int argc, char** argv ) 
{
	initial();
	for (int j = 3; j < 5; j += 2)
	{
		for (int i = 5; i < 15; ++i)
		{
			for (int k = 4; k < 10; k += 2)
			{
				for (int l = 300; l < 500; l += 20)
				{
					IplImage *image = calc_with_variance();
					char filename[100];
					
					IplImage* image_out = cvCreateImage(cvSize(512,64),IPL_DEPTH_8U,3);
					randomSpot(image, k, l);

					halo(image, i);

					cvSmooth(image, image_out, CV_GAUSSIAN, 3, 3, j + 10);

					sprintf(filename, "./texture/ND_GB_RS_Marcov/texture_b%d_d%d_N%d_L%d.jpg", j, i, k, l);
					printf("generating %s\n", filename);
					cvSaveImage(filename, image_out);
					cvNamedWindow( "test", CV_WINDOW_AUTOSIZE);
					cvShowImage( "test", image_out);

					cvReleaseImage(&image);
				}
			}
			
		}
	}
	cvWaitKey();
	cvDestroyWindow("test");
	return 0;
}


