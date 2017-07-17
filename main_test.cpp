#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
using namespace cv;

int main(){
	
	Mat image = imread("lena.jpg", 0);
	Mat eqImage;
	equalizeHist(image, eqImage);

	Scalar intensity;
	int a=0;
	
	cv::Size s = eqImage.size();
	int rows = s.height;
	int cols = s.width;
	
	const int LIST_SIZE = rows * cols;

	int **A = new int*[rows];
	for (int i = 0; i<rows; i++)
		A[i] = new int[cols];
	
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {

			A[i][j] = image.at<uchar>(i, j);
		}
	}
	printf("inputimage: \n");
	for (int i = 0; i < rows; i++){
		printf("%d ", A[0][i]);
	}
	printf("\n inputimage finished\n");
	int total_pixels = rows*cols;
	int widthstep = total_pixels * sizeof(int);

	int * matrix_a = (int *)malloc(widthstep);
	int * matrix_b = (int *)malloc(widthstep);

	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("integral.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1,
		&device_id, &ret_num_devices);

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		LIST_SIZE * sizeof(int), NULL, &ret);
	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		LIST_SIZE * sizeof(int), NULL, &ret);


	ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
		LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
	cl_program program = clCreateProgramWithSource(context, 1,
		(const char **)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	cl_kernel kernel = clCreateKernel(program, "integral", &ret);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);

	size_t global_item_size = LIST_SIZE;
	size_t local_item_size = 64;
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);


	//int **B = (int**)malloc(sizeof(int)*LIST_SIZE);
	int **B = new int*[rows];
	for (int j = 0; j < cols; j++ ){
		B[j] = new int[cols];
	}
	ret = clEnqueueReadBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
		LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

	printf("outputimage: \n");
	for (int i = 0; i < rows; i++){
		printf("%d ", B[0][i]);
	}
	printf("\n outputimage finished\n");
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(a_mem_obj);
	ret = clReleaseMemObject(b_mem_obj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(A);
	free(B);

	return 0;
}
