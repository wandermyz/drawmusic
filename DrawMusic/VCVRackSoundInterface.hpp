//
//  VCVRackSoundInterface.hpp
//  DrawMusic
//
//  Created by Yang Liu on 6/24/18.
//  Copyright © 2018 PixelsRhythm. All rights reserved.
//

#ifndef VCVRackSoundInterface_hpp
#define VCVRackSoundInterface_hpp

#include "SoundInterface.hpp"
#include <memory>
#include <thread>

class VCVRackSoundInterface : public SoundInterface
{
public:
    static const int NUM_CHANNELS = 8;
    static const int SAMPLE_RATE = 48000;
    static const int BLOCK_SIZE = 512;

    VCVRackSoundInterface();
    ~VCVRackSoundInterface();
    
    virtual void SetTone(int channel, float frequency);
    virtual void SetLevel(int channel, float level);
    virtual float GetTone(int channel);
    virtual void Update(long long deltaMicro);

private:
    std::mutex toneMutex;
    float tones[NUM_CHANNELS];
    float levels[NUM_CHANNELS];
    double steps[NUM_CHANNELS];
    double angleDelta[NUM_CHANNELS];
    
    float input[BLOCK_SIZE * NUM_CHANNELS];
    float output[BLOCK_SIZE * NUM_CHANNELS];
    std::unique_ptr<struct BridgeClient> bridgeClient;
};

#endif /* VCVRackSoundInterface_hpp */
