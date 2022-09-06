/*
  Eileen Chang
  
  Header for distance.cpp
*/
#ifndef DISTANCE_H
#define DISTANCE_H
#include <opencv2/opencv.hpp>

int ssd(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches);

int histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches);

int multi_histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches);

int texturecolor_histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches);


#endif