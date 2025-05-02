// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "fractal.h"
#include "transform.h"
#include "dbg_report.h"

// Animation by step by step Angle or size 
// modification of mutable algo data

struct MovAnim : TranAlg {

  // Angle Animation state
  enum AngleAnimState {angleStop, angleClosing, angleOpening};

  // Resizing Animation state
  // assumed: Larger = first brach; Smaller = last branch
  enum ResizeAnimState {resizeStop, resizeMaxLarger, resizeMaxSmaller,
                                resizeMinLarger, resizeMinSmaller};

  enum StopType { freezeTimeStop, justStop, endOfStopList};

  // Steps per Closing/Opening - angle animation
  constexpr static int NR_OF_STEPS_ANIM { 150 };
   
  // Single Step for resizing (Enlarging/Diminishing)
  constexpr static float ENLARGE_SIZE_STEP { 0.002f };  // absolute
  constexpr static float DIMINISH_SIZE_STEP { 0.998f };   // relative in percent
  
  constexpr static float ANIM_SCALE_MAX { 0.6f };  // shall be greater than SCALE_MAX
  constexpr static float ANIM_SCALE_MIN { 0.01f }; // shall bel less than SCALE_MIN
  
  //default configuration array
  // constexpr static T_Algo_Arr algo_data_default = TranAlg::fixed_data_default;

  MovAnim(int speed=10) 
    : TranAlg{ speed }
    , stopAtZero { false }
    , angle_anim_state { angleStop }
    , resize_anim_state { resizeStop }
    , stopType {justStop }
  {
    Dbg::report_info("Init: MovAnim  (speed=)", speed);
  }
  
  // called once in a display loop
  // to realize possible animation
  void one_step_cfg_change();

  void stopAnimation();

  // Stop or Freeze animation - intermittently
  void stopFreezeAnimation();
  
  // animation related keys handling
  bool key_decodation(sf::Keyboard::Key key);
  
  bool isAnimationActive() const;

  bool ifFreezeTimeStopActive() const;

  // Make flash light temporary effect
  void resumeTimeFlow();

  bool demoGenerator(long int demoCnt, bool resetAction);

  // end of Closing animation
  bool stopAtZero;
  
  private:
    
  // Angle Movement animation state
  AngleAnimState angle_anim_state;
  // Resize animation state
  ResizeAnimState resize_anim_state;

  StopType stopType;
  
  // single step animation delta
  std::array<float, cFrac::NrOfElements> angle_anim_delta;
    // Current angles (float) - animation needs track accurate angles
  std::array<float, cFrac::NrOfElements> angles_tf;
  
  // Angles animation
  void one_step_closing();
  void one_step_opening();
  void make_angle_delta_table();
  // resizing animation
  void one_step_resizing();
};
