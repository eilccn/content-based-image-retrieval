/*
  Eileen Chang
  
  Header for features.cpp
*/
#ifndef FEATURES_H
#define FEATURES_H
#include <opencv2/opencv.hpp>

int process_baseline(cv::Mat &img, std::vector<float> &fvec);

int histogram(cv::Mat &img, std::vector<float> &fvec);

int multi_hist(cv::Mat &img, std::vector<float> &fvec);

int texturecolor(cv::Mat &img, std::vector<float> &fvec);

int green(cv::Mat &img, std::vector<float> &fvec);

#endif