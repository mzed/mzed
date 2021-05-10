/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2016-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.e.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;

constexpr size_t ARRAY_SIZE = 1024;
constexpr size_t NMAX = 50;
constexpr double RADIUS = 1.0;
constexpr size_t TWO_R = 2;
constexpr size_t FR = 2;
constexpr double VHAPPY = 1.0;
constexpr double DAMP = 1.0;
constexpr double GDT = 0.1;
constexpr double FRAC = 0.15;

class mzed_moshpit : public object<mzed_moshpit>, public ui_operator<200, 200>
{
public:
    MIN_DESCRIPTION{ "A two-dimensional model of moshers, similar to a disordered gas." };
    MIN_TAGS{ "chaos, ui" };
    MIN_AUTHOR{ "mzed" };
    MIN_RELATED{ "boids" };

    inlet<>  input{ this, "toggle on/off, reset" };
    outlet<> output{ this, "position of yellow mosher" };

    mzed_moshpit(const atoms& args = {})
        : ui_operator::ui_operator{ this, args }
    {
        // initialize pit
        for (size_t i = 0; i < numMoshers; ++i)
        {
            r[i] = 0.;
            mpX[i] = 0.;
            mpY[i] = 0.;
            type[i] = 0;
            vx[i] = 0.;
            vy[i] = 0.;
            fx[i] = 0.;
            fy[i] = 0.;
            col[i] = 0.;
        }

        // calculate sidelength
        lx = floor(1.03 * sqrt(M_PI * RADIUS * RADIUS * numMoshers));
        ly = lx;

        //neighborlist
        m_size[0] = floor(lx / FR);
        m_size[1] = floor(ly / FR);


        for (size_t i = 0; i < (m_size[0] * m_size[1] * NMAX * 10); ++i)
        {
            cells.push_back(0);
        }

        for (size_t i = 0; i < (m_size[0] * m_size[1]); ++i)
        {
            count[i] = 0;
        } 

        // init_circle(x);
        bool uniq = true;
        for (size_t i = 0; i < numMoshers; ++i)
        {
            double tx = lx * normRand();
            double ty = ly * normRand();

            type[i] = 0;
            r[i] = RADIUS;
            mpX[i] = tx;
            mpY[i] = ty;
            double dd = sqrt((tx - lx / 2) * (tx - lx / 2) + (ty - ly / 2) * (ty - ly / 2));
            double rad = sqrt(FRAC * lx * ly / M_PI);
            bool doCircle = true;

            if (doCircle) 
            {
                if (dd < rad) 
                {
                    type[i] = (uniq) ? 2 : 1;
                    uniq = false;
                }
            }
            else 
            {
                if (normRand() < FRAC) type[i] = 1;
            }

            vx[i] = VHAPPY * (normRand() - 0.5);
            vy[i] = VHAPPY * (normRand() - 0.5);
        }
    }

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

    attribute<double> noise { this, "noise", 3.0 };
    attribute<double> flock { this, "flock", 1.0 };
    attribute<int> frameskip { this, "frames between renders", 2 };
    attribute<int> framerate { this, "frames per second", 30 };
    attribute<bool> showforce { this, "showforce", false };
    attribute<bool> drawing{ this, "draw circles", true };

    //////////////////////////////////////////////////////////////    messages
    
    message<> toggle
    { 
        this, "int", "Turn on/off the internal timer.",
        MIN_FUNCTION 
        {
            on = args[0];
            return {};
        }
    };

