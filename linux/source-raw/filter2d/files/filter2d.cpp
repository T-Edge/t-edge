
/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <iostream>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "cmdlineparser.h"
#include "opencv2/opencv.hpp"

#include "coefficients.h"
#include "common.h"

#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"

using namespace sda;
using namespace sda::utils;

static void yuv2raw(const cv::Mat &yuv, uchar *y_buf, int stride_y,
                    uchar *u_buf, int stride_u, uchar *v_buf, int stride_v)
{
    // Assumes RGB or YUV 4:4:4
    for (int y = 0; y < yuv.rows; y++) {
        for (int x = 0; x < yuv.cols; x++) {
            cv::Vec3b cv_pix        = yuv.at<cv::Vec3b>(y, x);
            y_buf[y * stride_y + x] = cv_pix.val[0];
            u_buf[y * stride_u + x] = cv_pix.val[1];
            v_buf[y * stride_v + x] = cv_pix.val[2];
        }
    }
}

static void raw2yuv(uchar *y_buf, int stride_y, uchar *u_buf, int stride_u,
                    uchar *v_buf, int stride_v, cv::Mat &img)
{
    // Assumes RGB or YUV 4:4:4
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            cv::Vec3b cv_pix;
            cv_pix.val[0]           = y_buf[y * stride_y + x];
            cv_pix.val[1]           = u_buf[y * stride_u + x];
            cv_pix.val[2]           = v_buf[y * stride_v + x];
            img.at<cv::Vec3b>(y, x) = cv_pix;
        }
    }
}

static void writeRawImage(unsigned width, unsigned height, unsigned stride,
                          unsigned depth, unsigned nchannels, uchar *y_buf,
                          uchar *u_buf, uchar *v_buf, std::string filename)
{
    cv::Mat yuv(height, width, CV_8UC3);

    // Convert processed image from Raw to cvImage
    raw2yuv(y_buf, stride, u_buf, stride, v_buf, stride, yuv);

    cv::imwrite(filename.c_str(), yuv);
}

