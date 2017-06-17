//SixTrackLib
//
//Authors: R. De Maria, G. Iadarola, D. Pellegrini, H. Jasim
//
//Copyright 2017 CERN. This software is distributed under the terms of the GNU
//Lesser General Public License version 2.1, copied verbatim in the file
//`COPYING''.
//
//In applying this licence, CERN does not waive the privileges and immunities
//granted to it by virtue of its status as an Intergovernmental Organization or
//submit itself to any jurisdiction.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "block.h"
#include "tracking_cl.h"
#include "tracking_err.h"

cl_device_id create_device()
{
	cl_platform_id platform;
	cl_device_id dev;
	int err;
	err = clGetPlatformIDs(1, &platform, NULL);
	Check_Failure("Failed to retrieve OpenCL platform.");
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	Check_Failure("Failed to retrieve OpenCL device.");
	return dev;
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename,
			 const char *options)
{
	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	int err;
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
		perror("Failed to read OpenCL kernel.");
		return NULL;
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char *)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);
	program =
	    clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer,
				      &program_size, &err);
	Check_Failure("Failed to create OpenCL program.");
	free(program_buffer);
	err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
	if (err < 0) {
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0,
				      NULL, &log_size);
		program_log = (char *)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
				      log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		return NULL;
	}
	return program;
}

struct tracking_cl *tracking_cl_prepare(struct tracking_elements *elements,
					struct tracking_beam *beam,
					unsigned int nturn,
					unsigned int track_by_turn,
					unsigned int track_by_element)
{
	cl_int err;
	cl_device_id dev = create_device();
	if (dev == NULL) {
		return NULL;
	}
	cl_context context = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
	Check_Failure("Failed to create OpenCL context.");
	char build_options[100];
	sprintf(build_options,
		"%s %s %s",
		Build_Includes,
		(track_by_turn ? Track_By_Turn_Flag : ""),
		(track_by_element ? Track_By_Element_Flag : "")
	    );
	cl_program program =
	    build_program(context, dev, Kernel_File, build_options);
	if (program == NULL) {
		return NULL;
	}
	cl_command_queue queue = clCreateCommandQueue(context, dev, 0, &err);
	Check_Failure("Failed to create OpenCL command queue.");
	cl_kernel kernel = clCreateKernel(program, Tracking_Function, &err);
	Check_Failure("Failed to create OpenCL kernel.");
	cl_mem elements_buffer =
	    clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   elements->te_head * sizeof(value_t),
			   elements->te_data, &err);
	Check_Failure
	    ("Failed to create OpenCL buffer for tracking_cl elements data.");
	cl_mem offsets_buffer =
	    clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   (elements->te_offsets[0] + 1) * sizeof(int),
			   elements->te_offsets, &err);
	Check_Failure
	    ("Failed to create OpenCL buffer for tracking_cl elements offsets.");
	cl_mem beam_buffer =
	    clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			   beam->b_npart * sizeof(struct particle),
			   beam->b_particles, &err);
	Check_Failure("Failed to create OpenCL buffer for tracking_cl beam");
	struct tracking_beam *beam_by_turn = NULL;
	cl_mem beam_by_turn_buffer = NULL;
	struct tracking_beam *beam_by_element = NULL;
	cl_mem beam_by_element_buffer = NULL;
	if (track_by_turn) {
		beam_by_turn =
		    (struct tracking_beam *)
		    malloc(sizeof(struct tracking_beam));
		if (beam_by_turn == NULL) {
			perror
			    ("Failed to allocate tracking_cl beam by turn structure");
			return NULL;
		}
		beam_by_turn->b_npart = nturn * beam->b_npart;
		beam_by_turn->b_particles =
		    (struct particle *)malloc(beam_by_turn->b_npart *
					      sizeof(struct particle));
		if (beam_by_turn->b_particles == NULL) {
			perror
			    ("Failed to allocate tracking_cl beam by turn particles");
			return NULL;
		}
		beam_by_turn_buffer =
		    clCreateBuffer(context, CL_MEM_WRITE_ONLY,
				   beam_by_turn->b_npart *
				   sizeof(struct particle),
				   beam_by_turn->b_particles, &err);
		Check_Failure
		    ("Failed to create OpenCL buffer for beam by turn particles.");
	}
	if (track_by_element) {
		beam_by_element =
		    (struct tracking_beam *)
		    malloc(sizeof(struct tracking_beam));
		if (beam_by_element == NULL) {
			perror
			    ("Failed to allocate tracking_cl beam by element structure");
			return NULL;
		}
		beam_by_element->b_npart =
		    elements->te_offsets[0] * nturn * beam->b_npart;
		beam_by_element->b_particles =
		    (struct particle *)malloc(beam_by_element->b_npart *
					      sizeof(struct particle));
		if (beam_by_element->b_particles == NULL) {
			perror
			    ("Failed to allocate tracking_cl beam by element particles");
			return NULL;
		}
		beam_by_element_buffer =
		    clCreateBuffer(context, CL_MEM_WRITE_ONLY,
				   beam_by_element->b_npart *
				   sizeof(struct particle),
				   beam_by_element->b_particles, &err);
		Check_Failure
		    ("Failed to create OpenCL buffer for beam by turn particles.");
	}
	int i = 0;
	err = clSetKernelArg(kernel, i++, sizeof(cl_mem), &elements_buffer);
	Check_Failure("Failed to set argument of OpenCl kernel: elements");
	err = clSetKernelArg(kernel, i++, sizeof(cl_mem), &offsets_buffer);
	Check_Failure("Failed to set argument of OpenCl kernel: offsets");
	err = clSetKernelArg(kernel, i++, sizeof(cl_mem), &beam_buffer);
	Check_Failure("Failed to set argument of OpenCl kernel: particles");
	err = clSetKernelArg(kernel, i++, sizeof(unsigned int), &nturn);
	Check_Failure("Failed to set argument of OpenCl kernel: nturn");
	err = clSetKernelArg(kernel, i++, sizeof(int), &(beam->b_npart));
	Check_Failure("Failed to set argument of OpenCl kernel: npart");
	if (track_by_turn) {
		err =
		    clSetKernelArg(kernel, i++, sizeof(cl_mem),
				   &beam_by_turn_buffer);
		Check_Failure
		    ("Failed to set argument of OpenCl kernel: particles_by_turn");
	}
	if (track_by_element) {
		err =
		    clSetKernelArg(kernel, i++, sizeof(cl_mem),
				   &beam_by_element_buffer);
		Check_Failure
		    ("Failed to set argument of OpenCl kernel: particles_by_element");
	}
	struct tracking_cl *track =
	    (struct tracking_cl *)malloc(sizeof(struct tracking_cl));
	if (track == NULL) {
		perror("Failed to allocate tracking_cl structure.");
		return NULL;
	}
	track->t_context = context;
	track->t_queue = queue;
	track->t_program = program;
	track->t_kernel = kernel;
	track->t_elements = elements;
	track->t_elements_buffer = elements_buffer;
	track->t_offsets_buffer = offsets_buffer;
	track->t_beam = beam;
	track->t_beam_buffer = beam_buffer;
	track->t_beam_by_turn = beam_by_turn;
	track->t_beam_by_turn_buffer = beam_by_turn_buffer;
	track->t_beam_by_element = beam_by_element;
	track->t_beam_by_element_buffer = beam_by_element_buffer;
	track->t_nturn = nturn;
	return track;
}

