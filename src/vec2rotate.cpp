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
#include "transform.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <utility>
#include <assert.h>

constexpr static int cPromile { 1000 };

// rotation matrix calculations
template<typename T>
void Vec2D::rotation_matrix(const T sin_val, const T cos_val) {
  int dx_new = static_cast<int>(dx * cos_val - dy * sin_val);
  dy = static_cast<int>(dx * sin_val + dy * cos_val);
  dx = dx_new;
}

// move along x,y and x+dx,y+dy line - shall be done before rotation
// proportionaly to length size
void Vec2D::reposition(const int promile) {
  x = x + (dx * promile) / cPromile;
  y = y + (dy * promile) / cPromile;
}


// move along x,y and x+dx,y+dy line - shall be done before rotation
void Stem::reposition_stem(const int promile, Stem::ThickLevel level) {
  int thickness = 0;

  if (level == Stem::thick2) { thickness = 6; } 
  else if (level == Stem::thick1) { thickness = 8; }
  else { thickness = 0; }

  // creating width (optionally)
  if (level != Stem::thickNone) {
    x1 = vec_xy.x + (vec_xy.dx * (promile - thickness)) / cPromile;
    y1 = vec_xy.y + (vec_xy.dy * (promile - thickness)) / cPromile;
    x2 = vec_xy.x + (vec_xy.dx * (promile + thickness)) / cPromile;
    y2 = vec_xy.y + (vec_xy.dy * (promile + thickness)) / cPromile;
  }

  // Central Line transformation
  vec_xy.x = vec_xy.x + (vec_xy.dx * promile) / cPromile;
  vec_xy.y = vec_xy.y + (vec_xy.dy * promile) / cPromile;
}


// Calculate coordinates of stem taking given width
void Stem::recalculateStemWidthCoordinates(float cumulativeFactor) {
  // Use multiplied values for better accuracy tranformation
  float stem_x, stem_y, length;
  // create perpendicular vector of given width
  stem_x = -vec_xy.dy;
  stem_y = vec_xy.dx;
  length = std::sqrt(stem_x*stem_x + stem_y*stem_y);
  auto adjustedStemWidth = cumulativeFactor * cFrac::PrimStemWidth;
  y1 = static_cast<int>
    (vec_xy.y - stem_y * ((adjustedStemWidth/length) * cTran::AccurMltp ));
  y2 = static_cast<int>
    (vec_xy.y + stem_y * ((adjustedStemWidth/length) * cTran::AccurMltp ));
  x1 = static_cast<int>
    (vec_xy.x - stem_x * ((adjustedStemWidth/length) * cTran::AccurMltp ));
  x2 = static_cast<int>
    (vec_xy.x + stem_x * ((adjustedStemWidth/length) * cTran::AccurMltp ));
}


// Shrink stem according to given (usable) window Center
// used for auto-scaling
void Stem::shrinkStemCenter(float factor, float cumulativeFactor, 
                            int xCenter, int yCenter) {

  assert(factor <= 1 and factor > 0 and "factor for shrinking expected to be 0..1");
  assert(xCenter > 0 and yCenter > 0 and "expected plus coordinates");
  
  // Change position to obtain shrinking in relation to (window) central point
  int x1_center = vec_xy.x - xCenter;
  x1_center = static_cast<int>(x1_center * factor);
  vec_xy.x = x1_center + xCenter;
  
  int y1_center = vec_xy.y - yCenter;
  y1_center = static_cast<int>(y1_center * factor);
  vec_xy.y = y1_center + yCenter;
  
  vec_xy.dx *= factor; 
  vec_xy.dy *= factor;

  // Calculate coordinates of stem taking given width
  recalculateStemWidthCoordinates(cumulativeFactor);
}

// move by given (absolute) dx dy
void Stem::repositionStemAbsolute(const int dx, const int dy) {
  // Move Stem axis (vector)
  vec_xy.x += dx;
  vec_xy.y += dy;
  // Move possible stem ply drawing
  x1 += dx;
  x2 += dx;
  y1 += dy;
  y2 += dy;
}

void Vec2D::rotate(const int angle, const float scale = 1.0) {
  // sin, cos lookup table
  struct Pre_sin_cos {
    int theta; // accurAngleMltp parts of degrees
    float sinus;
    float cosin;
  };

  // sin, cos Lookup Table
  // Table possible size: 1 (practically no use) .. 10 (optimal) .. 100 (less than optimal)
  // Usually NrOfElements*2 angles are used per signle frame
  constexpr static int PRECALC_MAX { 10 }; 
  static std::array<Pre_sin_cos, PRECALC_MAX> angle_tab {}; // intialize all to zero

  if (angle != 0) {

    // First check value in Look-Up tanble;
    // try to find already stored angle (memoization algo)
    auto result = std::find_if(
        begin(angle_tab), end(angle_tab),
        [&angle](const Pre_sin_cos &tab) { return angle == tab.theta; });
    if (result != end(angle_tab)) {
      // reuse precalculated sinus and cosinus
      // dbg.report_trace( "precalc (x1000) sinus: ", 1000*result->sinus );
      rotation_matrix(result->sinus, result->cosin);
    } else {
      Dbg::report_trace("New (unused) angle: ", angle);
      // Utilize row in table, according to index, to store calculated new values

      // (simple) Rotate to the right table elements in order to make room
      // for a new value which will be placed at the very beginning (0-index);
      // this is to shorten search for recently used angle
      std::rotate(angle_tab.rbegin(), angle_tab.rbegin() + 1, angle_tab.rend());

      // Primary sin/cos math calculations needed
      // Converting 0.1 degress (or other granularity) to radians
      float angle_rad = angle * 3.14159 / (180 * cTran::accurAngleMltp);

      // Place calculated value at the very beginning (0-index)
      angle_tab[0].theta = angle;
      angle_tab[0].sinus = sin(angle_rad);
      angle_tab[0].cosin = cos(angle_rad);
      Dbg::report_trace("New calculation for angle: ", angle);
      // dbg::report_trace("  (x1000) sin:", 1000*result->sinus); // x1000 =
      // float to int scale dbg::report_trace("  (x1000) cos:",
      // 1000*result->cosin);
      rotation_matrix(angle_tab[0].sinus, angle_tab[0].cosin);
    }
  }

  if (scale != 1.0) {
    dx = dx * scale;
    dy = dy * scale;
  }
}
