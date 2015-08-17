// OpenCLTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Exercise()
{
	//cl_int err;
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cout << "Platform number is: " << platformList.size() << std::endl;
	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cout << "Platform is by: " << platformVendor << std::endl;
}

int main(int argc, char* argv[])
{
	


	// 1. Get a platform.
	cl_platform_id platform;
	cl_uint num_platforms;
	clGetPlatformIDs(1, &platform, &num_platforms);

	cl_device_id device;
	cl_uint num_devices;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
	
	std::cout << "Device is by: " << device << std::endl;

	// 3. Create a context and command queue on that device.
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);
	
	// 4. Perform runtime source compilation, and obtain kernel entry point.
	std::ifstream file("kernel.txt");
	std::string source;
	while (!file.eof())
	{
		char line[256];
		file.getline(line, 255);
		source += line;
	}
	const char* str = source.c_str();
	cl_program program = clCreateProgramWithSource(context,
		1,
		&str,
		NULL, NULL);

	cl_int result = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (result)
	{
		std::cout << "Error during compilation! (" << result << ")" << std::endl;
	}
	cl_kernel kernel = clCreateKernel(program, "main", NULL);

	// 5. Create data buffers.
	cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 10 * sizeof(cl_int), NULL, 0);
	cl_mem buffer1 = clCreateBuffer(context, CL_MEM_READ_WRITE, 10 * sizeof(cl_int), NULL, 0);
	cl_mem buffer2 = clCreateBuffer(context,
		CL_MEM_READ_WRITE,
		10 * sizeof(cl_int),
		NULL, 0);
	clSetKernelArg(kernel, 0, sizeof(output), (void*)&output);
	clSetKernelArg(kernel, 1, sizeof(buffer1), (void*)&buffer1);
	clSetKernelArg(kernel, 2, sizeof(buffer2), (void*)&buffer2);  // 6. Fill input data buffers
	cl_int *buffer1Ptr = (cl_int *)clEnqueueMapBuffer(queue,
		buffer1,
		CL_TRUE,
		CL_MAP_WRITE,
		0,
		10 * sizeof(cl_int),
		0, NULL, NULL, NULL);
	cl_int *buffer2Ptr = (cl_int *)clEnqueueMapBuffer(queue,
		buffer2,
		CL_TRUE,
		CL_MAP_WRITE,
		0,
		10 * sizeof(cl_int),
		0, NULL, NULL, NULL);  for (int i = 0; i < 10; i++){
		buffer1Ptr[i] = i;
		buffer2Ptr[i] = i;
	}
	clEnqueueUnmapMemObject(queue, buffer1, buffer1Ptr, 0, 0, 0);
	clEnqueueUnmapMemObject(queue, buffer2, buffer2Ptr, 0, 0, 0);
	size_t global_work_size = 10;
	// 7. Execute the kernel
	clEnqueueNDRangeKernel(queue,
		kernel,
		1,
		NULL,
		&global_work_size,
		NULL, 0, NULL, NULL);

	// 8. Look at the results via synchronous buffer map.
	cl_int *resultBufferPtr = (cl_int *)clEnqueueMapBuffer(queue,
		output,
		CL_TRUE,
		CL_MAP_READ,
		0,
		10 * sizeof(cl_int),
		0, NULL, NULL, NULL);  for (int i = 0; i < 10; i++){
		std::cout << "ptr[" << i << "] = " << resultBufferPtr[i] << std::endl;
	}

	return 0;
}





