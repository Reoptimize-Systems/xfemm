#ifndef FEMM_CCPOINTPROP_H
#define FEMM_CCPOINTPROP_H

#include <string>
#include "femmcomplex.h"

namespace femm {

/**
 * @brief The CPointProp class
 * Contains data defined in section 2.1 of the .FEM file format ("[PointProps]").
 */
class CPointProp
{
    public:

        CPointProp();

        // <PointName>
        std::string PointName;
        // <I_re>, <I_im>
        double Jr,Ji;            // applied point current, A
        // <A_re>, <A_im>, magnetic vector potential
        double Ar,Ai;                // prescribed nodal value;

    private:
};

}
#endif
