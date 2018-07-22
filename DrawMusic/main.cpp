//
//  main.cpp
//  DrawMusic
//
//  Created by Yang Liu on 6/23/18.
//  Copyright © 2018 PixelsRhythm. All rights reserved.
//

#include "VCVRackSoundInterface.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <vector>

static VCVRackSoundInterface rack;

void thread_worker()
{
    std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point prev = t0;
    while (true)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds delta = std::chrono::duration_cast<std::chrono::microseconds>(t1 - prev);
        // std::cout << "delta: " << delta.count() / 1000.0 << std::endl;
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        rack.Update(delta.count());
        std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
        prev = t1;
        std::chrono::microseconds spent = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
        // std::cout << "spent: " << spent.count() / 1000.0 << std::endl;
        
        // usleep((double)rack.BLOCK_SIZE / (double)rack.SAMPLE_RATE * 1000.0);
        // usleep(10000);
        std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    }
}

int main(int argc, const char * argv[]) {
    cv::VideoCapture cap(1);
    // cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
    // cap.set(CV_CAP_PROP_FRAME_HEIGHT,360);
    
    rack.SetTone(0, 440);
    rack.SetTone(1, 880);
    rack.SetTone(2, 220);
    
    std::thread audioThread(thread_worker);
    
    cv::namedWindow("video");
    cv::moveWindow("video", 2560, 0);
    
    // cv::namedWindow("diff");
    // cv::moveWindow("diff", 2560, 600);

    cv::namedWindow("edge");
    cv::moveWindow("edge", 2560, 600);
    
    cv::namedWindow("delta");
    cv::moveWindow("delta", 2560, 1200);

    cv::Mat staticBack;
    cv::Mat prevFrame;
    
    for(;;)
    {
        cv::Mat fullFrame;
        cap >> fullFrame; // get a new frame from camera
        
        cv::Mat frame = fullFrame(cv::Rect(480, 270, 960, 540));
        
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);
        
        if (staticBack.empty() || prevFrame.empty())
        {
            staticBack = gray;
            prevFrame = gray;
            continue;
        }

        cv::Mat diffFrame;
        cv::absdiff(staticBack, gray, diffFrame);
        
        cv::Mat deltaFrame;
        cv::absdiff(prevFrame, gray, deltaFrame);

        cv::Mat threshFrame;
        cv::threshold(diffFrame, threshFrame, 15, 255, cv::THRESH_BINARY);
        // cv::dilate(threshFrame, threshFrame, cv::Mat());
        
        cv::Mat edgeFrame;
        // cv::GaussianBlur(gray, edgeFrame, cv::Size(3, 3), 0, 0);
        // cv::Sobel(gray, sobelFrame, CV_8U, 3, 3);
        cv::Canny(gray, edgeFrame, 0, 30);
        
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(threshFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for( std::vector<cv::Point>& contour : contours )
        {
            double area = cv::contourArea(contour);
            if (area < 10000)
            {
                continue;
            }
            
            cv::Rect rect = cv::boundingRect(contour);
            cv::rectangle(frame, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 3);
        }

        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        // Canny(edges, edges, 0, 30, 3);
        
        cv::imshow("video", frame);
        // cv::imshow("diff", threshFrame);
        cv::imshow("delta", deltaFrame);
        cv::imshow("edge", edgeFrame);
        
        cv::Mat threshFloatFrame;
        threshFrame.convertTo(threshFloatFrame, CV_32FC1, 1.0 / 255.0);
        float absDiff = cv::sum(threshFloatFrame)[0] / (threshFloatFrame.rows * threshFloatFrame.cols);
        // std::cout << absDiff << std::endl;

        cv::Mat deltaFloatFrame;
        deltaFrame.convertTo(deltaFloatFrame, CV_32FC1, 1.0 / 255.0);
        float movement = cv::sum(deltaFloatFrame)[0] / (deltaFloatFrame.rows * deltaFloatFrame.cols);

        cv::Mat edgeFloatFrame;
        edgeFrame.convertTo(edgeFloatFrame, CV_32FC1, 1.0 / 255.0);
        float density = cv::sum(edgeFrame)[0] / (edgeFloatFrame.rows * edgeFloatFrame.cols);
        std::cout << density << std::endl;
        
        prevFrame = gray;
        
        int c = cv::waitKey(30);
        
        rack.SetTone(0, 440 * (0.5 + 8 * absDiff) );
        rack.SetTone(1, 532.25 * (0.5 + 2 * absDiff) );
        rack.SetTone(2, 659.25 * (0.5 + 12 * absDiff) );
        rack.SetTone(3, 440 * (1 + 16 * movement));
        rack.SetTone(4, 659.25 * (1 + 8 * movement));
        
        
        rack.SetLevel(7, density);

        if (c == (int)'w')
        {
            // rack.SetTone(0, rack.GetTone(0) * 1.1);
        }
        else if (c == (int)'s')
        {
            // rack.SetTone(0, rack.GetTone(0) * 0.9);
        }
        else if ( c == (int)'r')
        {
            staticBack = gray;
        }
    }
    
    // while (true)
    // {
    //     char c = getchar();
    //
    //     if (c == 'w')
    //     {
    //         rack.SetTone(0, rack.GetTone(0) * 1.1);
    //     }
    //     else if ( c == 's')
    //     {
    //         rack.SetTone(0, rack.GetTone(0) * 0.9);
    //     }
    // }
    
    audioThread.join();
    
    return 0;
}
