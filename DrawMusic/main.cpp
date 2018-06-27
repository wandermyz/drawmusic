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
    rack.SetTone(0, 440);
    rack.SetTone(1, 880);
    rack.SetTone(2, 220);
    
    std::thread t(thread_worker);
    
    while (true)
    {
        char c = getchar();
        
        if (c == 'w')
        {
            rack.SetTone(0, rack.GetTone(0) * 1.1);
        }
        else if ( c == 's')
        {
            rack.SetTone(0, rack.GetTone(0) * 0.9);
        }
    }
    
    t.join();
    
    // pthread_attr_t attr;
    // pthread_attr_init(&attr);
    // sched_param param;
    // pthread_attr_getschedparam(&attr, &param);
    // param.sched_priority = 20;
    // pthread_attr_setschedparam(&attr, &param);
    //
    // pthread_t t;
    // pthread_create(&t, &attr, thread_worker, nullptr);
    //
    // pthread_join(t, nullptr);
    
    return 0;
}
