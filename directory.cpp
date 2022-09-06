/*
  Eileen Chang
  
  Code to read image files in a directory, compute their features, write their features to a csv file, then read the feature set froom the written csv file
  Initializes feature set computation function corresponding to a command line argument passed in as a parameter
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
#include "outputfile.csv"

using namespace std;
using namespace cv;

/*
  Given a directory on the command line, scans through the directory for image files.
  Reads the full path name for each file, computes each image's features, writes those features to a csv file, then reads those features from the csv file
  Initializes feature set computation function corresponding to a command line argument passed in as a parameter
 */
int process_directory(char *dir, char *csv, char *featuretype) {
    // initialize variables for reading directory
    char dirname[256];
    char buffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int i;

    // get the directory path
    strcpy(dirname, dir);
    printf("Processing directory %s\n", dirname );

    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }

    // loop over all the files in the image file listing
    while( (dp = readdir(dirp)) != NULL ) {

        // check if the file is an image
        if( strstr(dp->d_name, ".jpg") ||
        strstr(dp->d_name, ".png") ||
        strstr(dp->d_name, ".ppm") ||
        strstr(dp->d_name, ".tif") ) {

            // printf("processing image file: %s\n", dp->d_name);

            // build the overall filename
            strcpy(buffer, dirname);
            strcat(buffer, "/");
            strcat(buffer, dp->d_name);

            // read each image
            cv::Mat img;
            img = cv::imread(buffer, cv::IMREAD_COLOR);
            if( img.data == NULL ) {
                printf("Unable to read file %s, skipping\n", buffer );
                continue;
            }

            // COMPUTE FEATURE VECTORS
            // initialize varaiables for command line arguments passed in as parameters
            char outputfile[256]; // csv file
            char ft[256]; // feature type
            strcpy(ft, featuretype);
            std::vector<float> fvec; // vector for image data

            // if-else ladder for feature type computation based on command line argument
            if (strcmp(ft, "b") == 0) {
                // compute 9x9 center feature vector
                process_baseline(img, fvec);
            }
            else if (strcmp(ft, "h") == 0) {
                // compute normalized histogram feature vector
                histogram(img, fvec);
            }
            else if (strcmp(ft, "m") == 0) {
                // compute multihistogram feature vector
                multi_hist(img, fvec);
            }
            else if (strcmp(ft, "t") == 0) {
                // compute texture color feature vector
                texturecolor(img, fvec);
            }
            /*
            else if (strcmp(ft, "g") == 0) {
                // compute green histogram feature vector
                groundsky(img, fvec);
            }
            */
            else if (strcmp(ft, "gr") == 0) {
                green(img, fvec);
            }
            
            // write feature set to new csv file
            strcpy(outputfile, csv);
            append_image_data_csv(outputfile, dp->d_name, fvec, 0);

        }
    }
    return 0;
}