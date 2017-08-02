/*
This code measures the duration of calculation of integral image with
different parallelization strategies.
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "features.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
using namespace cv;

int* calcIntegral();
void calcFeatures();
void loadImage();

int* calcIntegral(){



}

void loadImage(){

}


int main(void) {

	int detectorsize = 24;
	int detectorsTotal;
	int detectorsPerRow;
	int detectorsPerCol;

	Mat image = imread("lena.jpg", 0);
	Mat eqImage, cvImage;
	equalizeHist(image,eqImage);
	image.convertTo(cvImage, CV_32FC3, 1.0/255.5);
	Scalar intensity;
	cv::Size s = eqImage.size();
	int rows = s.height;
	int cols = s.width;

	int *colsTotal = &cols;
        int *rowsTotal = &rows;

	detectorsPerRow = cols - detectorsize;
	detectorsPerCol = rows - detectorsize;
	detectorsTotal = detectorsPerRow * detectorsPerCol; 




    // Create the two input vectors
    int i;
    const int LIST_SIZE = rows*cols;
  //  int *inputImage = (int*)malloc(sizeof(int)*LIST_SIZE);
    float *inputImage = new float[LIST_SIZE];
	float *semi = new float[LIST_SIZE];
    for(int i=0; i < rows; i++){
	for(int j=0; j < cols; j++){

	inputImage[i*cols+j] =cvImage.at<float>(i,j);
        }
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("integral_kernel_2d.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, 
            &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector 
    cl_mem inputImage_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            LIST_SIZE * sizeof(float), NULL, &ret);
    cl_mem integralImage_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            LIST_SIZE * sizeof(float), NULL, &ret);
    cl_mem semiintegralImage_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, 
            LIST_SIZE * sizeof(float), NULL, &ret);

    // Copy the list to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, inputImage_mem_obj, CL_TRUE, 0,
            LIST_SIZE * sizeof(float), inputImage, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, semiintegralImage_mem_obj, CL_TRUE, 0,
            LIST_SIZE * sizeof(float), semi, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "integral_2d", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputImage_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&integralImage_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&semiintegralImage_mem_obj);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void*)colsTotal );
    ret = clSetKernelArg(kernel, 4, sizeof(int), (void*)rowsTotal );

    auto start_time = std::chrono::steady_clock::now();
    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 64; // Process in groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            &global_item_size, &local_item_size, 0, NULL, NULL);

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    std::cout << "Duration is  "<< duration.count() <<"  nanoseconds\n";
    // Read the memory buffer C on the device to the local variable C
    // int *integralImage = (int*)malloc(sizeof(int)*LIST_SIZE);
    float *integralImage = new float[LIST_SIZE];
	
    ret = clEnqueueReadBuffer(command_queue, integralImage_mem_obj, CL_TRUE, 0, 
            LIST_SIZE * sizeof(float), integralImage, 0, NULL, NULL);

    for(i = 0; i < LIST_SIZE; i++){
        printf("%f = %f\n", inputImage[i], integralImage[i]);
    }
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(inputImage_mem_obj);
    ret = clReleaseMemObject(integralImage_mem_obj);
    ret = clReleaseMemObject(semiintegralImage_mem_obj);

    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);


   // free(inputImage);
   // free(integralImage);
    delete [] inputImage;
    delete [] integralImage;
    

    return 0;
}


