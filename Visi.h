#ifndef VISI_H
#define VISI_H

using namespace cv;

class vRecog
{

    public:

        Mat edges;
        Mat hsv;
        Mat eroded;
        Mat threshed;
        Mat dilated;
        Mat gaus;
        Mat equal;
        //vector<vector<Point> > contours;
        vector <vector<Point> > contours2;
        //vector<Point2f>centR;
        //vector<Point2f>centC;
        vector <Point2f> cP;
        vector <Vec4i> hierarchy;
        vector <float> dst;
        vector <int> smallBlobs;
        Scalar color(int,int,int);
        double contourarea;
        double threshold;
        double threshold2;
        double threshold3;

        Mat filter(Mat, Mat &);
        vector <vector <Point> > genSmallBlobRemover(Mat&);
        void rectangleIdent(vector <vector<Point> > , vector <Point2f>& , vector<double>&, Rect&);
        void circIdent(vector <vector< Point> >,vector <Point2f>&,vector <double>&,vector<int>&, Rect&);
        void tweenerRect(Mat&,vector <vector <Point> >);
        void tweenerCirc(Mat&,vector <vector <Point> >);
        void genRec(Mat &, Mat&, vector <vector <Point> > );
        void histcheck(Rect ,Mat);

};


#endif // VISI_H
