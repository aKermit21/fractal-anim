// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "dbg_report.h"
#include "fractal.h"
#include "light.h"
#include <iostream>
#include <memory>

// Angle maths on vectors

// Cosine between two vectors:

//               _   _
//               u * v   (vector dot product)
// cos(theta) = -------
//              |u|*|v|
//          _ _
//  thus if u*v > 0 then theta < 90deg
//          _ _
//   and if u*v < 0 then theta > 90deg
//                     _ _
// calculation formula u*v for 2d vector is =(ux*vx + uy*vy)

LightAngleCase StemFlash::light_vec_angle(int vx, int vy) {
  long int dot_product;

  // take dot product between light vector and fractal (stem) vector
  dot_product = (LightS::s_lightVec.x * static_cast<long int>(vx)) + 
                (LightS::s_lightVec.y * static_cast<long int>(vy));
  
  // Debug
  // Dbg::report_trace("lvec.x = ", LightS::s_lightVec.x);
  // Dbg::report_trace("lvec.y = ", LightS::s_lightVec.y);
  // Dbg::report_trace("vx = ", vx);
  // Dbg::report_trace("vy = ", vy);
  // std::cout << " dot_product " << dot_product << "\n";
  
  if (dot_product > 0) {
    return lAngleBelow90;
  } else {
    return lAngleAbove90;
  }
}


// had angle between light rays and (this) vec was changed (<90 vs >90 deg)
bool StemFlash::light_vec_angle_flip() {
  bool ret = false;
  LightAngleCase curr_l_angle;
  
  curr_l_angle = light_vec_angle(vec_xy.dx, vec_xy.dy);

  if ((prev_l_angle != lAngleUnknown) and
      (curr_l_angle != lAngleUnknown) and
       (curr_l_angle != prev_l_angle)) 
  {
    ret = true;
  }

  prev_l_angle = curr_l_angle;
  return ret;
}

