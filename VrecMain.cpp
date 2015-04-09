//change to test script for committing to vision
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "Visi.h"
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

vRecog rec;

int main()
{

    bool loop = true;//storage for images and data
    Mat image,origianl,edges, hsv;
    vector <vector <Point> > contours2;
    vector <Point2f> centersC;
    vector <Point2f> centersR;
    vector <double> circArea;
    vector <double> rectArea;
    vector <int> cVertices;
    Rect brect4rect;
    Rect brect4circ;
    Scalar color( 255,255,255 );

    VideoCapture cap;//assigning camera and proportions
    cap.set(3, 320);
    cap.set(4, 240);
    cap.set(5,5);
    //cap.open("/home/alexander/Documents/QTcreator/VisisonRecognition/test1.avi");
    cap.open(0);//starting camera feed

    namedWindow("original",1);//creating window for unmodified image
    namedWindow("go bot go",1);//creating window fo filtered image

        while(loop == true)
        {

            try
            {

                cap>>origianl;
                cap>>image;// grabbing and saving frame from cam
                edges = rec.filter(image,hsv);
                contours2 = rec.genSmallBlobRemover(edges);
                //rec.genRec(edges,origianl,contours2);
                //cout <<"before : "<< contours2.size() << endl;
                rec.circIdent(contours2,centersC,circArea,cVertices,brect4circ);
                rec.rectangleIdent(contours2,centersR,rectArea,brect4rect);
                //rec.histcheck(brect4rect, hsv);
                //cout << "Circular Centers : " << centersC.size() << endl ;
                //cout << "Rectangle Centers : " << centersR.size() << endl ;

                    if(centersC.size() > centersR.size())
                    {

                        for (unsigned int i=0; i < centersC.size() ; i++)
                        {

                            cout << "Area Circle " << i << " : " << circArea[i] << endl;
                            cout << "Center Circle " << i << " : " << centersC[i] << endl;
                            cout << "Vertices Circles " << i << " : " << cVertices[i] << endl;
                            circle(edges,centersC[i],8,color,-1,8,0);

                        }

                    }

                    if(centersR.size() > centersC.size())
                    {

                        for (unsigned int i=0; i < centersR.size() ; i++)
                        {

                            cout << "Area Rectangle " << i << " : " << rectArea[i] << endl;
                            cout << "Center Rectangle " << i << " : " << centersR[i] << endl;
                            circle(edges,centersR[i],8,color,-1,8,0);

                        }

                    }

                    imshow("go bot go",edges);//showing filtered image
                    imshow("original",origianl);//showin unfiltered image

                  }

                catch (Exception& e)//error checking
                {

                    const char* err_msg = e.what();
                    cout << "exception caught: \n" << err_msg << std::endl;

                }

                char key = waitKey(33) & 0xFF;//checking for key press

                if (key == 'q')//press q to quit
                {

                    cap.release();
                    loop = false;

                }

        }

    return 0;

}


