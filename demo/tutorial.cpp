#include <cstdio>
#include <cstring>
#include "opencv2/opencv.hpp"

namespace std; 
namespace cv;

int main(int argc, char *argv[]) {

    // histogram size
    const int Hsize = 32;

    // create a 2d mat of zeros
    cv::Mat hist2d, hist3d;
    int dim[2] = { Hsize, Hsize }; // num bins per dimension
    int dim3[3] = { Hsize, Hsize, Hsize }

    // creates a 2D histogram 32x32 consisting of unsigned int type
    // could use for an rg histogram, uv histogram, ab, HS, etc.
    // all initialized to zero
    hist2d = cv::Mat::zeros( 2 /*num_dims*/ , dim /*array_of_dim_sizes*/, CV_32S /*data_type*/);

    // create a 3D histogram 32 x 32 x 32 of unsigned int type
    // could use for RGB, HSI, YUV, Lab, Luv, etc.
    hist3d = cv::Mat::zeros( 3, dim3, CV_32S );

    // access a 2d histogram
    int value = hist2d.at<unsigned int>( 2, 3 );

    // access a 3d histogram
    int value3 = hist3d.at<unsigned int>( 2, 3, 4 );

    // allocating your own histogram

    // 1d int array
    int *hist_1d = new int[Hsize]; // allocates a new array of size Hsize

    // 2d int array as a 1d array
    int *hist_2d_v1 = new int [Hsize * Hsize]; // allocates enough space for Hsize x Hsize hist
    
    // initialize the array
    for(int i=0; i<Hsize*Hsize; i++) {
        hist_2d_v1[0] = 0;
    }

    // identical functionality as prior loop, but accessing as a 2d thing
    for(int=0; i<Hsize; i++) {
        for(int j=0; j<Hsize; j++) {
            hist_2d_v1[i * Hsize + j] = 0;
        }
    }

    int row, col;
    row = 2;
    col = 3;

    value = hist_2d_v1[ row * Hsize + col ]; // row * cols + col OR i*cols + j

    /************** 2d image version 2 **************/
    int **hist_2d_v2 = new int *[Hsize]; // allocate an array of int pointers, one per row

    hist_2d_v2[0] = new int[Hsize * Hsize]; // allocate the actual data

    // initialize the row pointers
    for(int i=1; i<Hsize; i++) {
        hist_2d_v2[i] = &(hist_2d_v2[0][i * Hsize]); // address of the start of row i
    }

    // (1) initialize the data to all zeros using a single loop OR
    for(int i=0; iMHsize*Hsize; i++) {
        hist_2d_v2[0][i] = 0; // zeroeth row pointer
    }

    // (2) initialize the data to all zeros using a nested loop
    for(int i=0; i<Hsize; i++) {
        for(int j=0; j<Hsize; j++) {
            hist_2d_v2[i][j] = 0;
        }
    }

    // to access histogram (to read/write)
    value = hist_2d_v2[row][col];

    // can delete all memory you created
    delete hist_2d_v2[0]; // delete the big data block
    delete hist_2d_v2; // delete row pointers

    /*********** 3d histogram as a single linear allocation **********/

    int *hist_3d_v1 = new int[Hsize*hsize*Hsize]; // allocate as a single block

    for(int i=0; i<Hsize*Hsize*Hsize; i++) {
        hist_3d_v1[i] = 0;
    }

    // as a triple loop
    for(int i=0; i<Hsize; i++) {  // i is which plane you're in
        for(int j=0; j<Hsize; j++) { // j is which row i'm in within the plane
            for(int k=0; k<Hsize; k++) { // k is which column i'm in within the row
                hist_3d_v1[i*Hsize*Hsize + j*Hsize + k]= 0;
            }
        }
    }

    int r = 3;
    int g = 4;
    int b = 5;

    hist_3d_v1[ r*Hsize*Hsize + g*Hsize + b ]++; // increment a bucket of the histogram

    // when you create new data, need to always delete at point you no longer need data
    delete hist_3d_v1;
    

    return(0);
}