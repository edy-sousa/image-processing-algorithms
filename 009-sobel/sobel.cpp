//headers, librerias
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include<math.h>
using namespace std;
using namespace cv;

int main()
{
	//Imagenes originales
	const char* filenameD = "monalisa.jpg";
	Mat image = imread(filenameD, CV_LOAD_IMAGE_GRAYSCALE);
	const char* filenameA = "peppernoise.png";
	Mat pepperNoise = imread(filenameA, CV_LOAD_IMAGE_GRAYSCALE);
	//Mats resutado
	Mat blur(image.rows, image.cols, CV_8UC1);
	Mat diferencia(image.rows, image.cols, CV_8UC1);
	Mat filtroMediana(pepperNoise.rows, pepperNoise.cols, CV_8UC1);
	Mat sobelH(image.rows, image.cols, CV_32FC1);
	Mat sobelV(image.rows, image.cols, CV_32FC1);
	Mat raiz(image.rows, image.cols, CV_32FC1);
	Mat gaussiano2(image.rows, image.cols, CV_8UC1);
	Mat gaussiano3(image.rows, image.cols, CV_8UC1);
	int innerMatrixIndex = 3;
	int tamCampana = 5;
	int horiz[3][3] = { { -1,0,1 },{ -2,0,2 },{ -1,0,1 } };
	int vert[3][3] = { { -1,-2,-1 },{ 0,0,0 },{ 1,2,1 } };
	float suma, promedio;
	float gauss[25][25] = { 0 };

	int dim = 15;
	int x0 = dim / 2;
	int y0 = dim / 2;
	int sigma = 3;
	float pi = 3.1416;
	float sumaFiltro = 0;
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			int cX = i - x0;
			int cY = y0 - j;
			float up = (cX*cX) + (cY*cY);
			float down = 2 * (sigma * sigma);
			float exp1 = exp(-(up) / (down));
			float constant = 1.0 / (sigma*sigma * 2 * pi);
			gauss[i][j] = constant * exp1;
			sumaFiltro += constant * exp1;
		}
	}

	float suma4 = 0;
	int promedio4 = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			suma4 = 0;
			promedio4 = 0;
			int y4 = 0;
			for (int a = -(dim / 2); a <= dim / 2; a++)
			{
				int x4 = 0;
				for (int b = -(dim / 2); b <= dim / 2; b++)
				{
					////// suma
					if ((i + a) >= 0 &&
						(i + a) < image.rows &&
						(j + b) >= 0 &&
						(j + b) < image.cols)
					{
						suma4 += int(image.at<uchar>(i + a, j + b))*gauss[y4][x4];
					}
					x4++;
				}
				y4++;
			}
			promedio4 = int(suma4 / sumaFiltro);
			gaussiano3.at<uchar>(i, j) = promedio4;
		}
	}
	Mat cambios(image.rows, image.cols, CV_32FC1, Scalar(0));
	Mat acentuada(image.rows, image.cols, CV_8UC1, Scalar(0));
	//cambios bruscos (resta)
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			cambios.at<float>(i, j) = image.at<uchar>(i, j) - gaussiano3.at<uchar>(i, j);
		}
	}

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			
			if ((image.at<uchar>(i, j) + cambios.at<float>(i, j)) > 255) {
				acentuada.at<uchar>(i, j) = 255;
			}
			else {
				acentuada.at<uchar>(i, j) = image.at<uchar>(i, j) + cambios.at<float>(i, j);
			}
		}
	}

	//sobel horizontal
	int suma2 = 0;
	int promedio2 = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			suma2 = 0;
			promedio2 = 0;

			int y = 0;
			for (int a = -(innerMatrixIndex / 2); a <= innerMatrixIndex / 2; a++)
			{
				int x = 0;
				for (int b = -(innerMatrixIndex / 2); b <= innerMatrixIndex / 2; b++)
				{
					////// suma
					if ((i + a) >= 0 &&
						(i + a) < image.rows &&
						(j + b) >= 0 &&
						(j + b) < image.cols)
					{
						suma2 += (image.at<uchar>(i + a, j + b)*horiz[y][x]);
					}
					x++;
				}
				y++;
			}
			sobelH.at<float>(i, j) = (suma2)/255.0;
		}
	}

	//sobel vertical
	int suma3 = 0;
	int promedio3 = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			suma3 = 0;
			promedio3 = 0;

			int y = 0;
			for (int a = -(innerMatrixIndex / 2); a <= innerMatrixIndex / 2; a++)
			{
				int x = 0;
				for (int b = -(innerMatrixIndex / 2); b <= innerMatrixIndex / 2; b++)
				{
					////// suma
					if ((i + a) >= 0 &&
						(i + a) < image.rows &&
						(j + b) >= 0 &&
						(j + b) < image.cols)
					{
						suma3 += (image.at<uchar>(i + a, j + b)*vert[y][x]);
					}
					x++;
				}
				y++;
			}
			sobelV.at<float>(i, j) = (suma3)/255.0;
		}
	}

	//suma vectorial

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			float xValue = (sobelH.at<float>(i, j)*sobelH.at<float>(i, j));
			float yValue = (sobelV.at<float>(i, j)*sobelV.at<float>(i, j));
			raiz.at<float>(i, j) = (sqrt(xValue + yValue));
		}
	}

	namedWindow("orig", WINDOW_AUTOSIZE);
	imshow("orig", image);

	namedWindow("gauss sigma 3", WINDOW_AUTOSIZE);
	imshow("gauss sigma 3", gaussiano3);

	namedWindow("cambios", WINDOW_AUTOSIZE);
	imshow("cambios", cambios);

	namedWindow("acentuada", WINDOW_AUTOSIZE);
	imshow("acentuada", acentuada);

	namedWindow("sobel h", WINDOW_AUTOSIZE);
	imshow("sobel h", sobelH);

	namedWindow("sobel V", WINDOW_AUTOSIZE);
	imshow("sobel V", sobelV);

	namedWindow("raiz", WINDOW_AUTOSIZE);
	imshow("raiz", raiz);

	waitKey(0);
}