/* Copyright 2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by
 * Linz Center of Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include "epproc.h"

#include "CMeshNode.h"
#include "FemmProblem.h"
#include "FemmReader.h"

#include <string>
#include <sstream>

using namespace femm;

ElectrostaticsPostProcessor::ElectrostaticsPostProcessor()
    : PostProcessor()
{
}

ElectrostaticsPostProcessor::~ElectrostaticsPostProcessor()
{
}

femm::ParserResult ElectrostaticsPostProcessor::parseSolution(std::istream &input, std::ostream &err)
{
    using femmsolver::CSMeshNode;
    using femmsolver::CSElement;
    // read in meshnodes;
    int k;
    input >> k;
    meshnodes.reserve(k);
    for(int i=0;i<k;i++)
    {
        meshnodes.push_back(std::make_unique<CSMeshNode>(CSMeshNode::fromStream(input,err)));
    }

    // read in elements;
    input >> k;
    meshelems.reserve(k);
    auto &labellist = problem->labellist;
    for(int i=0;i<k;i++)
    {
        CSElement elm = CSElement::fromStream(input,err);
        elm.blk = labellist[elm.lbl]->BlockType;
        meshelems.push_back(std::make_unique<CSElement>(elm));
    }

    // read in circuit data;
    // first clear original circuit data:
    auto &circproplist = problem->circproplist;
    circproplist.clear();
    input >> k;
    circproplist.reserve(k);
    for(int i=0;i<k;i++)
    {
        circproplist.push_back(
                    std::make_unique<CSCircuit>(CSCircuit::fromStream(input,err))
                    );
    }

    return femm::F_FILE_OK;
}

bool ElectrostaticsPostProcessor::OpenDocument(std::string solutionFile)
{
    std::stringstream err;
    problem = std::make_unique<FemmProblem>(FileType::ElectrostaticsFile);
    ElectrostaticsReader reader(problem.get(),this,err);
    if (reader.parse(solutionFile) != F_FILE_OK)
        return false;
    return true;
}
