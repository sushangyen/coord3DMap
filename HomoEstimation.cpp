//********************************************************************
// File name: HOMOESTIMATION.CPP
// Designer : Shang-Yen Su
// Create date: Jun. 07, 2017
// Updated date: Jun. 07, 2017
// Description:
// Find the best matching result with homography transformation
// Convention:
// Prefix:	m = dynamic memory; a = argument; t = temp; c = const;
//			s = string; i = integer; u = unisgned integer; f = float; d = double;
//			g = GridCell
//********************************************************************

//#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include "header.h"	//include my header file.
#include <iostream>

using namespace cv;
using namespace std;

bool refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints, const std::vector<cv::KeyPoint>& trainKeypoints, float reprojectionThreshold, std::vector<cv::DMatch>& matches, cv::Mat& homography){
    const int minNumberMatchesAllowed = 8;
    if (matches.size() < minNumberMatchesAllowed)
        return false;
    // Prepare data for cv::findHomography
    std::vector<cv::Point2f> srcPoints(matches.size());
    std::vector<cv::Point2f> dstPoints(matches.size());
    for (size_t i = 0; i < matches.size(); i++)
    {
        srcPoints[i] = trainKeypoints[matches[i].trainIdx].pt;
        dstPoints[i] = queryKeypoints[matches[i].queryIdx].pt;
    }
    // Find homography matrix and get inliers mask
    std::vector<unsigned char> inliersMask(srcPoints.size());
    homography = cv::findHomography(srcPoints, dstPoints, CV_FM_RANSAC, reprojectionThreshold, inliersMask);
    std::vector<cv::DMatch> inliers;
    for (size_t i=0; i<inliersMask.size(); i++)
    {
        if (inliersMask[i])
            inliers.push_back(matches[i]);
    }
    matches.swap(inliers);
    return matches.size() > minNumberMatchesAllowed;
}