cl_int tracking_cl_execute(struct tracking_cl * track)
{
	cl_int err;
	err =
	    clEnqueueNDRangeKernel(track->t_queue, track->t_kernel, 1, NULL,
				   &(track->t_beam->b_npart), NULL, 0, NULL,
				   NULL);
	if (err < 0) {
		perror("Failed to enqueue OpenCL kernel.");
		return err;
	}
	return 0;
}

cl_int tracking_cl_read(struct tracking_cl * track)
{
	cl_int err;
	err =
	    clEnqueueReadBuffer(track->t_queue, track->t_beam_buffer, CL_TRUE,
				0,
				track->t_beam->b_npart *
				sizeof(struct particle),
				track->t_beam->b_particles, 0, NULL, NULL);
	if (err < 0) {
		perror("Failed to read track_cl beam buffer");
		return err;
	}
	if (track->t_beam_by_turn_buffer != NULL
	    && track->t_beam_by_turn != NULL) {
		err =
		    clEnqueueReadBuffer(track->t_queue,
					track->t_beam_by_turn_buffer, CL_TRUE,
					0, sizeof(struct particle),
					track->t_beam_by_turn, 0, NULL, NULL);
		if (err < 0) {
			perror
			    ("Failed to read tracking_cl beam by turn buffer");
		}
	}
	if (track->t_beam_by_element_buffer != NULL
	    && track->t_beam_by_element != NULL) {
		err =
		    clEnqueueReadBuffer(track->t_queue,
					track->t_beam_by_element_buffer,
					CL_TRUE, 0, sizeof(struct particle),
					track->t_beam_by_element, 0, NULL,
					NULL);
		if (err < 0) {
			perror
			    ("Failed to read tracking_cl beam by element buffer");
		}
	}
	return err;
}

void tracking_cl_clean(struct tracking_cl *track)
{
	clReleaseKernel(track->t_kernel);
	clReleaseMemObject(track->t_elements_buffer);
	clReleaseMemObject(track->t_offsets_buffer);
	clReleaseMemObject(track->t_beam_buffer);
	if (track->t_beam_by_turn != NULL) {
		clReleaseMemObject(track->t_beam_by_turn_buffer);
	}
	if (track->t_beam_by_element != NULL) {
		clReleaseMemObject(track->t_beam_by_element_buffer);
	}
	clReleaseCommandQueue(track->t_queue);
	clReleaseProgram(track->t_program);
	clReleaseContext(track->t_context);
	if (track->t_beam_by_turn != NULL) {
		tracking_beam_clean(track->t_beam_by_turn);
	}
	if (track->t_beam_by_element != NULL) {
		tracking_beam_clean(track->t_beam_by_element);
	}
	tracking_beam_clean(track->t_beam);
	tracking_elements_clean(track->t_elements);
	free(track);
}
