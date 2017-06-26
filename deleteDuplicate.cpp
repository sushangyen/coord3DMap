//********************************************************************
// File name  : DELETEDUPLICATE.CPP
// Designer : Shang-Yen Su
// Create date: May. 27, 2017
// Description:
// Find the duplicated fature match and delete one of them.
// OpenCV mknnMatch matcher will sometime match two query feature points to
// a same train feature point. Hence, we need to delete one of them.
// Convention:
// Prefix:	m = dynamic memory; a = argument; t = temp; c = const;
//			s = string; i = integer; u = unisgned integer; f = float; d = double;
//			g = GridCell
//********************************************************************

#include <opencv2/opencv.hpp>
#include "header.h"

std::vector<cv::DMatch> deleteDuplicate(std::vector<cv::DMatch> aRawMatches){
    
    std::vector<cv::DMatch> mReturnMatches;
    
    //Set the first element
    mReturnMatches.push_back(aRawMatches[0]);
    for(int j = 1; j < aRawMatches.size(); j++ ){
        bool bDuplicate = false;
        for(int m = 0 ; m < mReturnMatches.size(); m ++){
            if(mReturnMatches[m].trainIdx == aRawMatches[j].trainIdx){
                //Found a duplicate match. Keep the one with smallest match only.
                bDuplicate = true;
                if(mReturnMatches[m].distance > aRawMatches[j].distance){
                    // aRawMatches[j] has samller hamming distance than mReturnBestNMatches[m]
                    // We keep the smaller hamming distance only.
                    mReturnMatches.erase(mReturnMatches.begin() + m);
                    mReturnMatches.push_back(aRawMatches[j]);
                }
                continue;
            }
        }
        if(!bDuplicate){
            //Not a duplicate match, push into the return vector
            mReturnMatches.push_back(aRawMatches[j]);
        }
    }
    
    return mReturnMatches;
}
