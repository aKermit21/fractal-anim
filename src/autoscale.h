// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "dbg_report.h"
#include "fractal.h"
#include "transform.h"
#include <string>
#include <iostream>

// Min/Max drawing structure defined in Dbg
// struct VecMinMax {
//   int min_x;
//   int min_y;
//   int max_x;
//   int max_y;
// };

// Perform (optionally) autoscale by changing size and location
// of first element to fit whole drawing into window
struct AutoScale
{
  using VecMinMax = Dbg::VecMinMax;
  struct VecDelta {int dx; int dy;};

  AutoScale(bool onOff = true)
  : m_minmax{} // actual start values are set by cycleStart
  , m_optionOn{onOff}
  , m_rescaleActive{false}
  , m_cumulativeFactor{1.0}
  {
    cycleStart();
    Dbg::report_info("Init: AutoScale (Xsize)  ", m_minmax.minX);
  }

  
  // Window margins considered in start or stop Rescale algo
  constexpr static int cMargin { 5 };
  // more room at the top expected because of light size
  constexpr static int cTopMargin { 70 }; 
  // margin histeresis
  constexpr static int cHistMargin { 20 };
  // NO mevement, centering, needed
  constexpr static VecDelta cDeltaNoMove { 0, 0 };

  // Usable window center used throughout autoscale
  constexpr static int winUsable_x_center = cTran::cXcenterM;
  constexpr static int winUsable_y_center = cTran::cYcenterM + 
                           ((cTran::AccurMltp * (cTopMargin - cMargin)) /2);
  
  // Single step move (centering)
  constexpr static int cSmallStep { 2 * cTran::AccurMltp}; // 2 (graphic) points
  constexpr static int cAcceptedDiff { 3 * cTran::AccurMltp}; // 3 (graphic) points

  // initialize data for new frame/cycle
  void cycleStart();

    // collect vector sizes to find drawing Min/Max per frame/cycle
  void findMinMax(const Vec2D & vec);
  
  // get Min,Max per frame in real size
  // rescale if needed
  void cycleResume(Element & prim);

  bool ifRescaleActive() const {
    return m_rescaleActive;
  };

  float getShrinkCumulativeFactor() const {
    return m_cumulativeFactor;
  }

  void resetAutoScale() {
    m_rescaleActive = false;
    m_cumulativeFactor = 1.0f;
  }
  
  private:

  // finding edges (min/max) of all elements per last frame 
  // (in multiplied vector size)
  VecMinMax m_minmax;

  // Rescale (option) is switched On
  bool m_optionOn;
  
  // Rescale is Currently active
  bool m_rescaleActive;

  float m_cumulativeFactor { 1 };
  
  VecMinMax getRealScale(void);
  
  VecDelta centerPicture(void);
  
  bool rescaleRequired(const VecMinMax vec) const;
  bool rescaleFinished(const VecMinMax vec) const;
};

