#include "common.h"
#include "hiddenSystem.h"

using namespace cv;
using namespace std;

#define POINTSIZE 40

int h_upper = 30, h_lower = 0;
int s_upper = 255, s_lower = 50;
int v_upper = 255, v_lower = 0;
cv::Mat img_hsv, img, img_gray, img_gray_th, img_bin, img_lbl, img_dst, img_rgb_th;
cv::Mat element8 = (cv::Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1); // 8近傍

const unsigned int channelNum = 4; // RGBなら3, RGBAなら4

int **bin;
int **red;
int **green;
int **blue;

extern int xsize, ysize;

// 隠面処理しきい値
extern float green_thr;

void hiddenSystem::InitCV(void) {

	bin = (int**)malloc(sizeof(int *) * xsize);
	red = (int**)malloc(sizeof(int *) * xsize);
	green = (int**)malloc(sizeof(int *) * xsize);
	blue = (int**)malloc(sizeof(int *) * xsize);
	for (int i = 0; i < xsize; i++) {
		bin[i] = (int*)malloc(sizeof(int *) * ysize);
		red[i] = (int*)malloc(sizeof(int *) * ysize);
		green[i] = (int*)malloc(sizeof(int *) * ysize);
		blue[i] = (int*)malloc(sizeof(int *) * ysize);
	}

}

void hiddenSystem::CleanCV(void) {
	for (int i = 0; i < xsize; i++) {
		free(red[i]);
		free(green[i]);
		free(blue[i]);
		free(bin[i]);
	}
	free(red);
	free(green);
	free(blue);
	free(bin);
}


void hiddenSystem::IplToMat(GLubyte* p, cv::Mat *mat, IplImage* outImage) {

	for (unsigned int j = 0; j < ysize; ++j)
	{
		for (unsigned int i = 0; i < xsize; ++i)
		{
			outImage->imageData[(ysize - j - 1) * outImage->widthStep + i * 4 + 0] = *(p + 2);
			outImage->imageData[(ysize - j - 1) * outImage->widthStep + i * 4 + 1] = *(p + 1);
			outImage->imageData[(ysize - j - 1) * outImage->widthStep + i * 4 + 2] = *(p);
			outImage->imageData[(ysize - j - 1) * outImage->widthStep + i * 4 + 3] = *(p + 3);
			p += 4;
		}
	}
	*mat = cv::cvarrToMat(outImage);

}

void hiddenSystem::HandDepthExtraction(void) {

	if (finger2_y[0] < green_thr && finger2_y[1] < green_thr) {  // すべての指 

		for (int i = 0; i < 5; i++) {
			glColor3f(0, 1.0, 0);
			glPointSize(POINTSIZE + 10);
			glBegin(GL_POINTS);
			glVertex3f(finger2_x[i] - 2, finger2_y[i], finger2_z[i] + 1);
			glVertex3f(distal_x[i], distal_y[i], distal_z[i]);
			glVertex3f(intermediate_x[i], intermediate_y[i], intermediate_z[i]);
			glVertex3f(proximal_x[i], proximal_y[i], proximal_z[i]);
			glEnd();
			glPointSize(POINTSIZE + 70);
			glBegin(GL_POINTS);

			if (i != 4) {
				glVertex3f(metacarpal_x[i], metacarpal_y[i], metacarpal_z[i]);
			}

			glEnd();
		}
	}
	else if (finger2_y[0] < green_thr + 2 && finger2_y[1] > green_thr - 2) {  //親指
		glColor3f(0, 1.0, 0);
		glPointSize(POINTSIZE - 2);
		glBegin(GL_POINTS);
		glVertex3f(2 * finger2_x[0] - (finger2_x[0] + intermediate_x[0]) / 2 - 2, finger2_y[0], finger2_z[0] - 3);
		glVertex3f(2 * finger2_x[0] - (finger2_x[0] + distal_x[0]) / 2, finger2_y[0], finger2_z[0] - 5);
		glEnd();
		glPointSize(POINTSIZE);
		glBegin(GL_POINTS);
		glVertex3f(finger2_x[0], finger2_y[0], finger2_z[0] - 5);
		glVertex3f((finger2_x[0] + distal_x[0]) / 2, (finger2_y[0] + distal_y[0]) / 2, (finger2_z[0] + distal_z[0]) / 2 - 4);
		glEnd();
		glPointSize(POINTSIZE + 3);
		glBegin(GL_POINTS);
		glVertex3f(distal_x[0], distal_y[0], distal_z[0] - 5);
		glVertex3f((distal_x[0] + intermediate_x[0]) / 2, (distal_y[0] + intermediate_y[0]) / 2, (distal_z[0] + intermediate_z[0]) / 2 - 2);
		glEnd();
		glPointSize(POINTSIZE + 10);
		glBegin(GL_POINTS);
		glVertex3f(intermediate_x[0], intermediate_y[0], intermediate_z[0] - 5);
		glVertex3f((intermediate_x[0] + proximal_x[0]) / 2, (intermediate_y[0] + proximal_y[0]) / 2, (intermediate_z[0] + proximal_z[0]) / 2 - 1);
		glEnd();
		glPointSize(POINTSIZE + 23);
		glBegin(GL_POINTS);
		glVertex3f(proximal_x[0], proximal_y[0], proximal_z[0]);
		glEnd();
	}

}


