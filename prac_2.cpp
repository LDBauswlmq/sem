#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    using namespace std;
    
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    //image = imread("lena.jpg");
    int nRows = image.rows;
    int nCols = image.cols;
    printf("%d Rows and %d Cols.\n",nRows,nCols);
    
    cout << "Width : " << image.size().width << endl;
    cout << "Height: " << image.size().height << endl;
    
    cout << "Width : " << image.rows << endl;
    cout << "Height: " << image.cols << endl;
    
    Scalar intensity;
    //intensity = image.at<uchar>(100,100);
    //stringstream ss;
    //ss << intensity;
    //string str = ss.str();
    //cout << str << endl;
    cout << "Pixel value is :" << image.at<uchar>(100,100) << endl;
    printf("Pixel value : %c\n", image.at<uchar>(100,100));
    /*
     * This part we get the integral graph.
     */
    long *IG;
    IG=new long[nRows * nCols];
    for(int IG_i=0; IG_i<nRows; IG_i++){       //IG_i and IG_j shows the elements inside the integral Graph.
        for(int IG_j=0; IG_j<nCols; IG_j++){
            //IG[IG_i*nCols + IG_j] = 0;
            long tmp = 0;
            for(int i=0; i<IG_i; i++){
                for(int j=0; j<IG_j; j++){
                    intensity = image.at<uchar>(i,j);
                    stringstream ss;
                    ss << intensity;
                    string str = ss.str();
                    tmp = tmp + str[0];
                }
            }
            IG[IG_i*nCols + IG_j] = tmp;
            cout << "Pixel value is :" << str[0] << endl;
            cout << "Array value is :" << IG[IG_i*nCols + IG_j] << endl;
            cout << "Array value is :" << tmp << endl;
        }
    }
    delete [] IG;
     
    
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}