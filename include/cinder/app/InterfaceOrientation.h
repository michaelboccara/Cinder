#pragma once

namespace cinder { namespace app {

    enum InterfaceOrientation {
        Unknown					= 0,
        Portrait				= 1 << 0,
        PortraitUpsideDown		= 1 << 1,
        LandscapeLeft			= 1 << 2,
        LandscapeRight			= 1 << 3,
        PortraitAll				= (Portrait | PortraitUpsideDown),
        LandscapeAll			= (LandscapeLeft | LandscapeRight),
        All						= (PortraitAll | LandscapeAll)
    };

} }
