//
//  SoundInterface.hpp
//  DrawMusic
//
//  Created by Yang Liu on 6/23/18.
//  Copyright © 2018 PixelsRhythm. All rights reserved.
//

#ifndef SoundInterface_hpp
#define SoundInterface_hpp

#include <stdio.h>

class SoundInterface
{
public:
    virtual void SetTone( int channel, float frequency ) = 0;
    virtual float GetTone( int channel ) = 0;
    virtual void SetLevel( int channel, float level ) = 0;
    virtual void Update( long long deltaMicro ) = 0;
};

#endif /* SoundInterface_hpp */
