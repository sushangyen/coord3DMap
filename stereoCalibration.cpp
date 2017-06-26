//********************************************************************
// File name: STEREOCALIBRATION.CPP
// Designer : Shang-Yen Su
// Create date: May. 12, 2017
// Updated date: May. 12, 2017
// Description:
// Get rotation matrix and translation matrix from the function "stereoCalibrate"
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

//===========================================
// Name:	SteroCal
// Input: cv::Mat Left, cv::Mat Right, cv::Mat CMatrix, cv::Mat distCoeffsLeft, cv::Mat distCoeffsRight, cv::Size_<int> size
// Output: Rotation Matrix, Translation Vector, EssentialMatrix, Fundamental Matrix
// Description(程式敘述):
// Get rotation matrix and translation matrix from the function "stereoCalibrate"
//===========================================

class stereoCalClass stereoCalClass::SteroCal(cv::Mat Left, cv::Mat Right, cv::Mat CMatrix, cv::Mat distCoeffs, cv::Size_<int> size, cv::Size boardSize){
    
    std::string cornerLeftName = "img/CornerRef.jpg";
    std::string cornerRightName = "img/CornerTar.jpg";
    
    //Read the square size of chess board
    const float squareSize = 1.f;
    
    Size imageSize = Left.size();
    
    // find corners of the first board0
    std::vector<std::vector<Point2f> > cameraLeftPoints(1);
    bool found1_1 = findChessboardCorners(Left, boardSize, cameraLeftPoints[0]);
    if(!found1_1)
    {
        std::cerr << "CheckboardLeft corners not found!" << std::endl;
        exit(-1);
    }
    
    // find corners of the second board
    std::vector<std::vector<Point2f> > cameraRightPoints(1);
    bool found2_1 = findChessboardCorners(Right, boardSize, cameraRightPoints[0]);
    if(!found2_1)
    {
        std::cerr << "CheckboardRight corners not found!" << std::endl;
        exit(-1);
    }
    
    namedWindow("Corners", 1 );
    
    //Mark all of the corners on the chess board in the first image
    drawChessboardCorners(Left, boardSize, cameraLeftPoints[0], found1_1);
    imwrite(cornerLeftName, Left);
    
    //Mark all of the corners on the chess board in the second image
    drawChessboardCorners(Right, boardSize, cameraRightPoints[0], found2_1);
    imwrite(cornerRightName, Right);
    
    //Declare input and output matrix for the function "stereoCalibrate()"
    std::vector<std::vector<Point3f> > objectPoints(1);
    objectPoints[0] = Create3DChessboardCorners(boardSize, squareSize);
    vector<Point3f> ob;
    ob =Create3DChessboardCorners(boardSize, squareSize);
    Mat cameraMatrix[2];
    cameraMatrix[0] = CMatrix;
    cameraMatrix[1] = CMatrix;
    
    Mat rotationMatrix;
    Mat translationVector;
    Mat essentialMatrix;
    Mat fundamentalMatrix;
    
    //Output the rotation matrix
    //Output the translation matrix
    //Output the essential matrix
    //Output the fundamental matrix
    stereoCalibrate(objectPoints, cameraLeftPoints, cameraRightPoints, cameraMatrix[0], distCoeffs, cameraMatrix[1], distCoeffs, imageSize, rotationMatrix, translationVector, essentialMatrix, fundamentalMatrix, TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_FIX_INTRINSIC);
    
    stereoCalClass stereoCalContent;
    stereoCalContent.rotationMatrix = rotationMatrix;
    stereoCalContent.translationMatrix = translationVector;
    stereoCalContent.fundamentalMatrix = fundamentalMatrix;
    stereoCalContent.essentialMatrix = essentialMatrix;
    
    //cout << translationVector << endl;
    //cout << rotationMatrix << endl;
    
    return stereoCalContent;
}

//===========================================
// Name:	Create3DChessboardCorners
// Input: Size boardSize, float squareSize
// Output: Coordinate info of chessboard corners
// Description(程式敘述):
// Find the corner in the stereo images
//===========================================
std::vector<Point3f> Create3DChessboardCorners(Size boardSize, float squareSize)
{
    // This function creates the 3D points of your chessboard in its own coordinate system
    
    std::vector<Point3f> corners;
    
    for( int i = 0; i < boardSize.height; i++ )
    {
        for( int j = 0; j < boardSize.width; j++ )
        {
            corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
        }
    }
    
    return corners;
}
