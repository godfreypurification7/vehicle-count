// https://docs.opencv.org/4.x/d1/dc5/tutorial_background_subtraction.html
#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;
int count_line_position=550;
int min_width_react=80;
int min_height_react=80;

int globalWidth = 640;
int globalHeight = 480;
int iterations = 0;
cv::Point startPoint(100, 100);
cv::Point endPoint(400, 400);

cv::Mat frame, fgMask, fgMaskThresh, framesize, fgMaskErod, hstacked, hstackedImage1, fgMaskdilate, hstackedImage, foregroundPart, frameCopy2;
int main() {
    // Open a video capture object
    cv::VideoCapture cap("E:\\c++\\parkingsystemdec24\\video\\video.mp4");
    std::cout << "Video File Path: " << "E:\\c++\\parkingsystemdec24\\video\\video.mp4" << std::endl;

    // cv::VideoCapture cap("E:\\c++\\parkingsystem\\video\\video.mp4");

    // Check if the video is opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the video file." << std::endl;
        return -1;
    }
    int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');  // Use 'XVID' codec
    double fps = 30.0;  // Frames per second
    cv::Size frameSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    cv::VideoWriter videoWriter("output_video.avi", codec, fps, frameSize);
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open the output video file for writing." << std::endl;
        return -1;
    }


    // Create a background subtractor using MOG2 method
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor = cv::createBackgroundSubtractorMOG2();
    // Set specific parameters
    bgSubtractor->setHistory(2);
    bgSubtractor->setVarThreshold(16.0);
    bgSubtractor->setDetectShadows(true);
    while (true) {
        cv::Mat frame;
        cap >> frame;  // Capture frame from camera

        // Break the loop if capturing is unsuccessful
        if (frame.empty()) {
            std::cerr << "Error: Failed to capture frame." << std::endl;
            break;
        }
        cv::Point startPoint(180, 300);
        cv::Point endPoint(630, 300);
        
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));

        // cv::resize(frame, frame, cv::Size(desiredWidth, desiredHeight));
        // std::cout << "Size of framesize: " << frame.size() << std::endl;
        // std::cout << "Channel of framesize: " << frame.channels() << std::endl;
        // std::cout << "data of framesize: " << frame.type() << std::endl;
        // Apply the background subtractor to obtain a foreground mask
        bgSubtractor->apply(frame, fgMask);
         //cv::GaussianBlur(fgMask, fgMask, cv::Size(5, 5), 1.5);
        // std::cout << "Size of framesize: " << fgMask.size() << std::endl;
        // std::cout << "channel of framesize: " << fgMask.channels() << std::endl;
        // std::cout << "data of framesize: " << fgMask.type() << std::endl;
        cv::threshold(fgMask, fgMaskThresh, 250, 255, cv::THRESH_BINARY);
        cv::erode(fgMaskThresh, fgMaskErod, element);
        // ,cv::Point(-1,-1),1);
            // cv::erode(inputImage, erodedImage, element);

        cv::erode(fgMaskErod, fgMaskdilate, element);
        // ,cv::Point(-1,-1),2);
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec2f> lines;

        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat frameCopy = frame.clone();
        // cv::line(frameCopy, startPoint, endPoint, cv::Scalar(255, 0255), 2, cv::LINE_AA);
        cv::line(frameCopy, cv::Point(25, count_line_position), cv::Point(1200, count_line_position), cv::Scalar(255, 127, 0), 3);
        // cv::drawContours(frame,contours,-1,(0,0,255),1) ;
        std::vector<std::vector<cv::Point>> filtered_contours;
        for (size_t i = 0; i < contours.size(); i++)
        {
            // Calculate the area of the contour
            double area = cv::contourArea(contours[i]);
            // <<std::endl;
            // std::cout << "Contour " << i + 1 << " Area: " << area << std::endl;


             // Check if the area is greater than 400
            if (area > 8000)
                //&& area < 9000)
            {
                // Add the contour to the filtered vector
                filtered_contours.push_back(contours[i]);
                
            }
        }
        // Draw the bounding rectangles for the filtered contours
        for (size_t i = 0; i < filtered_contours.size(); i++)
        {

            // Find the bounding rectangle for the contour
            cv::Rect rect = boundingRect(filtered_contours[i]);

            cv::rectangle(frameCopy, rect, cv::Scalar(0, 0, 255), 3, 8, 0);
            cv::Point center_of_rect = (rect.br() + rect.tl()) * 0.5;
            // cv::circle(frameCopy, center_of_rect, 10, cv::Scalar(0, 255, 255),-1);
            // cv::circle(frame, center_of_rect, 10, cv::Scalar(0, 255, 255), -1);

            // cv::putText(frameCopy, "Car", rect, cv::FONT_HERSHEY_COMPLEX_SMALL, 2, (0, 0, 255), 2);
            // cv::putText(frameCopy,"Car", rect.tl().x, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
            cv::String text = "Car";
            cv::Point position(50, 100);  // Use cv::Point for the third argument
            int fontFace = cv::FONT_HERSHEY_SIMPLEX;
            double fontScale = 1.0;
            cv::Scalar color(255, 255, 255);  // White color
            int thickness = 2;
            int lineType = cv::LINE_AA;
            cv::putText(frameCopy, text, rect.tl(), fontFace, fontScale, color, thickness, lineType);
            // cv::circle(frameCopy, cv::Point center, 3, cv::Scalar(0, 255, 255), 3);
            // cv::putText(frame, std::to_string(contourArea(contours[i])), rect.tl(), fontFace, fontScale, color, thickness, lineType);

         // cv::putText(image, text, position, fontFace, fontScale, color, thickness, lineType);


            // Draw the rectangle on the original image
            // cv::rectangle (frameCopy,rect, (255, 255, 255), 2);
            // Calculate center point

           // Draw a circle at the center point
            // cv::circle(frameCopy, circle, 3, cv::Scalar(0, 255, 255), 3);
        }
        // foregroundPart
        cv::cvtColor(fgMask, fgMask, cv::COLOR_GRAY2RGB);
        cv::bitwise_and(frame,fgMask,foregroundPart);

        // std::cout << "frame size: " << frame.size() <<"Type"<<frame.type()<<"channel"<<frame.channels()<<std::endl;
        // std::cout << "fgMask size: " << foregroundPart.size() <<"Type"<<foregroundPart.type()<<"channel"<<foregroundPart.channels()<<std::endl;
        // std::cout << "frameCopy2 size: " << frameCopy.size() << "Type"<<frameCopy.type()<<"channel"<<frameCopy.channels()<<std::endl;
        videoWriter.write(frame);

        cv::hconcat(frame, foregroundPart, hstacked);
        cv::hconcat(hstacked, frameCopy, hstackedImage);


        cv::resize(hstackedImage, hstackedImage1, cv::Size(1300, 300));
        cv::imshow("All Three Results", hstackedImage1);

        // Further processing or visualization can be done with fgMask

            // Display the original frame and the foreground mask
            // cv::imshow("Original Frame", frameCopy2);
            // cv::imshow("Foreground Mask", hstacked);
            // cv::imshow("Rectangle",frameCopy);

            // Break the loop if the user presses 'Esc'
             // Break the loop if the user presses 'Esc'
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    // Release the video capture object
    cap.release();
    videoWriter.release();


    // Close all windows
    cv::destroyAllWindows();

    return 0;
}



