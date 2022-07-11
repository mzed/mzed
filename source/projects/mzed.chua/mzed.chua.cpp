/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2002-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class mzed_chua : public object<mzed_chua>
{
public:
    MIN_DESCRIPTION{ "Generate a Chua attractor." };
    MIN_TAGS{ "chaos" };
    MIN_AUTHOR{ "mzed" };
    MIN_RELATED{ "roessler, lorenz" };

    inlet<>  input   { this, "(bang) calculate next point on attractor" };
    outlet<> outlet_x{ this, "(float) x coordinate" };
    outlet<> outlet_y{ this, "(float) y coordinate" };
    outlet<> outlet_z{ this, "(float) z coordinate" };

    attribute<double> c_a{ this, "a", 14.5 };
    attribute<double> c_b{ this, "b", 1.0 };
    attribute<double> c_c{ this, "c", 25.58 };
    attribute<double> c_d{ this, "d", -1.0 }; // -8/7 -1.14285714286
    attribute<double> c_e{ this, "e", 0.0 }; // -5/7 -0.714285714286
    attribute<double> c_h{ this, "timestep (h)", 0.01 };

    argument<number> a_arg
    { 
        this, "a", "Initial a value.",
        MIN_ARGUMENT_FUNCTION 
        {
            c_a = arg;
        }
    };

    argument<number> b_arg
    { 
        this, "b", "Initial b value.",
        MIN_ARGUMENT_FUNCTION 
        {
            c_b = arg;
        }
    };

    argument<number> c_arg
    { 
        this, "c", "Initial c value.",
        MIN_ARGUMENT_FUNCTION 
        {
            c_c = arg;
        }
    };

    argument<number> d_arg
    {
        this, "d", "Initial d value.",
        MIN_ARGUMENT_FUNCTION
        {
            c_d = arg;
        }
    };

    argument<number> e_arg
    {
        this, "e", "Initial e value.",
        MIN_ARGUMENT_FUNCTION
        {
            c_e = arg;
        }
    };

    argument<number> h_arg
    { this, "h", "Initial h (timestep) value.",
        MIN_ARGUMENT_FUNCTION 
        {
            c_h = arg;
        }
    };

    message<> bang
    {
        this, "bang", "Calculate the next point.",
        MIN_FUNCTION
        {
            // calculate the attractor
            double zNew = currentZ + (c_h * -c_c * currentY);
            double yNew = currentY + (c_h * c_b * (currentX - currentY + currentZ));
            float g = (c_e * currentX) + (c_d + c_e) * (fabs(currentX + 1) - fabs(currentX - 1));
            double xNew = currentX + (c_h * c_a * (currentY - currentX - g));
           
            // save the calculated values
            currentX = xNew;
            currentY = yNew;
            currentZ = zNew;

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
            cout << "mzed.chua object by Michael F. Zbyszynski, v1.0 (c)2022" << endl;
            return {};
        }
    };

private:
    double currentX = 1.0;
    double currentY = 1.0;
    double currentZ = 1.0;
};

MIN_EXTERNAL(mzed_chua);
