/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2002-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class mzed_lorenz : public object<mzed_lorenz>
{
public:
    MIN_DESCRIPTION{ "Generate a Lorenz attractor" };
    MIN_TAGS{ "chaos" };
    MIN_AUTHOR{ "mzed" };
    MIN_RELATED{ "roessler" };

    inlet<>  inlet_x{ this, "(float or bang) bang calculates or x coordinate of previous point in attractor" };
    inlet<>  inlet_y{ this, "(float) y coordinate of previous point in attractor" };
    inlet<>  inlet_z{ this, "(float) z coordinate of previous point in attractor" };
    inlet<>  inlet_h{ this, "(float) h timestep value for attractor" };
    outlet<> outlet_x{ this, "(float) x coordinate" };
    outlet<> outlet_y{ this, "(float) y coordinate" };
    outlet<> outlet_z{ this, "(float) z coordinate" };

    attribute<double> l_h{ this, "timestep", 0.01 };

    mzed_lorenz(const atoms& args = {}) 
    {
        l_xyz[0] = l_xyz[1] = l_xyz[2] = 0.6;

        if (args.size() > 0)
            l_xyz[0] = args[0];
        if (args.size() > 1)
            l_xyz[1] = args[1];
        if (args.size() > 2)
            l_xyz[2] = args[2];
        if (args.size() > 3)
            l_h = args[3];
    }

    message<> ints
    {
        this, "float", "set variable",
        MIN_FUNCTION
        {
            switch (inlet)
            {
                case 0:
                    l_xyz[0] = args[0];
                case 1:
                    l_xyz[0] = args[0];
                case 2:
                    l_xyz[2] = args[0];
                default:
                    assert(false);
            }
            return {};
        }
    };

    message<> bang
    {
        this, "bang", "Calculate the next point.",
        MIN_FUNCTION
        {
            double xNew;
            double yNew;
            double zNew;

            // calculate the attractor
            zNew = l_xyz[2] + (l_h * ((l_xyz[0] * l_xyz[1]) - ((8.0 / 3.0) * l_xyz[2])));
            yNew = l_xyz[1] + (l_h * ((28.0 * l_xyz[0] - l_xyz[1]) - (l_xyz[0] * l_xyz[2])));
            xNew = l_xyz[0] + ((l_h * 10.0) * (l_xyz[1] - l_xyz[0]));

            // save the calculated values
            l_xyz[0] = xNew;
            l_xyz[1] = yNew;
            l_xyz[2] = zNew;

            // output the calculated values
            outlet_z.send(zNew);
            outlet_y.send(yNew);
            outlet_x.send(xNew);
            return {};
        }
    };


    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup
    { 
        this, "maxclass_setup",
        MIN_FUNCTION 
        {
            cout << "lorenz object by Michael F. Zbyszyński, v3.0 ©2002-2021" << endl;
            return {};
        }
    };

private:
    double l_xyz[3];
};

MIN_EXTERNAL(mzed_lorenz);
