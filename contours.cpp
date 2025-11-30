// Include the necessary headers
// https://www.youtube.com/watch?v=2FYm3GOonhk&t=3683s
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;
cv::Point startPoint(200, 240);
cv::Point endPoint(630, 240);
cv::Mat frame,imgGray,imgCanny,imgDil,imgErode,imgBlur,imgErod;
int hmin=0, smin=110,vmin=153;
int hmax=19, smax=240,vmax=255;

void getContours(Mat imgErod, Mat frame){

    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;

    findContours(imgErod,contours,hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(imgErod,contours,-1,cv::Scalar(255,0,255),2);
    for (int i=0; i<contours.size();i++)
    {
        int area=contourArea(contours[i]);
        std::cout << area<<std::endl;

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());
        string objectType;
        // if (area>2000)
        {
            float peri=arcLength(contours[i], true);
            approxPolyDP(contours[i],conPoly[i],0.02 * peri,true);
            // drawContours(frame,conPoly,i,cv::Scalar(255,0,255),2);
            std::cout << conPoly[i]<<std::endl;
            boundRect[i]=boundingRect(conPoly[i]);
            
            int objCor =(int)conPoly[i].size();
            // if (objCor==3) {objectType="Car";}
            // if (objCor==4) {objectType="Car";}
            if (objCor>4) {objectType="Car";}
            // drawContours(imgErod,conPoly,i,cv::Scalar(255,0,255),2);
            rectangle(frame,boundRect[i].tl(),boundRect[i].br(), Scalar(0,255,0),3);
            cv::Point center_point(boundRect[i].x + boundRect[i].width / 2, boundRect[i].y + boundRect[i].height / 2);
            int radius = 5;
            cv::Scalar color(0, 0, 255);
            cv::circle(frame, center_point, radius, color, -1);
            putText(frame,objectType, {boundRect[i].x,boundRect[i].y-5},FONT_HERSHEY_DUPLEX,0.75,cv::Scalar(0,0,255),2);
            putText(frame, to_string(area), cv::Point(boundRect[i].x, boundRect[i].y - 15),cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(0, 0, 255), 2);




        
            


            

            // putText(frame,objectType, {boundRect[i].x,boundRect[i].y-5},FONT_HERSHEY_DUPLEX,0.75,cv::Scalar(0,0,255),2);


        }
    }


}

void main() {
    cv::VideoCapture cap("E:\\c++\\parkingsystem\\video\\carsvid.mp4");

    while (true) {
        cv::Mat frame;
        cap >> frame;  // Capture frame from camera
        //Preprocessing
        cvtColor(frame,imgGray,cv::COLOR_BGR2GRAY);
        GaussianBlur(imgGray,imgBlur, cv::Size(3,3),3,0);
        Canny(imgBlur,imgCanny,25,75);
        Mat kernel=getStructuringElement(MORPH_RECT,Size(3,3));
        dilate(imgCanny,imgDil,kernel,cv::Point(-1,-1),2);
        erode(imgDil,imgErod,kernel,cv::Point(-1,-1),1);

        getContours(imgErod, frame);

        cv::imshow("frame", frame);
        // cv::imshow("imgGray", imgGray);
        // cv::imshow("imgBlur", imgBlur);
        // cv::imshow("imgCanny", imgCanny);
        // cv::imshow("imgDil", imgDil);
        cv::imshow("ImgErod", imgErod);
        if (cv::waitKey(30) == 27) {
        break;

        }
    }
    cv::destroyAllWindows();
    }

