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

    attribute<number> numMoshers  //TODO: Threadsafe?
    {
        this, "number of moshers", 300,
        range {1, 8192}
    };

    // respond to the bang message to do something
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
};

MIN_EXTERNAL(mzed_moshpit);
