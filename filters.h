/*
  Eileen Chang
  
  Header for filters.cpp
*/
#ifndef FILTERS_H
#define FILTERS_H
#include <opencv2/opencv.hpp>

// function declaration

int greyscale(cv::Mat &src, cv::Mat &dst);

int blur5x5(cv::Mat &src, cv::Mat &dst);

int sobelX3x3(cv::Mat &src, cv::Mat &dst);

int sobelY3x3(cv::Mat &src, cv::Mat &dst);

int magnitude(cv::Mat &sx, cv::Mat &sy, cv::Mat &dst);

int blurQuantize(cv::Mat &src, cv::Mat &dst, int levels);

int cartoon(cv::Mat &src, cv::Mat &dst, int levels, int magThreshold);

int invert(cv::Mat &src, cv::Mat &dst);

#endif