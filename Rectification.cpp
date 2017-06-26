//********************************************************************
// File name: RECTIFICATION.CPP
// Designer : Shang-Yen Su
// Create date: May. 23, 2017
// Updated date: May. 24, 2017
// Description:
// Do matched FPs' Rectification and remove bad matching result
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

class RectifyClass RectifyClass:: rectification(cv::Mat refImage, cv::Mat tarImage, cv::Mat CMatrix, cv::Mat distCoeffs, cv::Mat RotationMatrix, cv::Mat TranslationMatrix, cv::Size_<int> size, std::vector<cv::KeyPoint> MapKeypoint_ref, cv::Mat MapDescriptor_ref, std::vector<cv::KeyPoint> MapKeypoint_tar, cv::Mat MapDescriptor_tar, std::vector< cv::DMatch > tReturnStereoCameraMatches){
    
    char refMatchFPsName[20] = "img/refMatchFPs.jpg";
    char tarMatchFPsName[20] = "img/tarMatchFPs.jpg";
    
    int matchingThreshold = 25; //The threshold of rectified FPs
    
    int NumFPs = tReturnStereoCameraMatches.size();

    //Save data in FeaturePt format
    FeaturePt fptFeatureRef[MapKeypoint_ref.size()];
    for(int i = 0; i < MapKeypoint_ref.size(); i++){
        fptFeatureRef[i].u = MapKeypoint_ref[i].pt.x;
        fptFeatureRef[i].v = MapKeypoint_ref[i].pt.y;
        for(int j = 0 ; j < MapDescriptor_ref.cols ; j++){
            fptFeatureRef[i].BRISK_Descriptor[j] = (int) MapDescriptor_ref.at<uchar>(i,j);
        }
    }
    
    FeaturePt fptFeatureTar[MapKeypoint_tar.size()];
    for(int i = 0; i < MapKeypoint_tar.size(); i++){
        fptFeatureTar[i].u = MapKeypoint_tar[i].pt.x;
        fptFeatureTar[i].v = MapKeypoint_tar[i].pt.y;
        for(int j = 0 ; j < MapDescriptor_tar.cols ; j++){
            fptFeatureTar[i].BRISK_Descriptor[j] = (int) MapDescriptor_tar.at<uchar>(i,j);
        }
    }
    
    std::vector<Point2f> matchingRefPts[NumFPs];
    std::vector<Point2f> matchingTarPts[NumFPs];

    //Write 2D coord and desciptor info into txt file
    for(int u = 0; u < NumFPs; u++){
        int kp1_idx = tReturnStereoCameraMatches[u].queryIdx;
        int kp2_idx = tReturnStereoCameraMatches[u].trainIdx;
        double x1 = fptFeatureRef[kp1_idx].u;
        double y1 = fptFeatureRef[kp1_idx].v;
        double x2 = fptFeatureTar[kp2_idx].u;
        double y2 = fptFeatureTar[kp2_idx].v;

        matchingRefPts[u].push_back(Point2f(x1, y1));
        matchingTarPts[u].push_back(Point2f(x2, y2));
    }
    
    //Do undistort for the stereo images
    cv::Mat R1 = cv::Mat::zeros(3, 3, CV_64FC1);
    cv::Mat P1 = cv::Mat::zeros(3, 4, CV_64FC1);
    cv::Mat R2 = cv::Mat::zeros(3, 3, CV_64FC1);
    cv::Mat P2 = cv::Mat::zeros(3, 4, CV_64FC1);
    cv::Mat Q = cv::Mat::zeros(4, 4, CV_32FC1);
    
    //Do rectification to find R1, P1, R2, P2
    cv::stereoRectify(CMatrix, distCoeffs, CMatrix, distCoeffs, size, RotationMatrix, TranslationMatrix, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, size);
    
    int DPSNumber = 0;
    
