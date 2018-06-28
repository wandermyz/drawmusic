//
//  VCVRackSoundInterface.cpp
//  DrawMusic
//
//  Created by Yang Liu on 6/24/18.
//  Copyright © 2018 PixelsRhythm. All rights reserved.
//

#include "VCVRackSoundInterface.hpp"
#include <memory.h>
#include <assert.h>
#include "../3rdparty/vcvbridge/common/client.cpp"

VCVRackSoundInterface::VCVRackSoundInterface() : bridgeClient(std::make_unique<BridgeClient>())
{
    memset(tones, 0, sizeof(tones));
    memset(steps, 0, sizeof(steps));
    
    bridgeClient->setPort(0);
    bridgeClient->setSampleRate(SAMPLE_RATE);
}

VCVRackSoundInterface::~VCVRackSoundInterface()
{
    
}

void VCVRackSoundInterface::SetTone( int channel, float frequency )
{
    assert(channel < NUM_CHANNELS);
    
    toneMutex.lock();
    tones[channel] = frequency;
    angleDelta[channel] = frequency / SAMPLE_RATE * 2.0 * 3.1415926525;
    toneMutex.unlock();
}

float VCVRackSoundInterface::GetTone(int channel)
{
    assert(channel < NUM_CHANNELS);
    
    toneMutex.lock();
    float freq = tones[channel];
    toneMutex.unlock();
    
    return freq;
}

void VCVRackSoundInterface::Update(long long deltaMicro)
{
    toneMutex.lock();
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int c = 0; c < NUM_CHANNELS; c++)
        {
            float sample = 0;
            
            if (tones[c] > 0)
            {
                sample = sin(steps[c]);
            }
            input[i * NUM_CHANNELS + c] = sample;
            steps[c] += angleDelta[c];
        }
    }
    toneMutex.unlock();
    
    bridgeClient->processStream(input, output, BLOCK_SIZE);
}
