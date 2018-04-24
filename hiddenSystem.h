#ifndef HIDDEN_SYSTEM_H
#define HIDDEN_SYSTEM_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>


extern int h_upper, h_lower;
extern int s_upper, s_lower;
extern int v_upper, v_lower;
extern cv::Mat img_hsv, img, img_gray, img_gray_th, img_bin, img_lbl, img_dst, img_rgb_th;
extern cv::Mat element8; // 8‹ß–T

extern const unsigned int channelNum; // RGB‚È‚ç3, RGBA‚È‚ç4

extern int **bin;
extern int **red;
extern int **green;
extern int **blue;

class hiddenSystem {

public:
	void InitCV();
	void CleanCV();
	void colorExtraction(cv::Mat* src, cv::Mat* dst, int code, int ch1Lower, int ch1Upper, int ch2Lower, int ch2Upper, int ch3Lower, int ch3Upper);
	void IplToMat(GLubyte* p, cv::Mat *mat, IplImage* outImage);
	void HandDepthExtraction(void);
	void HiddenPreShow(void);
	void HiddenResult(void);
};

#endif