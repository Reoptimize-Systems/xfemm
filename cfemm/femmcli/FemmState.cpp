#include "FemmState.h"

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
