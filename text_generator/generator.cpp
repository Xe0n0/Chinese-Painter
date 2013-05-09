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
#define variance 8.0
#define range 11
#define mean 0

double cumulativeNormal(int x) {
	static bool dispatch = false;
	static double p[margin] = {};

	if (!dispatch)
	{
		const int nrolls = 1000000;  // number of experiments

		std::default_random_engine generator;
		std::normal_distribution<double> distribution(0, variance);


		for (int i = 0; i < nrolls; ++i) {
			double number = distribution(generator);
			if ((number >= 0.0 ) && (number <= margin)) ++p[int(number)];
		}

		for (int i = 0; i < margin; ++i) {
			p[i] /= nrolls;
			// p[i] /= p[0];
			// p[i] = sqrt(p[i]);

			printf("%f\n", p[i]);
		}
		dispatch = true;
	}
	return p[x % margin];
}


int main( int argc, char** argv ) 
{
	IplImage* image = cvCreateImage(cvSize(512,64),IPL_DEPTH_8U,3);
	if(!image) 
	{
		return -1;
	}
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

	for (int i = 0; i < image->width; ++i)
	{
		uchar* origin = &CV_IMAGE_ELEM(image, uchar, margin, i * 3);
		for (int j = 0; j < margin; ++j)
		{
			double factor = cumulativeNormal(margin - j - 1) / cumulativeNormal(0);
			printf("%f\n", factor);
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
	// cvNot(image, image);

	cvSaveImage( "radial.jpg", image );
	cvNamedWindow( "test", 1 );
	cvShowImage( "test", image );
	cvWaitKey();
	cvDestroyWindow("test");
	cvReleaseImage(&image);
	return 0; 
}


