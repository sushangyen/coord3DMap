//********************************************************************
// File name: FPSMATCHING.CPP
// Designer : Shang-Yen Su
// Create date: May. 12, 2017
// Updated date: May. 22, 2017
// Description:
// Feature points matching
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

std::vector<cv::DMatch> FPsMatching(cv::Mat refImage, cv::Mat tarImage, std::vector<cv::KeyPoint> MapKeypoint_ref, cv::Mat MapDescriptor_ref, std::vector<cv::KeyPoint> MapKeypoint_tar, cv::Mat MapDescriptor_tar){
    
    /*
    //Flann Matching
    cv::FlannBasedMatcher matcher;
    std::vector<DMatch> matches;
    matcher.match(MapDescriptor_ref, MapDescriptor_tar, matches);
    
    std::vector<cv::DMatch> NoDupMatches = deleteDuplicate(matches);
    
    
    //Find max distance and min distance of matching result
    double max_dist = 0; double min_dist = 100;
    //for( int i = 0; i < MapDescriptor_ref.rows; i++ ){
    //Q: The range of i is MapDescriptor_ref.rows which is larger than NoDupMatches.size.
    //   Will it be a problem?
    for( int i = 0; i < NoDupMatches.size(); i++ ){
        double dist = NoDupMatches[i].distance;
        if( dist < min_dist ){
            min_dist = dist;
        }
        if( dist > max_dist ){
            max_dist = dist;
        }
    }
    
    cout << "After deleteDuplicate(), total matching no. becomes " << NoDupMatches.size() << endl;
    cout << "min_dist = " << min_dist << endl;
    cout << "max_dist = " << max_dist  << endl;
    
    //Find good matches
    std::vector< DMatch > good_matches;
    
    //for( int i = 0; i < MapDescriptor_ref.rows; i++ ){
    //Q: The range of i is MapDescriptor_ref.rows which is larger than NoDupMatches.size.
    //   Will it be a problem?
    for( int i = 0; i < NoDupMatches.size(); i++ ){
        if( NoDupMatches[i].distance <= 2.0 *min_dist ){
            good_matches.push_back( NoDupMatches[i]);
        }
    }
    */
    
    //Do Matching with KNN approach
    //cv::FlannBasedMatcher matcher;
    cv::BFMatcher matcher(cv::NORM_HAMMING, false);
    vector<vector<DMatch>> matches;
    vector<DMatch> InitMatches;
    matcher.knnMatch(MapDescriptor_ref, MapDescriptor_tar, matches, 2);
    //Compare the best and the second best matches
    for (size_t i = 0; i < matches.size(); i++){
        if (matches[i][0].distance < 0.6 * matches[i][1].distance){
            InitMatches.push_back(matches[i][0]);
        }
    }
    cout << "After InitMatches, total matching no. becomes " << InitMatches.size() << endl;
    
    //Delete Duplicate Matches
    std::vector<cv::DMatch> NoDupMatches = deleteDuplicate(InitMatches);
    cout << "After DuplicateFilter(), total matching no. becomes " << NoDupMatches.size() << endl;
    
    //Do distance filter
    std::vector<cv::DMatch> tReturnStereoCameraMatches;
    tReturnStereoCameraMatches = DistanceFilter(NoDupMatches, MapKeypoint_tar);
    cout << "After DistanceFilter(), total matching no. becomes " << tReturnStereoCameraMatches.size() << endl;
    
    
    /*
    cv::Mat ref, tar;
    ref = cv::imread("img/ref.jpg", 1); tar = cv::imread("img/tar.jpg", 1);
    for(int u = 0; u < tReturnStereoCameraMatches.size(); u++){
        int kp1_idx = tReturnStereoCameraMatches[u].queryIdx;
        int kp2_idx = tReturnStereoCameraMatches[u].trainIdx;
        double x1 = MapKeypoint_ref[kp1_idx].pt.x;
        double y1 = MapKeypoint_ref[kp1_idx].pt.y;
        double x2 = MapKeypoint_tar[kp2_idx].pt.x;
        double y2 = MapKeypoint_tar[kp2_idx].pt.y;
        
        //printf("%f, %f, %f, %f\n", x1, y1, x2, y2);
        
        cv::Point2f pointRef;
        cv::Point2f pointTar;
        pointRef.x = x1; pointRef.y = y1;
        pointTar.x = x2; pointTar.y = y2;
        
        circle(ref, pointRef, 10, cv::Scalar(0,0,255), 6);
        circle(tar, pointTar, 10, cv::Scalar(0,0,255), 6);
    }
    
    cv::imwrite("img/MatchRefColor.jpg", ref);
    cv::imwrite("img/MatchTarColor.jpg", tar);
    */
    
    return tReturnStereoCameraMatches;
}
