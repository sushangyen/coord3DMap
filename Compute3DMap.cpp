//********************************************************************
// File name: COMPUTE3DMAP.CPP
// Designer : Shang-Yen Su
// Create date: May. 12, 2017
// Updated date: May. 22, 2017
// Description:
// Calculate 3D coords of FPs
// Convention:
// Find good rectified FPs' pairs and compute their 3D coord
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

void compute3DMap(cv::Mat P1, cv::Mat P2, float CameraDistance, float ScreenPosition, int number, cv::vector<FeaturePt> fptFinalRef, cv::vector<FeaturePt> fptFinalTar){
    
    //Setting for writing the FPs' 3D coord data into the files
    FILE *c3DMapRef;
    char Map3DRefName[20] = "FPs/Map3DRef.txt";
    if((c3DMapRef = fopen(Map3DRefName, "w")) == NULL)
    {
        printf("File Not Existing...\n");
    }
    
    //Setting for writing the FPs' 3D coord data into the files
    FILE *c3DMapTar;
    char Map3DTarName[20] = "FPs/Map3DTar.txt";
    if((c3DMapTar = fopen(Map3DTarName, "w")) == NULL)
    {
        printf("File Not Existing...\n");
    }
    
    //Setting for writing the FPs' 3D coord data into the files
    FILE *c3DMap;
    char Map3DName[20] = "FPs/Map3D.txt";
    if((c3DMap = fopen(Map3DName, "w")) == NULL)
    {
        printf("File Not Existing...\n");
    }

    
    //Set the normal vector and a point on the screen
    Norm3D PlaneNorm = {0, -1, 0};
    Coords3D P0;
    P0.X = ScreenPosition * tan(RADIAN(10.0));
    P0.Y = ScreenPosition;
    P0.Z = 50;
    double TheD = PlaneNorm.X * P0.X +  PlaneNorm.Y * P0.Y +  PlaneNorm.Z * P0.Z;
    
    //Generate a matrix for pixel to world trandformation
    cv::Mat XMatrix = Pixel2World(ScreenPosition);
    
    fprintf(c3DMap, "Feature Numbers = %d\n", number);
    
    for(int p = 0; p < number; p++){
        cv::Mat PixelRef = cv::Mat::ones(3, 1, CV_64FC1);
        PixelRef.at<double>(0, 0) = fptFinalRef[p].u;
        PixelRef.at<double>(1, 0) = fptFinalRef[p].v;
        cv::Mat WorldCoordRef = XMatrix * PixelRef;
        
        double dWRef = WorldCoordRef.at<double>(3,0);
        cv::Point3f P2W3DRef = cv::Point3f( WorldCoordRef.at<double>(0,0)/dWRef, WorldCoordRef.at<double>(1,0)/dWRef, WorldCoordRef.at<double>(2,0)/dWRef);
        
        double tRef = TheD / (PlaneNorm.X * P2W3DRef.x + PlaneNorm.Y * P2W3DRef.y + PlaneNorm.Z * P2W3DRef.z);
        
        P2W3DRef.x = P2W3DRef.x * tRef;
        P2W3DRef.y = P2W3DRef.y * tRef;
        P2W3DRef.z = P2W3DRef.z * tRef;

        fprintf(c3DMapRef, "%lf\t%lf\t%lf\n", P2W3DRef.x, P2W3DRef.y, P2W3DRef.z);
        
        cv::Mat PixelTar = cv::Mat::ones(3, 1, CV_64FC1);
        PixelTar.at<double>(0, 0) = fptFinalTar[p].u;
        PixelTar.at<double>(1, 0) = fptFinalTar[p].v;
        cv::Mat WorldCoordTar = XMatrix * PixelTar;
        
        double dWTar = WorldCoordTar.at<double>(3,0);
        cv::Point3f P2W3DTar = cv::Point3f( WorldCoordTar.at<double>(0,0)/dWTar, WorldCoordTar.at<double>(1,0)/dWTar, WorldCoordTar.at<double>(2,0)/dWTar);
        
        double tTar = TheD / (PlaneNorm.X * P2W3DTar.x + PlaneNorm.Y * P2W3DTar.y + PlaneNorm.Z * P2W3DTar.z);
        
        P2W3DTar.x = P2W3DTar.x * tTar + CameraDistance;    // Translate the coord (The reference coord is at the left image)
        P2W3DTar.y = P2W3DTar.y * tTar;
        P2W3DTar.z = P2W3DTar.z * tTar;
        
        fprintf(c3DMapTar, "%lf\t%lf\t%lf\n", P2W3DTar.x, P2W3DTar.y, P2W3DTar.z);
        
        //Find disparity in the real world coord system
        double disparity =  P2W3DTar.x - P2W3DRef.x;
        
        //Compute the distance between the projected screen and the FP in the real world coord
        double dist = (ScreenPosition*disparity/(CameraDistance - disparity)) + ScreenPosition;
        
        //Find FPs' 3D coord
        cv::Mat Map3DRef = cv::Mat::ones(3, 1, CV_64FC1);
        Map3DRef.at<double>(0, 0) = P2W3DRef.x*dist/ScreenPosition;
        Map3DRef.at<double>(1, 0) = dist;
        Map3DRef.at<double>(2, 0) = P2W3DRef.z*dist/ScreenPosition;
        
        
        fprintf(c3DMap, "%lf\t%lf\t%lf\n", Map3DRef.at<double>(0, 0), Map3DRef.at<double>(1, 0), Map3DRef.at<double>(2, 0));

        /*
        fprintf(c3DMap, "Feature 3D Coordinate = %lf\t%lf\t%lf\n", Map3DRef.at<double>(0, 0), Map3DRef.at<double>(1, 0), Map3DRef.at<double>(2, 0));
        fprintf(c3DMap, "Descriptor =");
        for(int w = 0; w < 64; w++){
            fprintf(c3DMap, "%d ", fptFinalRef[p].BRISK_Descriptor[w]);
        }
        fprintf(c3DMap, "\n");
        */
        
        /* Method referred from P.453, Learning OpenCV <--- Cannot Work!
        cv::Mat Map3DRef = cv::Mat::ones(4, 1, CV_64FC1);
        Map3DRef.at<double>(0, 0) = P2W3DRef.x;
        Map3DRef.at<double>(1, 0) = P2W3DRef.z;
        Map3DRef.at<double>(2, 0) = disparity;
        
        cv::Mat Map3DCoordRef = Q * Map3DRef;

        double dW = Map3DCoordRef.at<double>(3,0);
        fprintf(c3DMap, "%lf\t%lf\t%lf\n", Map3DCoordRef.at<double>(0,0), Map3DCoordRef.at<double>(1,0), Map3DCoordRef.at<double>(2,0));
        */
    }

    fclose(c3DMapRef);
    fclose(c3DMapTar);
    fclose(c3DMap);
}
