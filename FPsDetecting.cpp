//********************************************************************
// File name: FPSDETECTING.CPP
// Designer : Shang-Yen Su
// Create date: May. 22, 2017
// Updated date: May. 23, 2017
// Description:
// ORB Feature points detection (save desciptor by SIFT method)
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

class FPsDetectClass FPsDetectClass::featurePtsDetect(cv::Mat refImage, cv::Mat tarImage){
    
    //Number of detected FPs
    int FPsNumber = 2000;
    
    //---------------Feature Points dectection approach #1 ---------------------
    //Use ORB detector to compute FPs of left and right images
    cv::OrbFeatureDetector detector_ref(FPsNumber);
    std::vector<cv::KeyPoint> MapKeypoint_ref;
    detector_ref.detect(refImage, MapKeypoint_ref);
    //Save decriptors in BRISK
    cv::Mat BRISKDescriptor_ref;
    cv::BRISK briskRef;
    briskRef.compute(refImage, MapKeypoint_ref, BRISKDescriptor_ref);
     
    cv::OrbFeatureDetector detector_tar(FPsNumber);
    std::vector<cv::KeyPoint> MapKeypoint_tar;
    detector_tar.detect(tarImage, MapKeypoint_tar);
    //Save decriptors in BRISK
    cv::Mat BRISKDescriptor_tar;
    cv::BRISK briskTar;
    briskTar.compute(tarImage, MapKeypoint_tar, BRISKDescriptor_tar);
    
    /*
    //---------------Feature Points dectection approach #2 ---------------------
    //SIFT FPs detect
    cv::SiftFeatureDetector detector_ref(FPsNumber);
    std::vector<cv::KeyPoint> MapKeypoint_ref;
    detector_ref.detect(refImage, MapKeypoint_ref);
    cv::SiftDescriptorExtractor extractor_ref;
    //Save decriptors in SIFT
    cv::Mat MapDescriptor_ref;
    extractor_ref.compute(refImage, MapKeypoint_ref, MapDescriptor_ref);
    //Save decriptors in BRISK
    cv::Mat BRISKDescriptor_ref;
    cv::BRISK briskRef;
    briskRef.compute(refImage, MapKeypoint_ref, BRISKDescriptor_ref);
    
    cv::SiftFeatureDetector detector_tar(FPsNumber);
    std::vector<cv::KeyPoint> MapKeypoint_tar;
    detector_tar.detect(tarImage, MapKeypoint_tar);
    cv::SiftDescriptorExtractor extractor_tar;
    //Save decriptors in SIFT
    cv::Mat MapDescriptor_tar;
    extractor_tar.compute(tarImage, MapKeypoint_tar, MapDescriptor_tar);
    //Save decriptors in BRISK
    cv::Mat BRISKDescriptor_tar;
    cv::BRISK briskTar;
    briskTar.compute(tarImage, MapKeypoint_tar, BRISKDescriptor_tar);
    */
    
    FPsDetectClass FPsContent;
    FPsContent.MapKeypoint_ref = MapKeypoint_ref;
    //FPsContent.MapDescriptor_ref = MapDescriptor_ref;
    FPsContent.BRISKDescriptor_ref = BRISKDescriptor_ref;
    FPsContent.MapKeypoint_tar = MapKeypoint_tar;
    //FPsContent.MapDescriptor_tar = MapDescriptor_tar;
    FPsContent.BRISKDescriptor_tar = BRISKDescriptor_tar;
    
    return FPsContent;
}
