#include "FemmState.h"

#include "fmesher.h"
#include "fpproc.h"

#include <memory>


std::shared_ptr<FPProc> femmcli::FemmState::getFPProc()
{
    if (!theFPProc)
    {
        theFPProc = std::make_shared<FPProc>();
    }
    // TODO: see if we need to invalidate this sometimes.
    return theFPProc;
}

std::shared_ptr<fmesher::FMesher> femmcli::FemmState::getFMesher()
{
    if (!theFMesher)
    {
        theFMesher = std::make_shared<fmesher::FMesher>();
    }
    // TODO: see if we need to invalidate this sometimes.
    return theFMesher;
}
