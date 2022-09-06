/*
  Eileen Chang
  
  Functions for calculating distance metrics
  These functions are called via argv[5] in the command line:
  "ssd" calls the ssd function
  "histx" calls the histx function
  "multix" calls the multi_histx function
  "tcx" calls the texturecolor_histx function

*/
#include "csv_util.h"
#include "directory.h"
#include "features.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <numeric>
#include <climits>
#include "outputfile.csv"

using namespace std;
using namespace cv;

// sum squared difference
int ssd(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches) {

    // initialize struct for image filename & ssd value pairs
    struct ImageStruct {
        std::string img_name;
        float ssd_value;
    };

    std::vector<ImageStruct> img_ssd; // vector for pairs
    ImageStruct pair; 

    // ssd computation
    for(int i=0; i<dir_filenames.size(); i++) { 
        float ssd = 0;
        for (int j=0; j<target_data.size(); j++) {
            ssd += ( (target_data[j] - dir_fvec[i][j]) * (target_data[j] - dir_fvec[i][j]) );
        }

        // push ssd & image filename as pairs to a vector
        pair.ssd_value = ssd;
        pair.img_name = dir_filenames[i];
        img_ssd.push_back(pair);
    }
    
    // sort ssd values from min to max
    sort(img_ssd.begin(), img_ssd.end(), [](const ImageStruct& a, const ImageStruct& b) {
        return a.ssd_value < b.ssd_value;
    });
    
    // return N matches
    int N = atoi(num_matches); // convert argv[6] (N number of matches) from char to integer
    img_ssd.resize(N);

    std::cout << "*********************************" << std::endl;
    std::cout << "The top " <<  N << " matches are:" << std::endl;  
    std::cout << "*********************************" << std::endl;
    for( auto& n : img_ssd) {
        std::cout << n.img_name << ": " << std::fixed << n.ssd_value << std::endl;
    }

    return 0;
}


// histogram intersection
int histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches) {

    // initialize variables
    float intersection;
    double target_sum;
    double dir_sum;

    // initialize variables for normalized histogram
    float normalized_target;
    float normalized_dir;

    // initialize vector for min values
    std::vector<float> all_min; 

    // initialize struct for image filename & value pairs
    struct ImageStruct {
        char *img_name;
        float value;
    };

    std::vector<ImageStruct> img_value;
    ImageStruct pair;

    // compute target image data sum
    target_sum = std::accumulate(target_data.begin(), target_data.end(), 0);

    // find the min, normalize min image histograms, and populate values to vector
    for(int i=0; i<dir_filenames.size(); i++ ) {

        dir_sum = std::accumulate(dir_fvec[i].begin(), dir_fvec[i].end(), 0);

        float min_sum = 0;
        for(int j=0; j<target_data.size(); j++) {
            // normalize target and directory image values
            normalized_target = target_data[j] / target_sum;
            normalized_dir = dir_fvec[i][j] / dir_sum;

            // compute the sum of all normalized minimum values 
            min_sum += std::min(normalized_target, normalized_dir);
            
        }
        
        // compute the histogram intersection 
        intersection = abs(1 - min_sum); 

        // push intersection values & image filename as pairs to a vector
        pair.img_name = dir_filenames[i];
        pair.value = intersection;
        img_value.push_back(pair);
    }

    // sort ssd values from min to max
    sort(img_value.begin(), img_value.end(), [](const ImageStruct& a, const ImageStruct& b) {
        return a.value < b.value;
    });
    
    // return N matches
    int N = atoi(num_matches); // convert argv[6] (N number of matches) from char to integer
    img_value.resize(N);

    std::cout << "*********************************" << std::endl;
    std::cout << "The top " <<  N << " matches are:" << std::endl;  
    std::cout << "*********************************" << std::endl;
    for( auto& n : img_value) {
        std::cout << n.img_name << ": " << std::fixed << n.value << std::endl;
    }

    return 0;
}