    message<> bang 
    { 
        this, "bang", "ahoy",
        MIN_FUNCTION 
        {
            cout << "ahoy " << m_size[0] << endl;    // post to the max console
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
            
            // background
            rect<fill> 
            {	
                t,
                color { 0.9, 0.9, 0.9, 0.0 },
                line_width{ 1.0 }
            };

            // frame
            rect<> 
            {		
                t,
                color{ {0.3, 0.3, 0.3, 1.0} },
                line_width{ 1.0 }
            };

            nbl_bin();
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

     double mpX[ARRAY_SIZE];
     double mpY[ARRAY_SIZE];
     double vx[ARRAY_SIZE];
     double vy[ARRAY_SIZE];
     double fx[ARRAY_SIZE];
     double fy[ARRAY_SIZE];
     double col[ARRAY_SIZE];
     double r[ARRAY_SIZE];
     long type[ARRAY_SIZE];

     //neighbor list
     long lx;
     long ly;
     long m_size[2] = { 0, 0 };
     std::vector<long> cells;
     long count[ARRAY_SIZE];

     //things we can change
     long pbc[2] = { 1, 1 };
     long epsilon = 100;

     color greyColor = { 0.5, 0.5, 0.5, 0.8 };
     color redColor = { 1.0, 0.0, 0.0, 0.8 };

     //////////////////////////////////////////////////////////////    functions

     long moshpit_mod_rvec(int a, int b, int p, int* image) {
         *image = 1;
         if (b == 0) {
             if (a == 0) {
                 *image = 0;
             }
             return 0;
         }
         if (p != 0) {
             if (a > b) {
                 return a - b - 1;
             }
             if (a < 0) {
                 return a + b + 1;
             }
         }
         else {
             if (a > b) {
                 return b;
             }
             if (a < 0) {
                 return 0;
             }
         }
         *image = 0;
         return a;
     }

     void nbl_bin() 
     {
         for (size_t i = 0; i < (m_size[0] * m_size[1]); ++i) 
         {
             count[i] = 0;
         }

         for (size_t i = 0; i < numMoshers; ++i)
         {

             size_t indX = floor(mpX[i] / lx * m_size[0]);
             size_t indY = floor(mpY[i] / ly * m_size[1]);
             size_t tt = indX + indY * m_size[0];


                if ((NMAX * tt + count[tt]) > cells.size())
                {
                    cout << "oh noes!" << endl;
                    cout << "tt " << tt << endl;
                    cout << "thing " << NMAX * tt + count[tt] << endl;
                }

             cells[NMAX * tt + count[tt]] = i;
             count[tt]++;
         }
     }

     inline double normRand() 
     {
         return (double)rand() / (double)RAND_MAX;
     }

     inline double mymod(double a, double b) { //TODO: MZ why?
         return a - b * floor(a / b) + b * (a < 0);
     }
     
     void update()
     {
         // double colavg = 0.0;  //Doesn't seem like I need this
         int image[2] = { 0, 0 };
         
         for (size_t i = 0; i < numMoshers; ++i) 
         {
             col[i] = 0.0;
             fx[i] = 0.0;
             fy[i] = 0.0;
             double wx = 0.0;
             double wy = 0.0;
             long neigh = 0;

             long indX = floor(mpX[i] / lx * m_size[0]);
             long indY = floor(mpY[i] / ly * m_size[1]);

             for (int ttx = -1; ttx <= 1; ++ttx) 
             {
                 for (int tty = -1; tty <= 1; ++tty) 
                 {
                     bool goodcell = 1;
                     long tixx = moshpit_mod_rvec(indX + ttx, m_size[0] - 1, pbc[0], &image[0]);
                     long tixy = moshpit_mod_rvec(indY + tty, m_size[1] - 1, pbc[1], &image[1]);
                     if ((pbc[0] < image[0]) || (pbc[1] < image[1])) 
                     {
                         goodcell = 0;
                     }

                     if (goodcell) 
                     {
                         long cell = tixx + (tixy * m_size[0]);

                         for (int cc = 0; cc < count[cell]; ++cc) 
                         {
                             long j = cells[NMAX * cell + cc];
                             double dx = mpX[j] - mpX[i];
                             if (image[0]) {
                                 dx += lx * ttx;
                             }
                             double dy = mpY[j] - mpY[i];
                             if (image[1]) {
                                 dy += ly * tty;
                             }
                             double l = sqrt(dx * dx + dy * dy);
                             if (l > 1e-6 && l < TWO_R) 
                             {
                                 double r0 = r[i] + r[j];
                                 double f = (1 - l / r0);
                                 double c0 = -(epsilon) * f * f * (l < r0);
                                 fx[i] += c0 * dx;
                                 fy[i] += c0 * dy;
                                 double tcol = c0 * c0 * dx * dx + c0 * c0 * dy * dy; //fx[i]*fx[i] + fy[i]*fy[i]
                                 col[i] += tcol;
                             }

                             if (type[i] > 0 && type[j] > 0 && l > 1e-6 && l < FR) 
                             {
                                 wx += vx[j];
                                 wy += vy[j];
                                 neigh++;
                             }
                         }
                     }
                 }
             }

             double wlen = (wx * wx + wy * wy);

             if (type[i] > 0 && neigh > 0 && wlen > 1e-6) 
             {
                 fx[i] += flock * wx / wlen;
                 fy[i] += flock * wy / wlen;
             }

             double vlen = vx[i] * vx[i] + vy[i] * vy[i];
             double vhap = 0.;

             if (type[i] > 0) 
             {
                 vhap = VHAPPY;
             }
             else 
             {
                 vhap = 0.;
             }

             if (vlen > 1e-6) 
             {
                 fx[i] += DAMP * (vhap - vlen) * vx[i] / vlen;
                 fy[i] += DAMP * (vhap - vlen) * vy[i] / vlen;
             }

             if (type[i] > 0) 
             {
                 fx[i] += noise * (normRand() - 0.5);
                 fy[i] += noise * (normRand() - 0.5);
             }
             //Some keys stuff here, which I ignored
             //colavg += x->col[i];
         }
         for (size_t i = 0; i < numMoshers; ++i) 
         {
             vx[i] += fx[i] * GDT;
             vy[i] += fy[i] * GDT;
             mpX[i] += vx[i] * GDT;
             mpY[i] += vy[i] * GDT;

             if (pbc[0] == 0) 
             {
                 if (mpX[i] >= lx) 
                 {
                     mpX[i] = 2 * lx - mpX[i];
                     vx[i] *= -1;
                 }
                 if (mpX[i] < 0) 
                 {
                     mpX[i] = -(mpX[i]);
                     vx[i] *= -1;
                 }
             }
             else 
             {
                 if (mpX[i] >= lx || mpX[i] < 0) 
                 {
                     mpX[i] = mymod(mpX[i], lx);
                 }
             }

             if (pbc[1] == 0) {
                 if (mpY[i] >= ly) {
                     mpY[i] = 2 * ly - mpY[i];
                     vy[i] *= -1;
                 }
                 if (mpY[i] < 0) {
                     mpY[i] = -(mpY[i]);
                     vy[i] *= -1;
                 }

             }
             else {
                 if (mpY[i] >= ly || mpY[i] < 0) {
                     mpY[i] = mymod(mpY[i], ly);
                 }
             }
             /* TODO: Do I need this?
                 if (dovorticity == true) {
                     graph_vel(sqrt(x->vx[i]*x->vx[i] + x->vy[i]* x->vy[i]));
                 }*/
         }

         //colavg /= numMoshers;
     }

     void draw_all(target t)
     {
         double sx = t.width() / lx;
         double sy = t.height() / ly;
         double ss = sqrt(sx * sy);
         double cr = 0.;

         for (int i = 0; i < numMoshers; ++i) 
         {
             cr = fabs(col[i] / 25);
             cr = std::clamp(cr, 0.0, 1.0);
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
             //jgraphics_arc(g, sx * x->mpX[i], sx * x->mpX[i], ss * x->r[i], 0, 2 * M_PI);

             ellipse<fill> {
                 t,
                 color{ mosherColor },
                 position{ sx * mpX[i], sy * mpY[i] },
                 size{ ss * r[i], ss * r[i] }
             };

             //if (x->m_drawing == 1) {
                // jgraphics_fill(g);
             //}
         }
     }
};

MIN_EXTERNAL(mzed_moshpit);
