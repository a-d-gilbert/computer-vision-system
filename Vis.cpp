#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "Visi.h"
#include <vector>

//#include <fstream>
//#define logger "log.txt"

using namespace cv;
using namespace std;
//ofstream fout;

//*********************************************************************************************************************
//*********************************************************************************************************************

Mat vRecog::filter(Mat image, Mat &image4hist)//finished
{

    //Mat hsv,eroded,threshed,dilated,gaus,edges;

    cvtColor(image, hsv, CV_BGR2HSV);//conversion to hsv
    inRange(hsv, Scalar(0,150,150), Scalar(95,255,255), threshed); //thresholding image
    //inRange(hsv, Scalar(0,0,210), Scalar(100,100,255), threshed); //thresholding image
    erode(threshed, eroded, getStructuringElement(0, Size(3,3)));//eroding and dilating image to
    dilate(eroded,dilated , getStructuringElement(0, Size(3,3)));//smooth image
    GaussianBlur(dilated, gaus, Size(3,3), 1.5, 1.5);//using gaussian filter to remove noise
    Canny(gaus, edges, 100, 200, 3);//using canny edge detector to define edge

    image4hist = hsv;

    return edges;

}

//*********************************************************************************************************************
//*********************************************************************************************************************

vector<vector<Point> > vRecog::genSmallBlobRemover(Mat &edges)//finished
{

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<int> small_blobs;
    double contour_area;
    double threshold = 500;

    findContours( edges, contours, hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );//defining contours from edge detector

    if ( !contours.empty())
    {

        for (size_t i=0; i<contours.size(); ++i)
        {

            contour_area = contourArea(contours[i]) ;

            if ( contour_area < threshold)
            {

                small_blobs.push_back(i);

            }

         }

     }

    for (size_t i=0; i < small_blobs.size(); ++i)
    {

        drawContours(edges, contours, small_blobs[i], Scalar(0),-1, 8);
        findContours( edges, contours2, hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );

    }

    return contours2;

}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::rectangleIdent(vector<vector<Point> > contours2, vector<Point2f> &centR,
                            vector<double> &rAreas, Rect &rect1)//finished
{

    vector<Point> approxR;
    vector<Moments> M(contours2.size());//storage for keypoints
    vector<Point2f> centersR;
    vector<double> rectArea;
    threshold2 = 800;
    threshold3 = 15000;


    for ( unsigned int i = 0 ; i < contours2.size() ; i++)
    {

        approxPolyDP(contours2[i],approxR,arcLength(contours2[i],true)*0.02,true);

        if( contourArea(contours2[i]) > threshold2 && contourArea(contours2[i]) < threshold3
                           && isContourConvex(approxR) && approxR.size() == 4 )
        {

            rect1 = boundingRect(contours2[i]);
            rectArea.resize(i,contourArea(contours2[i]));
            M[i]= moments( contours2[i], true );
            centersR.resize(i , Point2f((M[i].m10/M[i].m00),(M[i].m01/M[i].m00)));

        }

    }

    rAreas=rectArea;

    centR=centersR;

}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::circIdent(vector<vector<Point> > contours2, vector<Point2f> &centC,
                       vector<double>&cAreas, vector<int>&Verts,Rect &rect2)//finished
{

    vector<Point> approxC;
    vector<Moments> M(contours2.size());//storage for keypoints
    vector<Point2f> centersC;
    vector<double>circArea;
    vector<int>vertices;
    threshold2 = 800;
    threshold = 15000;

    for ( unsigned int i = 0 ; i < contours2.size() ; i++)
    {

        approxPolyDP(contours2[i],approxC,arcLength(contours2[i],true)*0.02,true);

        if( contourArea(contours2[i]) > threshold2 && contourArea(contours2[i]) < threshold3
                            && isContourConvex(approxC) && approxC.size() > 6 )
        {

            rect2 = boundingRect(contours2[i]);
            vertices.resize(i,approxC.size());
            circArea.resize(i,contourArea(contours2[i]));
            M[i] =  moments( contours2[i], true );
            centersC.resize(i , Point2f((M[i].m10/M[i].m00),(M[i].m01/M[i].m00)));

        }

    }

    cAreas=circArea;
    Verts=vertices;
    centC = centersC;

}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::tweenerRect(Mat &edges, vector<vector<Point> > contours2)//works but not cleanly or accurately
{

    vector<Point> approx;
    vector<Moments> M( contours2.size() );//storage for keypoints
    vector<Point2f> cp(contours2.size() );
    vector<float> dst(contours2.size());
    double threshold2 = 800;
    Scalar color( 255,255,255 );

    for ( unsigned int i = 0 ; i < contours2.size() ; i ++)
    {

        approxPolyDP(contours2[i],approx,arcLength(contours2[i],true)*0.02,true);

        if( contourArea(contours2[i]) > threshold2 && isContourConvex(approx) && approx.size() == 4 )
        {

            M[i] = moments( contours2[i], true );

            if ( i > 2 )
            {

                cp[i] = Point2f((((M[i-2].m10/M[i-2].m00)+(M[i].m10/M[i].m00))/2),
                               (((M[i-2].m01/M[i-2].m00)+(M[i].m01/M[i].m00))/2));

                dst[i] = float(sqrt( powf(((M[i-2].m10/M[i-2].m00)-(M[i].m10/M[i].m00)),2)
                                   + powf(((M[i-2].m01/M[i-2].m00)-(M[i].m01/M[i].m00)),2))/2);

                circle(edges, cp[i], (int)dst[i], color, 1, 8, 0 );
                cout << "between : \n" << cp[i] << endl;

            }

        }

    }

}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::tweenerCirc(Mat &edges, vector<vector<Point> > contours2)//works but not cleanly or accurately
{

    vector<Point> approx;
    vector<Moments> M( contours2.size() );//storage for keypoints
    vector<Point2f> centers( contours2.size() );//storage for center cordinates
    vector<Point2f> cp(contours2.size() );
    vector<float> dst(contours2.size());
    double threshold2 = 800;
    Scalar color( 255,255,255 );

    for ( unsigned int i = 0 ; i < contours2.size() ; i ++)
    {

        approxPolyDP(contours2[i],approx,arcLength(contours2[i],true)*0.02,true);

        if( contourArea(contours2[i]) > threshold2 && isContourConvex(approx) && approx.size() >= 8 )
        {

            M[i] = moments( contours2[i], true );
            centers[i] = Point2f((M[i].m10/M[i].m00),(M[i].m01/M[i].m00));

            if ( i > 2 )
            {

                cp[i] = Point2f((((M[i-2].m10/M[i-2].m00)+(M[i].m10/M[i].m00))/2),
                               (((M[i-2].m01/M[i-2].m00)+(M[i].m01/M[i].m00))/2));

                dst[i] = float(sqrt( powf(((M[i-2].m10/M[i-2].m00)-(M[i].m10/M[i].m00)),2)
                                   + powf(((M[i-2].m01/M[i-2].m00)-(M[i].m01/M[i].m00)),2))/2);

                cout << "between : \n" << cp[i] << endl;
                circle(edges, cp[i], (int)dst[i], color, 1, 8, 0 );

            }

        }

    }

}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::genRec(Mat &edges, Mat& origianl, vector<vector<Point> > contours2)//finished
{

    //fout.open(logger,ios::app);
    vector<Point> approx;
    vector<Moments> M( contours2.size() );//storage for keypoints
    vector<Point2f> centers( contours2.size() );//storage for center cordinates
    double threshold2 = 800;
    Scalar color( 255,255,255 );

    for ( unsigned int i = 0 ; i < contours2.size() ; i ++)
    {

        approxPolyDP(contours2[i],approx,arcLength(contours2[i],true)*0.02,true);

        if( contourArea(contours2[i]) > threshold2 && isContourConvex(approx) )
        {

            M[i] = moments( contours2[i], true );
            centers[i] = Point2f((M[i].m10/M[i].m00),(M[i].m01/M[i].m00));
            cout << "Center point : \n" << centers[i] << endl;
            //fout << "Center point : \n" << centers[i] << endl;
            cout << "Area : \n" << contourArea(contours2[i]) << endl;
            //fout << "Area : \n" << contourArea(contours2[i]) << endl;
            cout << "Arc length : \n" << arcLength( contours2[i],true) << endl;
            //fout << "Arc length : \n" << arcLength( contours2[i],true) << endl;
            //fout << "Vertices : \n" << approx.size() << endl;
            circle(edges,centers[i],6,color,-1,8,0);
            circle(edges,centers[i],15,color,1,8,0);
            circle(origianl,centers[i],6,color,-1,8,0);
            circle(origianl,centers[i],15,color,1,8,0);
            //fout.close();

        }
    }
}

