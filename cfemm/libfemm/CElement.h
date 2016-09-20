#ifndef FEMM_CELEMENT_H
#define FEMM_CELEMENT_H

#include <string>
#include "femmcomplex.h"

namespace femm {

/**
 * @brief The CElement class holds solution elements.
 *
 * fpproc reads the elements as follows:
 */
class CElement
{
    public:

        int p[3];
        int blk,lbl;
        CComplex B1,B2;
        CComplex b1[3],b2[3];
        double magdir;
        CComplex ctr;
        double rsqr;
        int n[3];  // 3 ints to store elem's neighbouring elements

    private:
};

}
#endif
