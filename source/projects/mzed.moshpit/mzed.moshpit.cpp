/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2016-2023 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the GPL v3 License found in the License.md file.e.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;

constexpr size_t ARRAY_SIZE { 1024 };
constexpr int NMAX { 50 };
constexpr double RADIUS { 1.0 };
constexpr size_t TWO_R { 2 };
constexpr size_t FR { 4 };
constexpr double VHAPPY { 1.0 };
constexpr double DAMP { 1.0 };
constexpr double GDT { 0.1 };

class mzed_moshpit : public object<mzed_moshpit>, public ui_operator<200, 200>
{
public:
  MIN_DESCRIPTION{ "A two-dimensional model of moshers, similar to a disordered gas." };
  MIN_TAGS        { "chaos, ui" };
  MIN_AUTHOR      { "mzed" };
  MIN_RELATED     { "boids" };
  
  inlet<>  input { this, "toggle on/off, reset" };
  outlet<> out1 { this, "position of yellow mosher" };
  outlet<> out2 { this, "all of the moshers, by type" };
  
  mzed_moshpit(const atoms& args = {}) : ui_operator::ui_operator{ this, args }
  {
    // calculate sidelength
    lx = 1.03 * sqrt(M_PI * RADIUS * RADIUS * numMoshers);
    ly = lx;
    
    //neighborlist
    m_size[0] = lx / FR;
    m_size[1] = ly / FR;
    
    cells.resize((m_size[0] * m_size[1] * NMAX * 10), 0); //TODO: big should this be?
    count.fill(0);
    
    // init_circle(x);
    bool uniq { true } ;
    for (size_t mosher {}; mosher < numMoshers; ++mosher)
    {
      double tx { lx * normRand() };
      double ty { ly * normRand() };
      
      type[mosher] = 0;
      r[mosher] = RADIUS;
      mpX[mosher] = tx;
      mpY[mosher] = ty;
      double dd { sqrt((tx - lx / 2) * (tx - lx / 2) + (ty - ly / 2) * (ty - ly / 2)) };
      double rad { sqrt(fractionRed * lx * ly / M_PI) };
      bool doCircle { true };
      
      if (doCircle) // Expose this as an attribute?
      {
        if (dd < rad)
        {
          type[mosher] = (uniq) ? 2 : 1;
          uniq = false;
        }
      }
      else
      {
        if (normRand() < fractionRed) type[mosher] = 1;
      }
      
      vx[mosher] = VHAPPY * (normRand() - 0.5);
      vy[mosher] = VHAPPY * (normRand() - 0.5);
    }
  }
  
  timer<> clock
  {
    this,
    MIN_FUNCTION
    {
      redraw();
      double interval { double(floor(1000/framerate)) };
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
  attribute<double> fractionRed { this, "fraction red", 0.15 };
  attribute<int> frameskip { this, "frames between renders", 2 };
  attribute<int> framerate { this, "frames per second", 30 };
  attribute<bool> showforce { this, "showforce", false };
  attribute<bool> drawing { this, "draw circles", true };
  
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
      out1.send("ahoy");       // send out our outlet
      return {};
    }
  };
  
