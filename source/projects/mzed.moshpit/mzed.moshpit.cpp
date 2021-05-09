/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2016-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.e.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;

#define ARRAY_SIZE 1024


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
          redraw();

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
                color { 0.9, 0.9, 0.9, 1.0 }
            };


            for (int i = 0; i < frameskip; ++i) 
            {
                update();
            }
            draw_all(t);

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


 private:

     double col[ARRAY_SIZE];
     long type[ARRAY_SIZE];

     color greyColor = { 0.5, 0.5, 0.5, 0.8 };
     color redColor = { 1.0, 0.0, 0.0, 0.8 };

     //////////////////////////////////////////////////////////////    functions

     void mzed_moshpit::update()
     {
     

     }

     void mzed_moshpit::draw_all(target t)
     {
         int ly = lx; // I think this is cool, because we're only using circles
         double sx = t.width() / lx;
         double sy = t.height() / ly;
         double ss = sqrt(sx * sy);
         double cr = 0.;

         for (int i = 0; i < numMoshers; ++i) 
         {
             cr = fabs(col[i] / 25);
             std::clamp(cr, 0.0, 1.0);
             //if (cr < 0.) cr = 0.0;
             //else if (cr > 1.) cr = 1.0;

             color mosherColor;
         
             if (type[i] == 0) 
             {
                 if (showforce == true) mosherColor = { cr, cr, cr, 0.8 };
                 else mosherColor = greyColor;
             }
             else if (type[i] == 2) // yellow
             {
                 if (showforce == true) mosherColor = { 1.0, 1.0, 0.0, cr };
                 else mosherColor = { 1.0, 1.0, 0., 0.8 };
             }
             else 
             {
                 if (showforce == true) mosherColor = { 1.0, 0.0, 0.0, cr };
                 else mosherColor = redColor;
             }

             //TODO: deal with output
             /*
             if (x->type[i] == 2) {
                 t_atom outList[3];
                 atom_setlong(outList, sx * x->mpX[i]);
                 atom_setlong(outList + 1, sy * x->mpY[i]);
                 atom_setlong(outList + 2, cr * 100);
                 outlet_list(x->m_out, 0L, 3, outList);
             }

             t_atom outList[5];
             atom_setlong(outList, i);
             atom_setlong(outList + 1, x->type[i]);
             atom_setlong(outList + 2, sx * x->mpX[i]);
             atom_setlong(outList + 3, sy * x->mpY[i]);
             atom_setlong(outList + 4, cr * 100);
             outlet_list(x->m_out2, 0L, 5, outList);
             */

             //jgraphics_se,_line_width(g, 1.);
             //jgraphics_arc(g, sx * x->mpX[i], sy * x->mpY[i], ss * x->r[i], 0, 2 * M_PI);

             //arc?

             rect<fill>
             {
                 t,
                 color{ 0.0, 1.0, 0.0, 1.0 },
                 position { 100, 100 },
                 size { 10, 10 },
                 line_width { 1.0 },
             };


             //if (x->m_drawing == 1) {
                // jgraphics_fill(g);
             //}
         }
     }
};

MIN_EXTERNAL(mzed_moshpit);
