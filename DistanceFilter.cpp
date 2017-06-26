//********************************************************************
// File name  : DISTANCEFILTER.CPP
// Designer : Shang-Yen Su
// Create date: May. 28, 2017
// Description:
// Distance filter
// Convention:
// Prefix:	m = dynamic memory; a = argument; t = temp; c = const;
//			s = string; i = integer; u = unisgned integer; f = float; d = double;
//			g = GridCell
//********************************************************************

#include <opencv2/opencv.hpp>
#include "header.h"

std::vector<cv::DMatch> DistanceFilter(std::vector<cv::DMatch> aRawMatches, std::vector<cv::KeyPoint> aTarKeypoints){
    
    std::vector<cv::DMatch> mReturnMatches;
    
    int OverlapThres = 5;
    
    //mReturnMatches.push_back(aRawMatches[0]);// Push the first element
    //aRawMatches.erase(aRawMatches.begin());// Delete the first element
    while(aRawMatches.size()){
        bool btOverlapFlag = false;
        //bool btErase = false;
        int itDeleteIndex = 0;
        float itMinDist = 1000.0;
        for(int k = 1 ; k < aRawMatches.size() ; k ++){
            int iDistX = std::abs(aTarKeypoints[aRawMatches[0].queryIdx].pt.x - aTarKeypoints[aRawMatches[k].queryIdx].pt.x);
            int iDistY = std::abs(aTarKeypoints[aRawMatches[0].queryIdx].pt.y - aTarKeypoints[aRawMatches[k].queryIdx].pt.y);
            if(iDistX < OverlapThres && iDistY < OverlapThres){
                btOverlapFlag = true;
                float iCurrentDist = sqrt((iDistX * iDistX) + (iDistY * iDistY));
                if(iCurrentDist < itMinDist){
                    itDeleteIndex= k;//This match can be delete
                    itMinDist = iCurrentDist;//Set the itMinDist
                }
            }// Not too close
        }
        if(btOverlapFlag){
            // Two maches are too close
            if(aRawMatches[itDeleteIndex].distance > aRawMatches[0].distance){
                // aRawMatches[itDeleteIndex] and aRawMatches[0] are overlapped and
                // aRawMatches[itDeleteIndex] has larger hamming distance, then delete
                // aRawMatches[itDeleteIndex] and push aRawMatches[0] into return list
                aRawMatches.erase(aRawMatches.begin() + itDeleteIndex);
                mReturnMatches.push_back(aRawMatches[0]);
                aRawMatches.erase(aRawMatches.begin());
            }else{
                // aRawMatches[k] and aRawMatches[0] are overlapped and
                // aRawMatches[k] has smaller hamming distance, then delete
                // aRawMatches[0]
                aRawMatches.erase(aRawMatches.begin());
            }
        }else{
            // aRawMatches[0] is not overlapping with anyone.
            // Push aRawMatches[0] into return list directly.
            // Push into return list first
            mReturnMatches.push_back(aRawMatches[0]);
            // Then, erase from aRawMatches list
            aRawMatches.erase(aRawMatches.begin());
        }
        
        /*
         if(btErase){
         //mReturnMatches[] has larger hamming distance
         //mReturnMatches.erase(mReturnMatches.begin() + itOverlapIndex);
         mReturnMatches.push_back(aRawMatches[0]);
         //aRawMatches.erase(aRawMatches.begin());
         }else{
         //aRawMatched[0] has larger hamming distance
         // Drop aRawMatched[0] without adding to mReturnMatches
         //aRawMatches.erase(aRawMatches.begin());
         }
         }else{
         // Matches are all in acceptable distance
         mReturnMatches.push_back(aRawMatches[0]);// Push the first element
         //aRawMatches.erase(aRawMatches.begin());// Delete the first element
         }
         aRawMatches.erase(aRawMatches.begin());
         */
    }// End of while
    return mReturnMatches;
}