int main(int argc, char **argv)
{
    printf("-------------------------------------------------------\n");
    printf("\n");
    printf("Xilinx 2D Filter Example Application\n");
    printf("\n");

    // ---------------------------------------------------------------------------------
    // Parse command line
    // ---------------------------------------------------------------------------------

    CmdLineParser parser;
    parser.addSwitch("--nruns", "-n", "Number of times to image is processed",
                     "1");
    parser.addSwitch("--input", "-i", "Input image file");
    parser.addSwitch("--filter", "-f", "Filter type (0-6)", "0");
    parser.addSwitch("--maxreqs", "-r",
                     "Maximum number of outstanding requests", "3");
    parser.addSwitch("--compare", "-c", "Compare FPGA and SW performance",
                     "false", true);

    // parse all command line options
    parser.parse(argc, argv);
    string inputImage   = parser.value("input");
    int numRuns         = parser.value_to_int("nruns");
    unsigned filterType = parser.value_to_int("filter");
    int maxReqs         = parser.value_to_int("maxreqs");
    bool comparePerf    = parser.value_to_bool("compare");

    if (inputImage.size() == 0) {
        printf("ERROR: input image file must be specified using -i command "
               "line switch\n");
        return -1;
    }
    if (filterType > 6) {
        printf("ERROR: Supported filter type values are [0:6]\n");
        return -1;
    }

    printf("Input image       : %s\n", inputImage.c_str());
    printf("Number of runs    : %d\n", numRuns);
    printf("Filter type       : %d\n", filterType);
    printf("Max requests      : %d\n", maxReqs);
    printf("Compare perf.     : %d\n", comparePerf);
    printf("\n");

    // ---------------------------------------------------------------------------------
    // Read input image and format inputs
    // ---------------------------------------------------------------------------------

    std::string srcFileName = inputImage;

    cv::Mat src = cv::imread(srcFileName.c_str(), cv::IMREAD_COLOR);
    if (src.empty()) {
        printf("ERROR: Loading image %s failed\n", srcFileName.c_str());
        return -1;
    }

    // covert to YUV
    cv::Mat yuv;
    cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);

    unsigned width     = src.cols;
    unsigned height    = src.rows;
    unsigned depth     = src.depth();
    unsigned nchannels = src.channels();
    unsigned stride    = ceil(width / 64.0) * 64;
    unsigned nbytes    = (stride * height);

    // Input and output buffers (Y,U, V)
    unsigned char *y_src = (unsigned char *)malloc(nbytes);
    unsigned char *u_src = (unsigned char *)malloc(nbytes);
    unsigned char *v_src = (unsigned char *)malloc(nbytes);
    unsigned char *y_dst = (unsigned char *)malloc(nbytes);
    unsigned char *u_dst = (unsigned char *)malloc(nbytes);
    unsigned char *v_dst = (unsigned char *)malloc(nbytes);

    // Convert CV Image to AXI video data
    yuv2raw(src, y_src, stride, u_src, stride, v_src, stride);

    auto fpga_begin = std::chrono::high_resolution_clock::now();

    for (int n = 0; n < numRuns; n++) {
        Filter2DKernel(filterType, width, height, stride, y_src, y_dst);
        Filter2DKernel(filterType, width, height, stride, u_src, u_dst);
        Filter2DKernel(filterType, width, height, stride, v_src, v_dst);
    }
    //     Filter2DKernel.finish();

    auto fpga_end = std::chrono::high_resolution_clock::now();

    writeRawImage(width, height, stride, depth, nchannels, y_dst, u_dst, v_dst,
                  inputImage.substr(0, inputImage.size() - 4) + "_out.bmp");

    // ---------------------------------------------------------------------------------
    // Compute reference results and compare
    // ---------------------------------------------------------------------------------

    if (comparePerf) {
        printf("Running Software version\n");
    }

    // Retrieve filter factor and bias
    float factor = filterFactors[filterType];
    short bias   = filterBiases[filterType];

    // Create output buffers for reference results
    unsigned char *y_ref = (unsigned char *)malloc(nbytes);
    unsigned char *u_ref = (unsigned char *)malloc(nbytes);
    unsigned char *v_ref = (unsigned char *)malloc(nbytes);

    unsigned int numRunsSW = comparePerf ? numRuns : 1;

    auto cpu_begin = std::chrono::high_resolution_clock::now();

    for (unsigned int n = 0; n < numRunsSW; n++) {
        // Compute reference results
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, stride,
                 y_src, y_ref);
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, stride,
                 u_src, u_ref);
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, stride,
                 v_src, v_ref);
    }

    auto cpu_end = std::chrono::high_resolution_clock::now();

    // Write image to file
    writeRawImage(width, height, stride, depth, nchannels, y_ref, u_ref, v_ref,
                  inputImage.substr(0, inputImage.size() - 4) + "_ref.bmp");

    printf("Comparing results\n");

    // Compare results
    bool diff = false;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (y_dst[y * stride + x] != y_ref[y * stride + x])
                diff = true;
            if (u_dst[y * stride + x] != u_ref[y * stride + x])
                diff = true;
            if (v_dst[y * stride + x] != v_ref[y * stride + x])
                diff = true;
        }
    }

    if (diff) {
        printf("\n%sTest FAILED: Output has mismatches with reference%s\n",
               RED, RESET);
    } else {
        printf("\n%sTest PASSED: Output matches reference%s\n", GREEN, RESET);
    }

    // Report performance (if not running in emulation mode)
    std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
    std::chrono::duration<double> cpu_duration  = cpu_end - cpu_begin;

    float fpga_throughput = (double)numRuns * 3 * nbytes
                            / fpga_duration.count() / (1024.0 * 1024.0);
    float cpu_throughput = (double)numRuns * 3 * nbytes / cpu_duration.count()
                           / (1024.0 * 1024.0);

    printf("\n");
    printf("FPGA Time         : %10.4f s\n", fpga_duration.count());
    printf("FPGA Throughput   : %10.4f MB/s\n", fpga_throughput);
    if (comparePerf) {
        printf("CPU  Time         : %10.4f s\n", cpu_duration.count());
        printf("CPU  Throughput   : %10.4f MB/s\n", cpu_throughput);
        printf("FPGA Speedup      : %10.4f x\n",
               cpu_duration.count() / fpga_duration.count());
    }

    return (diff ? 1 : 0);
}
