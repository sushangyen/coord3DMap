//********************************************************************
// File name: MAIN.CPP
// Designer : Shang-Yen Su
// Create date: May. 08, 2017
// Updated date: May. 22, 2017
// Description:
// Build up the 3D map with two stereo images (both with chess board) took from iPhone 5S.
// Convention:
// Prefix:	m = dynamic memory; a = argument; t = temp; c = const;
//			s = string; i = integer; u = unisgned integer; f = float; d = double;
//			g = GridCell
//********************************************************************

//#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include "header.h"	//include my header file.

using namespace cv;
using namespace std;

//Board info read from the cof file
int boardSizeX; //Type of chessboard (numbers of squares)
int boardSizeY; //Type of chessboard (numbers of squares)
float boardSquareSize;  //Size of single square of chessboard
float CameraDistance;    //diatance between two corresponding cameras
float ScreenPosition;  //Position of the projected screen

bool ReadConfigure(){
    FILE *fConfig;//File pointer of coord3D configuration
    
    fConfig = fopen("Coord3D.cof", "r");
    if(fConfig==NULL){
        printf("Missing VBIPMAP.cof configuration file!!\n");
        return false;
    }
    if(!fscanf(fConfig, "boardSize = (%d, %d)\n", &boardSizeX, &boardSizeY)){
        printf("Configuration file format error - Board size!!\n");
        return false;
    };
    if(!fscanf(fConfig, "distance = %f\n", &CameraDistance)){
        printf("Configuration file format error - Camera distance!!\n");
        return false;
    }
    if(!fscanf(fConfig, "ScreenPosition = %f\n", &ScreenPosition)){
        printf("Configuration file format error - Board square size!!\n");
        return false;
    }
    
    fclose(fConfig);
    return true;
}

int main(void) {
    // Read the configuration first!
    if(!ReadConfigure()) return -1;
    
    //read two stereo imgs with chess board
cv:Mat imgRef, imgTar;
    std::string RefName = "img/cbRef.jpg";
    std::string TartName = "img/cbTar.jpg";
    
    std::string refName = "img/ref.jpg";
    std::string tarName = "img/tar.jpg";
    
    cv::Mat CBimgRef, CBimgTar;
    CBimgRef = imread(RefName, 0);  //Read image in gray scale format
    CBimgTar = imread(TartName, 0); //Read image in gray scale format
    
    //Set chess board size
    cv::Size boardSize(boardSizeX, boardSizeY);
    
    //Read camera matrix
    cv::string filename = "out_camera_data.xml";
    
    cv::FileStorage fs;
    if(!fs.open(filename, cv::FileStorage::READ)){
        printf("File Reading Failure\n");
        return NULL;
    }
    cv::Mat cameraMatrix;
    double dFx, dFy,dCx, dCy;
    //Extract camera matrix from the file
    fs["Camera_Matrix"] >> cameraMatrix;
    dFx = cameraMatrix.at<double>(0,0);
    dFy = cameraMatrix.at<double>(1,1);
    dCx = cameraMatrix.at<double>(0,2);
    dCy = cameraMatrix.at<double>(1,2);
    
    cv::Mat distCoeffs;
    double Coeffs0, Coeffs1, Coeffs4;
    //Extract distortion matrix from the file
    fs["Distortion_Coefficients"] >> distCoeffs;
    Coeffs0 = distCoeffs.at<double>(0, 0);
    Coeffs1 = distCoeffs.at<double>(1, 0);
    Coeffs4 = distCoeffs.at<double>(4, 0);
    
    //Set size of imgs
    //Q: Why need both col and row values?
    Size_<int> size = cv::Size(CBimgRef.cols, CBimgRef.rows);
    
    //Find rotation matrix, translation matrix, essential matrix, fundamental matrix, reprojection matrix (Q)
    //Note: I do not like instance and class use the same name. Easy get confused.
    class stereoCalClass stereoCal;
    stereoCal = stereoCalClass::SteroCal(CBimgRef, CBimgTar, cameraMatrix, distCoeffs, size, boardSize);
    //TODO: Should have the function member to access data member of a class
    cv::Mat RotationMatrix = stereoCal.rotationMatrix;
    cv::Mat TranslationMatrix = stereoCal.translationMatrix;
    cv::Mat FundamentalMatrix = stereoCal.fundamentalMatrix;
    
    //Read refImage and tarImage
    imgRef = imread(refName, 0);
    imgTar = imread(tarName, 0);
    
    //SIFT Feature points detection (save desciptor by SIFT & BRISK method)
    class FPsDetectClass FPs;
    FPs = FPsDetectClass::featurePtsDetect(imgRef, imgTar);
    std::vector<cv::KeyPoint> MapKeypoint_ref = FPs.MapKeypoint_ref;
    //cv::Mat MapDescriptor_ref = FPs.MapDescriptor_ref;
    cv::Mat BRISKDescriptor_ref = FPs.BRISKDescriptor_ref;
    std::vector<cv::KeyPoint> MapKeypoint_tar = FPs.MapKeypoint_tar;
    //cv::Mat MapDescriptor_tar = FPs.MapDescriptor_tar;
    cv::Mat BRISKDescriptor_tar = FPs.BRISKDescriptor_tar;
    
    //Feature points matching
    std::vector<cv::DMatch> MatchingPts = FPsMatching(imgRef, imgTar, MapKeypoint_ref, BRISKDescriptor_ref, MapKeypoint_tar, BRISKDescriptor_tar);
    
    cv::Mat homography;
    refineMatchesWithHomography(MapKeypoint_ref, MapKeypoint_tar, 1.5, MatchingPts, homography);
    
    
    //Do function undistortPoint and check the rectified result (eliminate bad matching)
    class RectifyClass Rectify;
    Rectify = RectifyClass::rectification(imgRef, imgTar, cameraMatrix, distCoeffs, RotationMatrix, TranslationMatrix, size, MapKeypoint_ref, BRISKDescriptor_ref, MapKeypoint_tar, BRISKDescriptor_tar, MatchingPts);
    cv::vector<FeaturePt> fptFinalRef = Rectify.fptFinalRef;
    cv::vector<FeaturePt> fptFinalTar = Rectify.fptFinalTar;
    cv::Mat P1 = Rectify.proejectionMatrix1;
    cv::Mat P2 = Rectify.proejectionMatrix2;
    cv::Mat QMatrix = Rectify.QMatrix;
    int DPSNumber = Rectify.DPSNumber;
    
    //Find good rectified FPs' pairs and compute their 3D coord
    compute3DMap(P1, P2, CameraDistance, ScreenPosition, DPSNumber, fptFinalRef, fptFinalTar);
    
    return 0;
}
