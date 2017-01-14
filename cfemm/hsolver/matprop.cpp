
#include "hmesh.h"
#include "fparse.h"

#include <string>
#include <iostream>
using std::string;
using namespace femm;



CHConductor CHConductor::fromStream(std::istream &input, std::ostream &err)
{
    CHConductor prop;

    if( expectToken(input, "<beginconductor>", err) )
    {
        string token;
        while (input.good() && token != "<endconductor>")
        {
            nextToken(input, &token);

            if( token == "<tc>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.V, err);
                continue;
            }

            if( token == "<qc>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.q, err);
                continue;
            }

            if( token == "<conductortype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.CircType, err);
                continue;
            }

            if ( token != "<endconductor>")
                err << "CHConductor: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CHConductor::toStream(std::ostream &out) const
{
    out << "<BeginConductor>\n";
    out << "<Tc>" << V << "\n";
    out << "<qc>" << q << "\n";
    out << "<ConductorType>" << CircType << "\n";
    if (!CircName.empty())
        out << "<ConductorName> =\"" << CircName << "\"\n";
    out << "<EndConductor>\n";
}
