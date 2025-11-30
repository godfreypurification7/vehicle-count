// https://www.youtube.com/watch?v=6kZftXunlTY&t=1834s
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

int width = 600;
int height = 400;
 int minRectWidth = 80;
int minRectHeight = 80;
double alpha = 4.0; // Scaling factor for intensity (adjust as needed)
double beta = 30.0; // Intensity offset (adjust as needed)
int count_line_position= 650;
int vehicleCount = 0;

cv::Mat frame, fgMask, fgMaskThresh, fgMaskErod, fgMaskDilate,Grayframe,Blurframe,Threshframe,brightenedImage,diff;
// void verties (){
//     cv::VideoWriter out("output_video.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), cap.get(cv::CAP_PROP_FPS),cv::Size(width, height));

//     std::vector<cv::Point> vertices = {
//         cv::Point(660, 263),  // Vertex 1
//         cv::Point(1203, 623), // Vertex 2
//         cv::Point(0, 569)     // Vertex 3
//     };

//     cv::Mat mask = cv::Mat::zeros(cv::Size(width, height), CV_8UC1);
//     cv::fillConvexPoly(mask, vertices, cv::Scalar(255, 255, 255), cv::LINE_8);
// }

int main() {
    cv::VideoCapture cap("E:\\c++\\parkingsystemdec24\\video\\video.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the video file." << std::endl;
        return -1;
    }

    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor = cv::createBackgroundSubtractorMOG2();
    bgSubtractor->setHistory(2);
    bgSubtractor->setVarThreshold(16.0);
    bgSubtractor->setDetectShadows(true);

    

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "End of video stream or empty frame." << std::endl;
            break;
        }

        // cv::resize(frame, frame, cv::Size(width, height));
        cv::cvtColor(frame,Grayframe,cv::COLOR_BGR2GRAY);
        // cv::GaussianBlur(, , (3,3),int(1),int(1),4);
        cv::GaussianBlur(Grayframe, Blurframe, cv::Size(3, 3), 5, 5, 1);
        cv::threshold(Blurframe,Threshframe,90,255,cv::THRESH_BINARY);
        bgSubtractor->apply(Blurframe, fgMask);
        // std::cout<<fgMask.channels()<<std::endl;
        // std::cout<<"Blurframe:"<<Blurframe.channels()<<std::endl;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::erode(fgMask, fgMaskErod, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        cv::erode(fgMaskErod, fgMaskDilate, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
        cv::convertScaleAbs(fgMaskDilate, brightenedImage, alpha, beta);
        cv::absdiff(fgMaskErod, fgMaskDilate, diff);

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
            if (center_of_rect.y > count_line_position && center_of_rect.y < count_line_position + 10) {
                vehicleCount++;
                std::cout << "Vehicle Count: " << vehicleCount << std::endl;
           
            cv::Point center_of_rect = (rect.br() + rect.tl()) * 0.5;
            vehicleCount++;
            cv::String text = "Car";
            double area = cv::contourArea(contours[i]);
            cv::Point position(50, 100);  // Use cv::Point for the third argument
            int fontFace = cv::FONT_HERSHEY_SIMPLEX;
            double fontScale = 1.0;
            cv::Scalar color(255, 255, 255);  // White color
            int thickness = 2;
            int lineType = cv::LINE_AA;
            cv::putText(frameCopy, text, rect.tl(), fontFace, fontScale, color, thickness, lineType);
            cv::circle(frameCopy, cv::Point(center_of_rect), 3, cv::Scalar(0, 255, 255), 3);
            }
        }
            // cv::putText(frameCopy, std::to_string(area), rect.tl() - cv::Point(10, 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);
            cv::putText(frameCopy, "VEHICLE COUNT:"+std::to_string(vehicleCount), cv::Point(30, 70), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);

        
        // cv::imshow("diff", diff);
        // cv::imshow("brightenedImage", brightenedImage);
        cv::imshow("Original Frame", frameCopy);

        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release();
    // out.release();
    cv::destroyAllWindows();

    return 0;
}
