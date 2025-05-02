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
#include "assert.h"

// Transformation Algorithm Configuration data
// used to tranform parent to child
// and setup primary element and
namespace cTran {
  // Values mulitplications (int fractioning) used in positioning for better accuracy
  inline constexpr int AccurMltp { 32 };
  inline constexpr float AccurMltp_f { static_cast<float>(AccurMltp) };
  
  // Angle degree values mulitplications used in tranformations for better accuracy
  inline constexpr int accurAngleMltp { 32 };
  inline constexpr float accurAngleMltp_f { static_cast<float>(AccurMltp) };

  // Window size centre in accuracy units
  inline constexpr int cXcenterM = (cFrac::WindowXsize * cTran::AccurMltp) / 2;
  inline constexpr int cYcenterM = (cFrac::WindowYsize * cTran::AccurMltp) / 2;
}

struct TranAlg {

  TranAlg(int speed=10) 
    : m_speedScale{ speed }
    , m_indexPre { 0 }
  {
    Dbg::report_info("Init: TranAlg (speed=) ", speed);
    Dbg::report_info("Init: TranAlg (algosize=) ", preCalcAlgoData.size());

    assert(preCalcAlgoData.size() > 0);
    algo_data = conv_to_assym(preCalcAlgoData[0]);
    algo_data_init = algo_data;

    // Initial Speed vs Detailed drawing scale table
    speedScaleVerify(); // input: m_speedScale
  }

  // Transformation full algorithm data used by derived animation class(es)
  // Working copy
  T_Algo_Arr algo_data;
  // Initial Data
  T_Algo_Arr algo_data_init;


  // Small vector - below this size stop recursive search/draw of children
  static int s_SmallVect;
  static int s_SmallVecAnim;

  // rotate from one of the pre-calculated configuration
  void rotate_pre_cfg();
  // refresh mutable (animated) data to pre-configured one
  void reset_pre_cfg();
  // take snaphot of working transformation configuration
  std::string log_trans_config();

  // Init pre-calculated configuration data
  // Transformation data Symmetrical (angle down branch = -angle up branch)
  //  { repos (promile), angle (0.1deg), scale }
  void initPreCalcAlgoData();

  // Calculate proportional cfg/transformation data
  // keeping current angles but varying branch scale
  // (this can be used for size animation)
  T_Algo_Arr get_prop_cfg_arr(float scale_max, float scale_min);

  // setting smallest vector size to be drawn
  // controls speed vs accurracy drawing
  void speedIncrement(void);
  void speedDecrement(void);

  int get_speedScale() const;

private:

  int m_speedScale;

  // Speed vs Detailed drawing scale
  // speed to small vector size calculation
  void speedScaleVerify(void);

  // Speed scale data - small vector sizes threshold (to be drawn)
  constexpr static int SpeedScaleDataSize = 21;
  constexpr static std::array<int, SpeedScaleDataSize> SpeedScalaData = {
    cTran::AccurMltp,  // 0. - Single actual point
    static_cast<int>(1.1 * cTran::AccurMltp), 
    static_cast<int>(1.2 * cTran::AccurMltp),
    static_cast<int>(1.4 * cTran::AccurMltp),
    static_cast<int>(1.6 * cTran::AccurMltp),
    static_cast<int>(1.8 * cTran::AccurMltp), // 5
    static_cast<int>(2 * cTran::AccurMltp),
    static_cast<int>(2.2 * cTran::AccurMltp),
    static_cast<int>(2.5 * cTran::AccurMltp),
    static_cast<int>(2.7 * cTran::AccurMltp),
    static_cast<int>(3 * cTran::AccurMltp), // 10. Default - 3 visible points
    static_cast<int>(3.3 * cTran::AccurMltp),
    static_cast<int>(4.1 * cTran::AccurMltp),
    static_cast<int>(4.5 * cTran::AccurMltp),
    static_cast<int>(5 * cTran::AccurMltp),
    static_cast<int>(5.6 * cTran::AccurMltp),
    static_cast<int>(6.4 * cTran::AccurMltp),
    static_cast<int>(7 * cTran::AccurMltp),
    static_cast<int>(8 * cTran::AccurMltp),
    static_cast<int>(9 * cTran::AccurMltp),
    static_cast<int>(10 * cTran::AccurMltp), // 20.  - 10 points
  };

  // Convert Symmetrical algo to working copy of (potentialy) Asymmetrical
  // and with diffrent granularity
  T_Algo_Arr conv_to_assym(T_Algo_Arr_Symm symm_algo);
  
  int m_indexPre;
  
  // Collection of Proportional cgf data pre-calculated (to be rotate by 'P')
  // Transformation data Symmetrical (angle down branch = -angle up branch)
  //  { repos (promile), angle (0.1deg), scale }
  constexpr static std::array<T_Algo_Arr_Symm, 4U> preCalcAlgoData {{
  
    {{{222, 750, 0.38f}, // default: ANGLE_MAX = 75 SCALE_MAX 0.38f
    {445, 687, 0.3225f},
    {634, 625, 0.265f},
    {790, 562, 0.2075f},
    {912, 500, 0.15f}}}, // default: ANGLE_MIN = 50 SCALE_MIN 0.15f    
  
    {{{234, 660, 0.422851f},
    {468, 605, 0.349669f},
    {662, 550, 0.276488f},
    {815, 495, 0.203306f},
    {927, 440, 0.130125f}}}, // more dense but slower animation
    
    {{{228, 495, 0.394231f},
    {456, 453, 0.330541f},
    {647, 412, 0.26685f},
    {801, 371, 0.20316f},
    {919, 330, 0.139469f}}}, // slightly more dense - more closed

    {{{238, 660, 0.468851f},
    {476, 605, 0.38417f},
    {672, 550, 0.299488f},
    {824, 495, 0.214806f},
    {933, 440, 0.130125f}}},
  // The Array can be extended or elements replaced taking data from snapshot log
  }};
  
};
