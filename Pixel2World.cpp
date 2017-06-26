//********************************************************************
// File name  : PIXEL2WORLD.CPP
// Designer : Shang-Yen Su
// Create date: May. 07, 2017
// Description:
// This file generate a matrix for pixel to world trandformation
// Naming convention:
// Prefix: s = string; i = integer; u = unisgned integer; f = float
//         d = double; m = dynamic memory; a = argument; t = temp
//********************************************************************
#include <stdio.h>

#include "header.h"	//include my header file.

cv::Mat Pixel2World(double dFL)
{
    //Set the return matrix
    cv::Mat XMatrix = cv::Mat::zeros(4, 3, CV_64FC1);
    
    //1. Open XML file of camera calibration data
    cv::string filename = "out_camera_data.xml";
    
    cv::FileStorage fs;
    if(!fs.open(filename, cv::FileStorage::READ)){
        printf("File Reading Failure\n");
        return XMatrix;
    }
    
    //2. Read camera intrinsic data from the file
    cv::Mat cameraMatrix;
    //double dFL = 2.9;//Set for test
    double dFx, dFy,dCx, dCy;
    //Extract camera matrix from the file
    fs["Camera_Matrix"] >> cameraMatrix;
    dFx = cameraMatrix.at<double>(0,0);
    dFy = cameraMatrix.at<double>(1,1);
    dCx = cameraMatrix.at<double>(0,2);
    dCy = cameraMatrix.at<double>(1,2);
    
#ifdef DEBUG_MATRIX
    printf("Fx = %lf\t Fy = %lf\t Cx = %lf\t Cy = %lf\n", dFx, dFy, dCx, dCy);
#endif
    
#ifdef Pxiel2World_Approach01
    //3. Set the matriics A and B
    cv::Mat MatrixL43 = cv::Mat::zeros(4, 3, CV_64FC1);
    cv::Mat MatrixR33 = cv::Mat::zeros(3, 3, CV_64FC1);
    //Set the left side matrix of multiplication
    MatrixL43.at<double>(0,0) = dFx;
    MatrixL43.at<double>(1,0) = dCx;
    MatrixL43.at<double>(1,1) = dCy;
    MatrixL43.at<double>(2,1) = -1*dFy;
    MatrixL43.at<double>(3,1) = dFL * dFy;
    MatrixL43.at<double>(1,2) = 1;
    //Set the right side matrix of multiplication
    MatrixR33.at<double>(0,0) = dFx*dFx + dCx * dCx;
    MatrixR33.at<double>(1,0) = dCx * dCy;
    MatrixR33.at<double>(2,0) = dCx;
    MatrixR33.at<double>(0,1) = dCx * dCy;
    MatrixR33.at<double>(1,1) = dCy * dCy + dFy * dFy + (dFL*dFy)*(dFL*dFy);
    MatrixR33.at<double>(2,1) = dCy;
    MatrixR33.at<double>(0,2) = dCx;
    MatrixR33.at<double>(1,2) = dCy;
    MatrixR33.at<double>(2,2) = 1;
    
    //4. Inverse matrix B and multiply with matrix A
    cv::Mat MatrixR33Inv = MatrixR33.inv();
    XMatrix = MatrixL43 * MatrixR33Inv;
#endif
    
    //#ifdef Pxiel2World_Approach02
    //3. Set the matriics A, B, C
    cv::Mat MatrixA43 = cv::Mat::zeros(4, 3, CV_64FC1);
    cv::Mat MatrixB33 = cv::Mat::zeros(3, 3, CV_64FC1);
    cv::Mat MatrixC33 = cv::Mat::zeros(3, 3, CV_64FC1);
    //Set matrix A of multiplication
    MatrixA43.at<double>(0,0) = 1;
    MatrixA43.at<double>(2,1) = -1;
    MatrixA43.at<double>(3,1) = dFL;
    MatrixA43.at<double>(1,2) = 1;
    //Set matrix B of multiplication
    MatrixB33.at<double>(0,0) = 1;
    MatrixB33.at<double>(1,1) = 1+dFL*dFL;
    MatrixB33.at<double>(2,2) = 1;
    //Set matrix B of multiplication
    MatrixC33.at<double>(0,0) = 1/dFx;
    MatrixC33.at<double>(1,1) = 1/dFy;
    MatrixC33.at<double>(0,2) = -dCx/dFx;
    MatrixC33.at<double>(1,2) = -dCy/dFy;
    MatrixC33.at<double>(2,2) = 1;
    
    //4. Inverse matrix B and multiply with matrix A
    cv::Mat MatrixB33Inv = MatrixB33.inv();
    cv::Mat MatrixC33Inv = MatrixC33.inv();
    XMatrix = MatrixA43 * MatrixB33Inv * MatrixC33;
    //#endif
    
#ifdef DEBUG_MATRIX
    printf("h00 = %lf\t h01 = %lf\t h02 = %lf\n", XMatrix.at<double>(0,0), XMatrix.at<double>(0,1), XMatrix.at<double>(0,2));
    printf("h10 = %lf\t h11 = %lf\t h12 = %lf\n", XMatrix.at<double>(1,0), XMatrix.at<double>(1,1), XMatrix.at<double>(1,2));
    printf("h20 = %lf\t h21 = %lf\t h22 = %lf\n", XMatrix.at<double>(2,0), XMatrix.at<double>(2,1), XMatrix.at<double>(2,2));
    printf("h30 = %lf\t h31 = %lf\t h32 = %lf\n", XMatrix.at<double>(3,0), XMatrix.at<double>(3,1), XMatrix.at<double>(3,2));
#endif
    
    return XMatrix;
}
