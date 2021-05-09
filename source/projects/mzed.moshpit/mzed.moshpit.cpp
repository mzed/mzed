/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2016-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.e.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;


class mzed_moshpit : public object<mzed_moshpit>, public ui_operator<200, 200>
{
public:
    MIN_DESCRIPTION	{"A two-dimensional model of moshers, similar to a disordered gas."};
    MIN_TAGS		{"chaos, ui"};
    MIN_AUTHOR		{"mzed"};
    MIN_RELATED		{"boids"};

    inlet<>  input	{ this, "toggle on/off, reset" };
    outlet<> output	{ this, "position of yellow mosher" };

    mzed_moshpit(const atoms& args = {})
        : ui_operator::ui_operator{ this, args } {}


  timer<> clock
  {
      this,
      MIN_FUNCTION
      {
          output.send("bang");
          double interval = double(floor(1000/framerate));
          clock.delay(interval);
          
          return {};
      }
  };


    //////////////////////////////////////////////////////////////    attributes
    
    attribute<bool> on
    { 
        this, "on", false,
        description {"Turn on/off the internal timer."},
        setter 
        { 
            MIN_FUNCTION 
            {
                if (args[0] == true) clock.delay(0.0);    // fire the first one straight-away
                else clock.stop();
                
                return args;
            }
        }
    };
    
    attribute<int> numMoshers  //TODO: Threadsafe?
    {
        this, "number of moshers", 300,
        range {1, 8192}
    };

    attribute<double> noise { this, "number of moshers", 3.0 };
    attribute<double> flock { this, "flock", 1.0 };
    attribute<int> lx { this, "mosher size", 31 };
    attribute<int> frameskip { this, "frames between renders", 2 };
    attribute<int> framerate { this, "frames per second", 30 };
    attribute<bool> showforce { this, "showforce", false };
    attribute<bool> drawing{ this, "draw circles", true };

    //////////////////////////////////////////////////////////////    messages
    
    message<> toggle
    { 
        this, "int", "Turn on/off the internal timer.",
          MIN_FUNCTION {
              on = args[0];
              return {};
          }
    };

    message<> bang 
    { 
        this, "bang", "ahoy",
        MIN_FUNCTION 
        {
            cout << "ahoy" << endl;    // post to the max console
            output.send("ahoy");       // send out our outlet
            return {};
        }
    };

    message<> paint
    {
        this, "paint",
        MIN_FUNCTION
        {
            target t        { args };
            
            rect<fill> {	// background
                t,
                color { 0.5, 0.5, 0, 1.0 }
            };

            return {};
        }
    };

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup 
    { 
        this, "maxclass_setup",
        MIN_FUNCTION 
        {
            cout << "moshpit v0.2 (6 July 2016) by Michael Zbyszyński ©2016" << endl;
            cout << "This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 License" << endl;
            cout << "To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/" << endl;

            return {};
        }
    };


    //////////////////////////////////////////////////////////////    functions
};

MIN_EXTERNAL(mzed_moshpit);