void hiddenSystem::colorExtraction(cv::Mat* src, cv::Mat* dst,
	int code,
	int ch1Lower, int ch1Upper,
	int ch2Lower, int ch2Upper,
	int ch3Lower, int ch3Upper
)
{
	cv::Mat colorImage;
	int lower[3];
	int upper[3];

	cv::Mat lut = cv::Mat(256, 1, CV_8UC3);

	cv::cvtColor(*src, colorImage, code);

	lower[0] = ch1Lower;
	lower[1] = ch2Lower;
	lower[2] = ch3Lower;

	upper[0] = ch1Upper;
	upper[1] = ch2Upper;
	upper[2] = ch3Upper;

	for (int i = 0; i < 256; i++) {
		for (int k = 0; k < 3; k++) {
			if (lower[k] <= upper[k]) {
				if ((lower[k] <= i) && (i <= upper[k])) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
			else {
				if ((i <= upper[k]) || (lower[k] <= i)) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
		}
	}

	//LUTを使用して二値化
	cv::LUT(colorImage, lut, colorImage);

	//Channel毎に分解
	std::vector<cv::Mat> planes;
	cv::split(colorImage, planes);

	//マスクを作成
	cv::Mat maskImage;
	cv::bitwise_and(planes[0], planes[1], maskImage);
	cv::bitwise_and(maskImage, planes[2], maskImage);

	//出力
	cv::Mat maskedImage;
	src->copyTo(maskedImage, maskImage);
	*dst = maskedImage;

}


void hiddenSystem::HiddenPreShow() {
	void* dataBuf = NULL;
	dataBuf = (GLubyte*)malloc(xsize * ysize * channelNum);

	glReadBuffer(GL_BACK);

	glReadPixels(0, 0, xsize, ysize, GL_RGBA, GL_UNSIGNED_BYTE, dataBuf);
	GLubyte* p = static_cast<GLubyte*>(dataBuf);

	IplImage* outImage = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);

	cv::Mat mat1;
	IplToMat(p, &mat1, outImage);

	for (int i = 0; i < xsize; ++i) {
		for (int j = 0; j < ysize; ++j) {
			if (bin[i][j] == 0) {
				red[i][j] = mat1.at<Vec4b>(j, i)[2];
				green[i][j] = mat1.at<Vec4b>(j, i)[1];
				blue[i][j] = mat1.at<Vec4b>(j, i)[0];
			}
		}
	}
	cv::Mat mat2(ysize, xsize, CV_8UC4);
	for (int j = 0; j < xsize; j++) {
		for (int k = 0; k < ysize; k++) {

			mat2.at<Vec4b>(k, j)[0] = blue[j][k];
			mat2.at<Vec4b>(k, j)[1] = green[j][k];
			mat2.at<Vec4b>(k, j)[2] = red[j][k];
			mat2.at<Vec4b>(k, j)[3] = 0;
		}
	}
	cv::imshow("Hand&Model", mat1);
	cv::imshow("normal", mat2);
	cvReleaseImage(&outImage);
	free(dataBuf);

	HandDepthExtraction();

}

void hiddenSystem::HiddenResult() {
	void* dataBuf = NULL;
	dataBuf = (GLubyte*)malloc(xsize * ysize * channelNum);

	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, xsize, ysize, GL_RGBA, GL_UNSIGNED_BYTE, dataBuf);
	GLubyte* p = static_cast<GLubyte*>(dataBuf);

	IplImage* outImage = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);
	cv::Mat mat;
	IplToMat(p, &mat, outImage);

	for (int j = 0; j < xsize; j++) {
		for (int k = 0; k < ysize; k++) {
			int f_blue = mat.at<Vec4b>(k, j)[0];
			int f_green = mat.at<Vec4b>(k, j)[1];
			int f_red = mat.at<Vec4b>(k, j)[2];
			if (f_blue == 0 && f_green == 255 && f_red == 0) {
				mat.at<Vec4b>(k, j)[0] = blue[j][k];
				mat.at<Vec4b>(k, j)[1] = green[j][k];
				mat.at<Vec4b>(k, j)[2] = red[j][k];
				mat.at<Vec4b>(k, j)[3] = 0;
			}
		}
	}

	resize(mat, mat, cv::Size(), 2, 2);
	cv::imshow("result", mat);
	cvReleaseImage(&outImage);
	free(dataBuf);
}