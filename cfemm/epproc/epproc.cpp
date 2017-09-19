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
    for(int i=0;i<k;i++)
    {
        CSElement elm = CSElement::fromStream(input,err);
        elm.blk = problem->labellist[elm.lbl]->BlockType;
        meshelems.push_back(std::make_unique<CSElement>(elm));
    }

    // read in circuit data;
    input >> k;
    for(int i=0;i<k;i++)
    {
        //fgets(s,1024,fp);
        //sscanf(s,"%lf  %lf",&circproplist[i].V,&circproplist[i].q);
    }

    return femm::F_FILE_OK;
}
