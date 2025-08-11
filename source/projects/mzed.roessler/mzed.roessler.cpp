/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2006-2025 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class mzed_roessler : public object<mzed_roessler> 
{
public:
  MIN_DESCRIPTION	{ "Generate a roessler atttractor" };
  MIN_TAGS		    { "chaos" };
  MIN_AUTHOR	    { "mzed" };
  MIN_RELATED		{"lorenz, chua"};
  
  inlet<>  input	{ this, "(bang) calculate next point on attractor" };
  outlet<> outlet_x { this, "(float) x coordinate" };
  outlet<> outlet_y { this, "(float) y coordinate" };
  outlet<> outlet_z { this, "(float) z coordinate" };
  
  attribute<double> r_a { this, "a", 0.02 };
  attribute<double> r_b { this, "b", 0.02 };
  attribute<double> r_c { this, "c", 5.7 };
  attribute<double> r_h { this, "timestep (h)", 0.05 };
  
  argument<number> a_arg { this, "a", "Initial a value.", MIN_ARGUMENT_FUNCTION { r_a = arg; } };
  argument<number> b_arg { this, "b", "Initial b value.", MIN_ARGUMENT_FUNCTION { r_b = arg; } };
  argument<number> c_arg { this, "c", "Initial c value.", MIN_ARGUMENT_FUNCTION { r_c = arg; } };

  argument<number> h_arg { this, "h", "Initial h (timestep) value.", MIN_ARGUMENT_FUNCTION { r_h = arg; } };

  message<> bang
  {
    this, "bang", "Calculate next point",
    MIN_FUNCTION
    {
      // calculate the attractor
      const double xNew { currentX + r_h * (-currentY - currentZ) };
      const double yNew { currentY + r_h * (currentX + r_a * currentY) };
      const double zNew { currentZ + r_h * (r_b + currentZ * (currentX - r_c)) };
      
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
  
  message<> maxclass_setup
  {
    this, "maxclass_setup",
    MIN_FUNCTION
    {
      cout << "mzed.roessler object by Michael F. Zbyszyński, v3.0 ©2002-2025" << endl;
      return {};
    }
  };
  
private:
  double currentX { 0.01 };
  double currentY { 0.01 };
  double currentZ { 0.01 };
};

MIN_EXTERNAL(mzed_roessler);
