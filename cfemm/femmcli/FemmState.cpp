#include "FemmState.h"

#include "fmesher.h"
#include "fpproc.h"

#include <memory>


const std::shared_ptr<femm::FemmProblem> femmcli::FemmState::femmDocument()
{
    return current.document;
}

void femmcli::FemmState::setDocument(std::shared_ptr<femm::FemmProblem> doc)
{
    // invalidate current state
    close();
    current.document = doc;
}

const std::shared_ptr<FPProc> femmcli::FemmState::getFPProc()
{
    if (!current.postprocessor)
    {
        current.postprocessor = std::make_shared<FPProc>();
    }
    return current.postprocessor;
}

const std::shared_ptr<fmesher::FMesher> femmcli::FemmState::getMesher()
{
    if (!current.mesher || current.mesher->problem != current.document)
    {
        current.mesher = std::make_shared<fmesher::FMesher>(current.document);
    }
    return current.mesher;
}

void femmcli::FemmState::invalidateSolutionData()
{
    current.postprocessor.reset();
}

void femmcli::FemmState::close()
{
    current.document.reset();
    current.mesher.reset();
    current.postprocessor.reset();
}
