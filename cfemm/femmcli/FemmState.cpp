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
    return theFPProc;
}

std::shared_ptr<fmesher::FMesher> femmcli::FemmState::getMesher()
{
    if (!theFMesher || theFMesher->problem != femmDocument)
    {
        theFMesher = std::make_shared<fmesher::FMesher>(femmDocument);
    }
    return theFMesher;
}

void femmcli::FemmState::invalidateSolutionData()
{
    theFPProc.reset();
}