  message<> paint
  {
    this, "paint",
    MIN_FUNCTION
    {
      target t        { args };
      
      rect<fill> background
      {
        t,
        color { 0.9, 0.9, 0.9, 0.0 },
        line_width{ 1.0 }
      };
      
      rect<> frame
      {
        t,
        color{ 0.3, 0.3, 0.3, 1.0 },
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
  
  double r[ARRAY_SIZE] {};
  double mpX[ARRAY_SIZE] {};
  double mpY[ARRAY_SIZE] {};
  size_t type[ARRAY_SIZE] {};
  double vx[ARRAY_SIZE] {};
  double vy[ARRAY_SIZE] {};
  double fx[ARRAY_SIZE] {};
  double fy[ARRAY_SIZE] {};
  double col[ARRAY_SIZE] {};
  
  //neighbor list
  int lx {};
  int ly {};
  int m_size[2] { 0, 0 };
  std::vector<int> cells {};
  std::array<int, ARRAY_SIZE> count {};
  
  //things we can change
  int pbc[2] { 1, 1 };
  int epsilon { 100 };
  
  const color greyColor { 0.5, 0.5, 0.5, 0.8 };
  const color redColor { 1.0, 0.3, 0.0, 0.8 }; //really orange
  const color yellowColor { 1.0, 1.0, 0., 0.8 };
  
  //////////////////////////////////////////////////////////////    functions
  
  int moshpit_mod_rvec(const int a, const int b, const int p, int* image)
  {
    *image = 1;
    
    if (b == 0 && a == 0)
    {
      *image = 0;
      return 0;
    }
    
    if (p != 0)
    {
      if (a > b) return a - b - 1;
      if (a < 0) return a + b + 1;
    }
    else
    {
      if (a > b) return b;
      if (a < 0) return 0;
    }
    
    *image = 0;
    return a;
  }
  
  void nbl_bin()
  {
    count.fill(0);
    
    for (int mosher {}; mosher < numMoshers; ++mosher)
    {
      const size_t indX { static_cast<size_t>(mpX[mosher] / lx * m_size[0]) };
      const size_t indY { static_cast<size_t>(mpY[mosher] / ly * m_size[1]) };
      const size_t tt { indX + indY * m_size[0] };
      cells[NMAX * tt + count[tt]] = mosher;
      ++count[tt];
    }
  }
  
  inline double normRand()
  {
    return (double)rand() / (double)RAND_MAX;
  }
  
  inline double mymod(const double a, const double b)
  { //TODO: MZ why?
    return a - b * floor(a / b) + b * (a < 0);
  }
  
  void update()
  {
    int image[2] { 0, 0 };
    
    for (size_t mosher {}; mosher < numMoshers; ++mosher)
    {
      col[mosher] = 0.0;
      fx[mosher] = 0.0;
      fy[mosher] = 0.0;
      
      double wx {};
      double wy {};
      long neigh {};
      
      int indX { static_cast<int>(mpX[mosher] / lx * m_size[0]) };
      int indY { static_cast<int>(mpY[mosher] / ly * m_size[1]) };
      
      for (int ttx { -1 }; ttx <= 1; ++ttx)
      {
        for (int tty { -1 }; tty <= 1; ++tty)
        {
          bool goodcell { true };
          const int tixx { moshpit_mod_rvec(indX + ttx, m_size[0] - 1, pbc[0], &image[0]) };
          const int tixy { moshpit_mod_rvec(indY + tty, m_size[1] - 1, pbc[1], &image[1]) };
          
          if ((pbc[0] < image[0]) || (pbc[1] < image[1])) goodcell = 0;
          
          if (goodcell)
          {
            long cell { tixx + (tixy * m_size[0]) };
            
            for (int cc {}; cc < count[cell]; ++cc)
            {
              long j { cells[NMAX * cell + cc] };
              double dx { mpX[j] - mpX[mosher] };
              if (image[0]) dx += lx * ttx;
              
              double dy { mpY[j] - mpY[mosher] };
              if (image[1]) dy += ly * tty;
              
              double l { sqrt(dx * dx + dy * dy) };
              if (l > 1e-6 && l < TWO_R)
              {
                double r0 { r[mosher] + r[j] };
                double f { (1 - l / r0) };
                double c0 { -(epsilon) * f * f * (l < r0) };
                
                fx[mosher] += c0 * dx;
                fy[mosher] += c0 * dy;
                col[mosher] +=  c0 * c0 * dx * dx + c0 * c0 * dy * dy; //fx[i]*fx[i] + fy[i]*fy[i]
              }
              
              if (type[mosher] > 0 && type[j] > 0 && l > 1e-6 && l < FR)
              {
                wx += vx[j];
                wy += vy[j];
                neigh++;
              }
            }
          }
        }
      }
      
      const double wlen = (wx * wx + wy * wy);
      
      if (type[mosher] > 0 && neigh > 0 && wlen > 1e-6)
      {
        fx[mosher] += flock * wx / wlen;
        fy[mosher] += flock * wy / wlen;
      }
      
      const double vlen = vx[mosher] * vx[mosher] + vy[mosher] * vy[mosher];
      const double vhap = type[mosher] > 0 ? VHAPPY: 0.;

      if (vlen > 1e-6)
      {
        fx[mosher] += DAMP * (vhap - vlen) * vx[mosher] / vlen;
        fy[mosher] += DAMP * (vhap - vlen) * vy[mosher] / vlen;
      }
      
      if (type[mosher] > 0)
      {
        fx[mosher] += noise * (normRand() - 0.5);
        fy[mosher] += noise * (normRand() - 0.5);
      }
    }
    
    for (size_t mosher {}; mosher < numMoshers; ++mosher)
    {
      vx[mosher] += fx[mosher] * GDT;
      vy[mosher] += fy[mosher] * GDT;
      mpX[mosher] += vx[mosher] * GDT;
      mpY[mosher] += vy[mosher] * GDT;
      
      if (pbc[0] == 0)
      {
        if (mpX[mosher] >= lx)
        {
          mpX[mosher] = 2 * lx - mpX[mosher];
          vx[mosher] *= -1;
        }
        if (mpX[mosher] < 0)
        {
          mpX[mosher] = -(mpX[mosher]);
          vx[mosher] *= -1;
        }
      }
      else
      {
        if (mpX[mosher] >= lx || mpX[mosher] < 0) mpX[mosher] = mymod(mpX[mosher], lx);
      }
      
      if (pbc[1] == 0)
      {
        if (mpY[mosher] >= ly)
        {
          mpY[mosher] = 2 * ly - mpY[mosher];
          vy[mosher] *= -1;
        }
        if (mpY[mosher] < 0)
        {
          mpY[mosher] = -(mpY[mosher]);
          vy[mosher] *= -1;
        }
      }
      else
      {
        if (mpY[mosher] >= ly || mpY[mosher] < 0) mpY[mosher] = mymod(mpY[mosher], ly);
      }
      /*
       TODO: Do I need this?
       if (dovorticity == true)
       {
        graph_vel(sqrt(x->vx[i]*x->vx[i] + x->vy[i]* x->vy[i]));
       }*/
    }
  }
  
  void draw_all(target t)
  {
    const double sx { t.width() / lx };
    const double sy { t.height() / ly };
    const double ss { sqrt(sx * sy) * 2.0 };
    
    for (size_t mosher {}; mosher < numMoshers; ++mosher)
    {
      const double cr { std::clamp(fabs(col[mosher] / 25), 0.0, 1.0) };
      
      if (drawing)
      {
        color mosherColor;
        
        if (type[mosher] == 0)
        {
          if (showforce == true) mosherColor = { cr, cr, cr, 0.8 };
          else mosherColor = greyColor;
        }
        else if (type[mosher] == 2) // yellow
        {
          if (showforce == true) mosherColor = { 1.0, 1.0, 0.0, cr };
          else mosherColor = yellowColor;
        }
        else
        {
          if (showforce == true) mosherColor = { 1.0, 0.0, 0.0, cr };
          else mosherColor = redColor;
        }
        
        const double shim { ss * r[mosher] * 0.5 };
        
        ellipse<fill> mosherEllipse
        {
          t,
          color{ mosherColor },
          position{ sx * mpX[mosher] - shim, sy * mpY[mosher] - shim },
          size{ ss * r[mosher], ss * r[mosher] }
        };
      }
      
      out2.send(int(mosher), int(type[mosher]), sx * mpX[mosher], sy * mpY[mosher], cr * 100);
      out1.send(sx * mpX[mosher], sy * mpY[mosher], cr * 100);
    }
  }
};

MIN_EXTERNAL(mzed_moshpit);
