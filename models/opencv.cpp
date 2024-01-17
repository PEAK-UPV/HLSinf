#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include "opencv.h"
#include "main.h"

using namespace cv;
using namespace std;

EXTERN_C void fn_load_image_as_rgb(char *sz_file_name, int width, int height, int depth, void *ptr, int resize_, int crop, float mean0, float mean1, float mean2, float std0, float std1, float std2) {
    // load image
    //
    cv::Mat image_ocv = imread(sz_file_name, cv::IMREAD_COLOR);   // Read the file
    if(!image_ocv.data) {cout <<  "Could not open or find the image " << sz_file_name << " undef image will be used" << std::endl; return;}

    // resize image
    cv::Mat resizedImg;
    resize(image_ocv, resizedImg, Size(width, height), 0, 0);
    // split in three channels and convert to float
    std::vector<Mat> RGB;
    split(resizedImg, RGB);

    // crop image
    if (crop) {
      if (!no_warnings) printf("WARNING: Crop operation, not supported yet\n");
    }

    // copy to ptr (and at the same time apply avg normalization
    float *ptr_w = (float *)ptr;
    for (int r=0;r<RGB[0].rows;r++) {
        for (int c=0;c<RGB[0].cols;c++) {
            unsigned char pixel = RGB[0].at<unsigned char>(r, c);
	    float px = (float)pixel / 255.f;
            *ptr_w = (px - mean0)/std0  ;
            ptr_w += 1;
        }
    }
    for (int r=0;r<RGB[1].rows;r++) {
        for (int c=0;c<RGB[1].cols;c++) {
            unsigned char pixel = RGB[1].at<unsigned char>(r, c);
	    float px = (float)pixel / 255.f;
            *ptr_w = (px - mean1) / std1;
            ptr_w += 1;
        }
    }
    for (int r=0;r<RGB[2].rows;r++) {
        for (int c=0;c<RGB[2].cols;c++) {
            unsigned char pixel = RGB[2].at<unsigned char>(r, c);
	    float px = (float)pixel / 255.f;
            *ptr_w = (px - mean2) / std2;
            ptr_w += 1;
        }
    }

    image_ocv.release();
    resizedImg.release();
}