// multi-histogram intersection (with weighted averaging)
/* 
This function passes in feature set data as a concatenation of two feature sets: 
(1) rg chromaticity histogram for center 5x5 crop of the image
(2) rg chromaticity histogram for the whole image

It was noted for weighted averaging that via the above concatenation, 
the first half (1024 values) in the features set are for the center crop rg histogram
and the next half (1024 values) are for the whole image rg histogram. 
*/
int multi_histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches) {

    // initialize variables
    float intersection;
    double centerdir_sum;
    double wholedir_sum;

    // initialize variables for normalized histogram
    float normalized_target;
    float normalized_dir;

    // initialize vector for min values
    std::vector<float> all_min; 

    // initialize struct for image filename & value pairs
    struct ImageStruct {
        char *img_name;
        float value;
    };

    std::vector<ImageStruct> img_value;
    ImageStruct center_pair;
    ImageStruct whole_pair;

    /*** center crop histogram histogram intersection ***/
    // compute target image data sum for center crop histogram (first 1024 values)
    float centertarget_sum = 0;
    for(int i=0; i<(target_data.size() / 2); i++) {
        centertarget_sum += target_data[i];
    }

    // find the min, normalize min image histograms, and populate values to vector
    for(int i=0; i<(dir_filenames.size() / 2); i++ ) {

        centerdir_sum = std::accumulate(dir_fvec[i].begin(), dir_fvec[i].end(), 0);

        float centermin_sum = 0;
        for(int j=0; j<(target_data.size() / 2); j++) {
            // normalize target and directory image values
            normalized_target = target_data[j] / centertarget_sum;
            normalized_dir = dir_fvec[i][j] / centerdir_sum;

            // compute the sum of all normalized minimum values 
            centermin_sum += std::min(normalized_target, normalized_dir);
            
        }
        
        // coefficient for increasing weight of center image histogram
        float coeff = 0.7;
        // compute the histogram intersection 
        intersection = abs(1 - centermin_sum); 

        // push intersection values & image filename as pairs to a vector
        center_pair.img_name = dir_filenames[i];
        center_pair.value = intersection;
        img_value.push_back(center_pair);
    }

    /*** whole image histogram histogram intersection ***/
    // compute target image data sum for center crop histogram (first 1024 values)
    float wholetarget_sum = 0;
    for(int i=0; i<(target_data.size() / 2); i++) {
        wholetarget_sum += target_data[i];
    }

    // find the min, normalize min image histograms, and populate values to vector
    for(int i=0; i<(dir_filenames.size() / 2); i++ ) {

        wholedir_sum = std::accumulate(dir_fvec[i].begin(), dir_fvec[i].end(), 0);

        float wholemin_sum = 0;
        for(int j=0; j<(target_data.size() / 2); j++) {
            // normalize target and directory image values
            normalized_target = target_data[j] / wholetarget_sum;
            normalized_dir = dir_fvec[i][j] / wholedir_sum;

            // compute the sum of all normalized minimum values 
            wholemin_sum += std::min(normalized_target, normalized_dir);
            
        }
        
        // coefficient for decreasing weight of whole image histogram
        float coeff = 0.3;
        // compute the histogram intersection 
        intersection = coeff * abs(1 - wholemin_sum); 

        // push intersection values & image filename as pairs to a vector
        whole_pair.img_name = dir_filenames[i];
        whole_pair.value = intersection;
        img_value.push_back(whole_pair);
    }

    // sort ssd values from min to max
    sort(img_value.begin(), img_value.end(), [](const ImageStruct& a, const ImageStruct& b) {
        return a.value < b.value;
    });
    
    // return N matches
    int N = atoi(num_matches); // convert argv[6] (N number of matches) from char to integer
    img_value.resize(N);

    std::cout << "*********************************" << std::endl;
    std::cout << "The top " <<  N << " matches are:" << std::endl;  
    std::cout << "*********************************" << std::endl;
    for( auto& n : img_value) {
        std::cout << n.img_name << ": " << std::fixed << n.value << std::endl;
    }

    return 0;
}

