#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <bits/stdc++.h>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;


int getFile(int argc, char** argv, Mat&& src, Mat&& img){
    if (argc < 2) { return -1; }
    const char* path = argc >= 3 ? argv[2] : "";
    const char* file = argv[1];

    int size1 = sizeof(path);
    int size2 = sizeof(file);

    string filename;
    filename = filename.append(path).append("/").append(file);
    // Loads an image
    img = (imread( samples::findFile( filename ) ));
    src = (imread( samples::findFile( filename ), IMREAD_GRAYSCALE ));
    return 0;
}

void getLines(Mat &src, Mat &dst, Mat &cdst, Mat &cdstP, vector<Vec2f> &lines, vector<Vec4i> &linesP){
    // Edge detection
    printf("hello");
    Canny(src, dst, 20, 100, 3);
    // Copy edges to the images that will display the results in BGR
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdst.copyTo(cdstP);
    // Standard Hough Line Transform
    HoughLines(dst, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
    HoughLinesP(dst, linesP, 1, CV_PI/180, 25, 500, 6 ); // runs the actual detection
}

void drawTheLines(Mat &cdst, vector<Vec2f> &lines){
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = (lines)[i][0], theta = (lines)[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
    }
}
void saveVerticalOnly(vector<Vec4i> &linesP){
    for(auto l = (linesP).begin(); l != (linesP).end();) {
        auto line = *l;
        if (pow((line[0] - line[2]), 2) > pow((line[1] - line[3])*0.06, 2)){
            (linesP).erase(l);
        }
        else {
            ++l;
        }
    }
}

void removeDuplicated(Mat &cdstP, vector<Vec4i> &linesP){
    std::sort(linesP.begin(), linesP.end(), [](Vec4i vec1, Vec4i  vec2)
    {
        return vec1[0] < vec2[0];
    });
    int i = 0;
    
    for(auto l = linesP.begin(); l != linesP.end();i++ ) {
        int j = 0;
        for(auto li = linesP.begin(); li != linesP.end();j++) {
            auto line = *l;
            if ((pow((*l)[0] - (*li)[0],2) < 400) && i!=j){
                printf("lx: %d ; lix: %d \n", (*l)[0], (*li)[0]);
                linesP.erase(li);
            }
            else {
                cv::line( cdstP, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(0,0,255), 3, LINE_AA);
                li++;
            }
        }
        l++;
    }

    for( size_t i = 0; i < linesP.size(); i++ ) {
        Vec4i l = linesP[i];
        printf("xCoord %d \n", l[0]);
        line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
}

vector<Rect> &getRects(vector<Rect> &rects, Mat &src, vector<Vec4i> &linesP){
    int s = linesP.size();
    if (s>0){
        rects.push_back(Rect(Point(linesP[s-1][0], linesP[s-1][1]), Point(src.size().width-1, linesP[s-1][3])));
        for(int i = 0; i < linesP.size() - 1; i++){
            Point tl = Point(linesP[i][0], max(linesP[i][1], linesP[i+1][1]));
            Point br = Point(linesP[i+1][2], min(linesP[i][3], linesP[i+1][3]));
            Rect rect = Rect(tl, br);
            rects.push_back(rect);
        }
        rects.push_back(Rect(Point(0, linesP[0][1]), Point(linesP[0][2], linesP[0][3])));
    }
    else{
        rects.push_back(Rect(Point(0, 0), Point(Point(src.size().width-1, src.size().height-1))));
    }
    return rects;
}

void saveRects(vector<Rect> &rects, Mat &src, char * filename){
    int i = 0;
    for(Rect rect : rects){
        if(rect.width > 5 && rect.height > 0){
            i++;
            Mat imgCropped = (src)(rect);
            auto ext = ".png";
            // string file(filename);

            string newFileName("cropped/");
            newFileName = newFileName.append(filename);
            fs::create_directories(newFileName);
            // mkdir(filename, S_IRWXU | S_IRGRP | S_IROTH );
            newFileName.append("/NUM").append(to_string(i)).append(ext); 
            imwrite(newFileName, imgCropped);
        }
    }
    printf("%d",((rects).size()));
}


int main(int argc, char** argv)
{
    // Declare the output variables
    Mat img, src, dst, cdst, cdstP;
    vector<Vec2f> lines; 
    vector<Vec4i> linesP;
    // lines->reserve(1000);
    // linesP->reserve(1000);

    if(getFile(argc, argv, move(src), move(img)) < 0) {return -1;}
    
    getLines(src, dst, cdst, cdstP, lines, linesP);
    drawTheLines(cdst, lines);
    
    // Probabilistic Line Transform
    
    saveVerticalOnly(linesP);

    removeDuplicated(cdstP, linesP);

    vector<Rect> rects;
    rects = getRects(rects, src, linesP);

    saveRects(rects, img, move((argv[1])));

    return 0;
}