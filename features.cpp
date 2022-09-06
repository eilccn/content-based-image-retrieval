/*
  Eileen Chang
  
  Functions for calculating feature sets
  These functions are called via argv[4] in the command line:
  "b" will call the process_baseline function 
  "h" will call the histogram function
  "m" will call the multi_hist function
  "t" will call the texturecolor function
*/

#include "filters.h"
#include "features.h"
#include "csv_util.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

// compute 9x9 center image baseline feature set
int process_baseline(cv::Mat &img, std::vector<float> &fvec) {
    
    // obtain center 9x9 square of the image
    cv::Mat center;
    const int cropSize = 9;
    const int offsetW = (img.cols - cropSize) / 2;
    const int offsetH = (img.rows - cropSize) / 2;
    const Rect cropped(offsetW, offsetH, cropSize, cropSize);
    center = img(cropped).clone();


    // store features to feature vector
    for(int i=0; i<center.rows; i++) {
        for(int j=0; j<center.cols; j++) {
            for(int c=0; c<3; c++) {
                
                // push values into feature vector      
                float value = center.at<Vec3b>(i,j)[c];
                fvec.push_back(value);

            }
        }
    }
    
    return 0;
}

// compute rg histogram feature set
int histogram(cv::Mat &img, std::vector<float> &fvec) {
    const int Hsize = 32;
    int dim[2] = {Hsize, Hsize};
    cv::Mat hist2d;
    hist2d = cv::Mat::zeros(2, dim, CV_32S);
    int i, j, rx, ry;
    int num_bins = 16;
    float r, g, b;

    for(i=0; i<img.rows; i++) {
        for(j=0; j<img.cols; j++) {
            r = img.at<cv::Vec3b>(i, j)[0];
            g = img.at<cv::Vec3b>(i, j)[1];
            b = img.at<cv::Vec3b>(i, j)[2];

            rx = num_bins * r / (r + b + g + 1);
            ry = num_bins * g / (r + b + g + 1);
            
            hist2d.at<int>(rx, ry)++;
        }

    }

    for(i=0; i<hist2d.rows; i++) {
        for(j=0; j<hist2d.cols; j++) {
            
            fvec.push_back(hist2d.at<int>(i,j));
        }
    }

    return 0;
}

// compute multi histogram feature set
int multi_hist(cv::Mat &img, std::vector<float> &fvec) {
    // initialize vectors for each separate histogram
    std::vector<float> center_fvec;
    std::vector<float> whole_fvec;

    // obtain center 5x5 square of the image
    cv::Mat center;
    const int cropSize = 5;
    const int offsetW = (img.cols - cropSize) / 2;
    const int offsetH = (img.rows - cropSize) / 2;
    const Rect cropped(offsetW, offsetH, cropSize, cropSize);
    center = img(cropped).clone();

    // compute rg histogram feature set for center 9x9
    histogram(center, center_fvec);

    // compute rg histogram feature set for whole image
    histogram(img, whole_fvec);

    /* Concatenate the center and whole image vectors */
    // populate the concatenated vector into the fvec output
    center_fvec.insert(center_fvec.end(), whole_fvec.begin(), whole_fvec.end());

    for(auto& n : center_fvec) {
        fvec.push_back(n);
    }

    return 0;
}

// compute texture and color feature set 
// this function is called in the command line as argv[4] 
int texturecolor(cv::Mat &img, std::vector<float> &fvec) {
    /* TEXTURE */
    // apply sobel filters, gradient magnitude filter, and greyscale filter
    cv::Mat sx;
	sx.create(img.size(), CV_16SC3);
	sobelX3x3(img, sx);

	cv::Mat sy;
	sy.create(img.size(), CV_16SC3);
	sobelY3x3(img, sy);

	cv::Mat mag_img;
    cv::Mat converted_img;
	mag_img.create(img.size(), img.type());
	magnitude(sx, sy, mag_img);
	convertScaleAbs(mag_img, converted_img);

    // initialize vectors for gray magnitude histogram
    const int Hsize = 32;
    int dim[1] = {Hsize};
    cv::Mat hist1d;
    hist1d = cv::Mat::zeros(1, dim, CV_32S);
    int i, j, c;
    int num_bins = 16;
    std::vector<float> graymag_fvec;

    // convert gradient magnitude image to greyscale
    cv::Mat gray_img;
    img.copyTo(gray_img);
    cv::cvtColor(converted_img, gray_img, cv::COLOR_BGR2GRAY);

    // push magnitude values to histogram
    for(i=0; i<gray_img.rows; i++) {
        for(j=0; j<gray_img.cols; j++) {
            for(c=0; c<3; c++) {
                auto value = gray_img.at<uchar>(i,j);
                /*hist1d.at<int>(value)++;*/
                graymag_fvec.push_back(value);
            }
        }
    }

    /*
    for(i=0; i<hist1d.rows; i++) {
        for(j=0; j<hist1d.cols; j++) {
            graymag_fvec.push_back(hist1d.at<int>(i,j));
        }
    }
    */

    /* COLOR */
    std::vector<float> color_fvec;
    histogram(img, color_fvec);

    /* Concatenate the COLOR and GRAYMAG vectors */
    // populate the concatenated vector into the fvec output
    graymag_fvec.insert(graymag_fvec.end(), color_fvec.begin(), color_fvec.end());

    for(auto& n : graymag_fvec) {
        fvec.push_back(n);
    }

    /*
    // Print out the vector
    std::cout << "v = { ";

    for (auto x : fvec) {
        std::cout << x << ", ";
    }
    std::cout << "}; \n";
    */
    
    return 0;
}

// compute custom feature set: green images
/*
This function collects only the green values from the image
as a 1d cv::Mat histogram
*/
int green(cv::Mat &img, std::vector<float> &fvec) {
    const int Hsize = 32;
    int dim[1] = {Hsize};
    cv::Mat hist1d;
    hist1d = cv::Mat::zeros(1, dim, CV_32S);
    int i, j, gx;
    int num_bins = 16;
    float r, g, b;

    for(i=0; i<img.rows; i++) {
        for(j=0; j<img.cols; j++) {
            r = img.at<cv::Vec3b>(i, j)[0];
            g = img.at<cv::Vec3b>(i, j)[1];
            b = img.at<cv::Vec3b>(i, j)[2];

            gx = num_bins * g / (r + b + g + 1);
            
            hist1d.at<int>(gx)++;
        }

    }

    for(i=0; i<hist1d.rows; i++) {
        for(j=0; j<hist1d.cols; j++) {
            
            fvec.push_back(hist1d.at<int>(i,j));
        }
    }

    return 0;
}