//*********************************************************************************************************************
//*********************************************************************************************************************

void vRecog::histcheck(Rect rect4hist,Mat image4hist)//,int clrflag);//in prgress
{

    int hbins = 255;
    int sbins = 179;
    int channels[] = {0,1};
    int histSize[] = {hbins,sbins};
    int hist_w = 512;
    int hist_h = 400;
    MatND hist;
    float h_ranges[] = { 0, 255 };
    float s_ranges[] = { 0, 179 };
    const float* ranges[] = { h_ranges, s_ranges};
    Rect ROI = rect4hist;
    Mat image2chk;
    image2chk = image4hist(ROI);

    calcHist(& image4hist,1,channels, Mat(),hist,2,histSize,ranges,true,false);
    int bin_w = hist_w/histSize[0];
    int bin_w1 = hist_w/histSize[1];
    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    normalize(hist,hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    for( int i = 1; i < histSize[0]; i++ )
    {

        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                         Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                         Scalar( 255, 0, 0), 2, 8, 0  );

    }

    for( int i = 1; i < histSize[1]; i++ )
    {

        line( histImage, Point( bin_w1*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                         Point( bin_w1*(i), hist_h - cvRound(hist.at<float>(i)) ),
                         Scalar( 0, 255, 0), 2, 8, 0  );

    }

    namedWindow("hist of ROI",1 );
    imshow("hist of ROI", histImage );

}


