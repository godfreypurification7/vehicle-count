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
    

    findContours(imgErod,contours,hierarchy,RETR_LIST, CHAIN_APPROX_SIMPLE);
    // drawContours(frame,contours,-1,cv::Scalar(255,0,255),2);
    for (int i=0; i<contours.size();i++)
    {
        int area=contourArea(contours[i]);
        std::cout << area<<std::endl;

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());
        string objectType;
        if (area > 5000 && area < 9000)
        // if (area < 8000)
        {
            float peri=arcLength(contours[i], true);
            approxPolyDP(contours[i],conPoly[i],0.02 * peri,true);
            drawContours(frame,contours,-1,cv::Scalar(255,0,255),2);
            std::cout << conPoly[i]<<std::endl;
            boundRect[i]=boundingRect(conPoly[i]);
            
            // int objCor =(int)conPoly[i].size();
            // if (objCor==3) {objectType="Car";}
            // if (objCor==4) {objectType="Car";}
            // if (objCor>4) {objectType="Car";}
            // drawContours(frame,conPoly,i,cv::Scalar(255,0,255),2);
            rectangle(frame,boundRect[i].tl(),boundRect[i].br(), Scalar(0,255,0),3);
            // cv::Point center_point(boundRect[i].x + boundRect[i].width / 2, boundRect[i].y + boundRect[i].height / 2);
            // int radius = 5;
            // cv::Scalar color(0, 0, 255);
            // cv::circle(frame, center_point, radius, color, -1);
            // putText(frame,objectType, {boundRect[i].x,boundRect[i].y-5},FONT_HERSHEY_PLAIN,0.75,cv::Scalar(0,0,255),2);
            // putText(frame, to_string(area), cv::Point(boundRect[i].x, boundRect[i].y - 15),cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(0, 0, 255), 2);
            putText(frame, to_string(area), cv::Point(boundRect[i].x, boundRect[i].y - 15), cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(0, 0, 255), 2);


            // putText(frame,objectType, {boundRect[i].x,boundRect[i].y-5},FONT_HERSHEY_DUPLEX,0.75,cv::Scalar(0,0,255),2);


        }
    }


}

int main() {
    // Read the input video
    VideoCapture cap("E:\\c++\\parkingsystem\\video\\video.mp4");
    if (!cap.isOpened()) {
        std::cout << "Error opening video file" << std::endl;
        return -1;
    }

    // Get the video properties
    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));

    // Define the output video
    VideoWriter out("output_video.mp4", VideoWriter::fourcc('M','P','4','V'), fps, Size(width, height));

    // Define the vertices of the triangle (example coordinates)
    std::vector<Point> vertices;
    vertices.push_back(Point(660, 263));  // Vertex 1
    vertices.push_back(Point(1203, 623));  // Vertex 2
    vertices.push_back(Point(0, 569));  // Vertex 3

    // Create a mask with the same size as the frame
    Mat mask = Mat::zeros(Size(width, height), CV_8UC1);

    // Fill the mask with a white triangle
    fillConvexPoly(mask, vertices, Scalar(255, 255, 255), LINE_8);

    // Loop over the frames
    Mat frame, result;
    while (true) {
        // Read the next frame
        cap >> frame;

        // Check if the frame is empty
        if (frame.empty()) {
            break;
        }

        // Bitwise AND operation to extract the ROI
        bitwise_and(frame, frame, result, mask);
        cvtColor(result,imgGray,cv::COLOR_BGR2GRAY);
        GaussianBlur(imgGray,imgBlur, cv::Size(3,3),3,0);
        Canny(imgBlur,imgCanny,25,75);
        Mat kernel=getStructuringElement(MORPH_RECT,Size(3,3));
        dilate(imgCanny,imgDil,kernel,cv::Point(-1,-1),2);
        erode(imgDil,imgErod,kernel,cv::Point(-1,-1),1);
        
        getContours(imgErod, frame);

        // Write the cropped frame to the output video
        out << result;

        // Display the original frame and the cropped frame
        imshow("Original Frame", frame);
        // imshow("Cropped Frame", result);

        // Press ESC to exit
        char c = static_cast<char>(waitKey(25));
        if (c == 27) {
            break;
        }
    }

    // Release the video objects
    cap.release();
    out.release();

    // Destroy all windows
    destroyAllWindows();

    return 0;
}