    FeaturePt fptFinalRef[MapKeypoint_ref.size()];
    FeaturePt fptFinalTar[MapKeypoint_tar.size()];
    
    
    /////////
    cv::Mat ref, tar;
    ref = cv::imread("img/ref.jpg", 1); tar = cv::imread("img/tar.jpg", 1);
    /////////
    std::vector<Point2f> UndistortRefPoints[NumFPs];
    std::vector<Point2f> UndistortTarPoints[NumFPs];
    float refUndistortPTsX, refUndistortPTsY, tarUndistortPTsX, tarUndistortPTsY;
    for (int j = 0; j < NumFPs; j++){
        cv::undistortPoints(matchingRefPts[j], UndistortRefPoints[j], CMatrix, distCoeffs, R1, P1);
        cv::undistortPoints(matchingTarPts[j], UndistortTarPoints[j], CMatrix, distCoeffs, R2, P2);
        
        refUndistortPTsX = UndistortRefPoints[j].at(0).x;
        refUndistortPTsY = UndistortRefPoints[j].at(0).y;
        tarUndistortPTsX = UndistortTarPoints[j].at(0).x;
        tarUndistortPTsY = UndistortTarPoints[j].at(0).y;
        
        if(abs(refUndistortPTsY - tarUndistortPTsY) < matchingThreshold)
        {
            cv::Point2f pointRef;
            //pointRef.x = refUndistortPTsX; pointRef.y = refUndistortPTsY;
            pointRef.x = matchingRefPts[j].at(0).x; pointRef.y = matchingRefPts[j].at(0).y;
        
            circle(refImage, pointRef, 10, cv::Scalar(255,0,0));
            cv::imwrite(refMatchFPsName, refImage);
            
            cv::Point2f pointTar;
            //pointTar.x = tarUndistortPTsX; pointTar.y = tarUndistortPTsY;
            pointTar.x = matchingTarPts[j].at(0).x; pointTar.y = matchingTarPts[j].at(0).y;

            circle(tarImage, pointTar, 10, cv::Scalar(255,0,0));
            cv::imwrite(tarMatchFPsName, tarImage);
            
            fptFinalRef[DPSNumber].u = refUndistortPTsX;
            fptFinalRef[DPSNumber].v = refUndistortPTsY;
            fptFinalTar[DPSNumber].u = tarUndistortPTsX;
            fptFinalTar[DPSNumber].v = tarUndistortPTsY;
            
            for(int w = 0 ; w < MapDescriptor_ref.cols ; w++){
                fptFinalRef[DPSNumber].BRISK_Descriptor[w] = (int) MapDescriptor_ref.at<uchar>(j,w);
                fptFinalTar[DPSNumber].BRISK_Descriptor[w] = (int) MapDescriptor_tar.at<uchar>(j,w);
            }
            
            DPSNumber = DPSNumber + 1;  //Count the number of selected FPs
        }
    }
    
    RectifyClass RectifyContent;
    RectifyContent.fptFinalRef.clear();
    RectifyContent.fptFinalRef.resize(DPSNumber);
    RectifyContent.fptFinalTar.clear();
    RectifyContent.fptFinalTar.resize(DPSNumber);
    
    for(int y = 0; y < DPSNumber; y++){
        RectifyContent.fptFinalRef[y].u = fptFinalRef[y].u;
        RectifyContent.fptFinalRef[y].v = fptFinalRef[y].v;
        RectifyContent.fptFinalTar[y].u = fptFinalTar[y].u;
        RectifyContent.fptFinalTar[y].v = fptFinalTar[y].v;
        for(int w = 0; w < MapDescriptor_ref.cols; w++){
            RectifyContent.fptFinalRef[y].BRISK_Descriptor[w] = fptFinalRef[y].BRISK_Descriptor[w];
            RectifyContent.fptFinalTar[y].BRISK_Descriptor[w] = fptFinalTar[y].BRISK_Descriptor[w];
        }
    }
    
    RectifyContent.proejectionMatrix1 = P1;
    RectifyContent.proejectionMatrix2 = P2;
    RectifyContent.QMatrix = Q;
    RectifyContent.DPSNumber = DPSNumber;
    
    return RectifyContent;
}