// texture and color intersection (with weighted averaging)
/* 
This function passes in feature set data as a concatenation of two feature sets: 
(1) rg chromaticity histogram for the whole image in grayscale gradient magnitude
(2) the rg chromaticity for the whole image

It was noted for equal weighted averaging that via the above concatenation, 
the first half (1024 values) in the features set are for the gray magnitude image 
and the next half (1024 values) are for the whole image rg histogram. 
*/
int texturecolor_histx(std::vector<float> &target_data, std::vector<char *> dir_filenames, std::vector<std::vector<float>> &dir_fvec, char *num_matches) {

    // initialize variables
    float intersection;
    double gmdir_sum;
    double rgdir_sum;

    // initialize variables for normalized histogram
    float normalized_target;
    float normalized_dir;

    // initialize vector for min values
    std::vector<float> all_min; 

    // initialize struct for image filename & value pairs
    struct ImageStruct {
        char *img_name;
        float value;
    };

    std::vector<ImageStruct> img_value;
    ImageStruct gm_pair;
    ImageStruct rg_pair;

    /*** center crop histogram histogram intersection ***/
    // compute target image data sum for center crop histogram (first 1024 values)
    float gmtarget_sum = 0;
    for(int i=0; i<(target_data.size() / 2); i++) {
        gmtarget_sum += target_data[i];
    }

    // find the min, normalize min image histograms, and populate values to vector
    for(int i=0; i<(dir_filenames.size() / 2); i++ ) {

        gmdir_sum = std::accumulate(dir_fvec[i].begin(), dir_fvec[i].end(), 0);

        float gm_minsum = 0;
        for(int j=0; j<(target_data.size() / 2); j++) {
            // normalize target and directory image values
            normalized_target = target_data[j] / gmtarget_sum;
            normalized_dir = dir_fvec[i][j] / gmdir_sum;

            // compute the sum of all normalized minimum values 
            gm_minsum += std::min(normalized_target, normalized_dir);
            
        }
        
        // coefficient for increasing weight of center image histogram
        float coeff = 0.5;
        // compute the histogram intersection 
        intersection = abs(1 - gm_minsum); 

        // push intersection values & image filename as pairs to a vector
        gm_pair.img_name = dir_filenames[i];
        gm_pair.value = intersection;
        img_value.push_back(gm_pair);
    }

    /*** whole image histogram histogram intersection ***/
    // compute target image data sum for center crop histogram (first 1024 values)
    float rgtarget_sum = 0;
    for(int i=0; i<(target_data.size() / 2); i++) {
        rgtarget_sum += target_data[i];
    }

    // find the min, normalize min image histograms, and populate values to vector
    for(int i=0; i<(dir_filenames.size() / 2); i++ ) {

        rgdir_sum = std::accumulate(dir_fvec[i].begin(), dir_fvec[i].end(), 0);

        float rg_minsum = 0;
        for(int j=0; j<(target_data.size() / 2); j++) {
            // normalize target and directory image values
            normalized_target = target_data[j] / rgtarget_sum;
            normalized_dir = dir_fvec[i][j] / rgdir_sum;

            // compute the sum of all normalized minimum values 
            rg_minsum += std::min(normalized_target, normalized_dir);
            
        }
        
        // coefficient for decreasing weight of whole image histogram
        float coeff = 0.5;
        // compute the histogram intersection 
        intersection = coeff * abs(1 - rg_minsum); 

        // push intersection values & image filename as pairs to a vector
        rg_pair.img_name = dir_filenames[i];
        rg_pair.value = intersection;
        img_value.push_back(rg_pair);
    }

    // sort ssd values from min to max
    sort(img_value.begin(), img_value.end(), [](const ImageStruct& a, const ImageStruct& b) {
        return a.value < b.value;
    });
    
    // return N matches
    int N = atoi(num_matches); // convert argv[6] (N number of matches) from char to integer
    img_value.resize(N);

    std::cout << "*********************************" << std::endl;
    std::cout << "The top " <<  N << " matches are:" << std::endl;  
    std::cout << "*********************************" << std::endl;
    for( auto& n : img_value) {
        std::cout << n.img_name << ": " << std::fixed << n.value << std::endl;
    }

    return 0;
}
