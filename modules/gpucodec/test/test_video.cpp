/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "test_precomp.hpp"

#ifdef HAVE_NVCUVID

PARAM_TEST_CASE(Video, cv::gpu::DeviceInfo, std::string)
{
};

//////////////////////////////////////////////////////
// VideoReader

GPU_TEST_P(Video, Reader)
{
    cv::gpu::setDevice(GET_PARAM(0).deviceID());

    const std::string inputFile = std::string(cvtest::TS::ptr()->get_data_path()) + "video/" + GET_PARAM(1);

    cv::gpu::VideoReader_GPU reader(inputFile);
    ASSERT_TRUE(reader.isOpened());

    cv::gpu::GpuMat frame;

    for (int i = 0; i < 10; ++i)
    {
        ASSERT_TRUE(reader.read(frame));
        ASSERT_FALSE(frame.empty());
    }

    reader.close();
    ASSERT_FALSE(reader.isOpened());
}

//////////////////////////////////////////////////////
// VideoWriter

#ifdef WIN32

GPU_TEST_P(Video, Writer)
{
    cv::gpu::setDevice(GET_PARAM(0).deviceID());

    const std::string inputFile = std::string(cvtest::TS::ptr()->get_data_path()) + "video/" + GET_PARAM(1);

    std::string outputFile = cv::tempfile(".avi");
    const double FPS = 25.0;

    cv::VideoCapture reader(inputFile);
    ASSERT_TRUE(reader.isOpened());

    cv::gpu::VideoWriter_GPU d_writer;

    cv::Mat frame;
    cv::gpu::GpuMat d_frame;

    for (int i = 0; i < 10; ++i)
    {
        reader >> frame;
        ASSERT_FALSE(frame.empty());

        d_frame.upload(frame);

        if (!d_writer.isOpened())
            d_writer.open(outputFile, frame.size(), FPS);

        d_writer.write(d_frame);
    }

    reader.release();
    d_writer.close();

    reader.open(outputFile);
    ASSERT_TRUE(reader.isOpened());

    for (int i = 0; i < 5; ++i)
    {
        reader >> frame;
        ASSERT_FALSE(frame.empty());
    }
}

#endif // WIN32

INSTANTIATE_TEST_CASE_P(GPU_Codec, Video, testing::Combine(
    ALL_DEVICES,
    testing::Values(std::string("768x576.avi"), std::string("1920x1080.avi"))));

#endif // HAVE_NVCUVID
