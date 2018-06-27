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
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        return -1;
    }
    
    rack.SetTone(0, 440);
    rack.SetTone(1, 880);
    rack.SetTone(2, 220);
    
    std::thread audioThread(thread_worker);
    
    cv::Mat edges;
    cv::namedWindow("video",1);
    for(;;)
    {
        cv::Mat frame;
        cap >> frame; // get a new frame from camera
        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        // Canny(edges, edges, 0, 30, 3);
        cv::imshow("video", frame);
        
        int c = cv::waitKey(30);
        if (c == (int)'w')
        {
            rack.SetTone(0, rack.GetTone(0) * 1.1);
        }
        else if (c == (int)'s')
        {
            rack.SetTone(0, rack.GetTone(0) * 0.9);
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
