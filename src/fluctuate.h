// Copyright (c) 2025-2026 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "fractal.h"
#include "animation.h"

// Modification of Transformation Algorithm 
// by adding wiggling effect (like from wind) to angle component or
// by adding growing effect to scale component

struct MovFluctuate : MovAnim {

  MovFluctuate(int speed=10) 
    : MovAnim{ speed }
    , fluctuateState {false, true}
    , growingDynamic {0} // all zero's except first element - see next lines of code
  { 
    Dbg::report_info("Init: MovFluctuate (speed=)", speed); 
    //TODO: Make this latter configurable by CLI option
    // fluctuateState.growing_anim_state = true; 
    // copy Algo per level
    algo_data_fluctuate = conv_to_fluctuate(algo_data);
    // Growing primary size starts from non-zero
    growingDynamic[0] = 3; 
    // Initialize algo_data_fluctuate table for growin before first display
    if (fluctuateState.growingActive) {
      oneStepGrowingChange();
    }
  }
  
  // Two possible fluctuation animations: shaking and (initial) growing
  // See FluctuateState defined in fractal.h
  FluctuateState fluctuateState;
  
  // animation related keys handling
  bool key_decodation(sf::Keyboard::Key key);

  // called once in a display loop
  // to realize fluctuation animation
  void one_step_cfg_change();
  
  void stop_wind();

  // Transformation full LIVE algorithm data including windy, growing effect
  // More specific algo: per level
  T_Fluctuate_Algo_Arr algo_data_fluctuate;

  T_Fluctuate_Algo_Arr conv_to_fluctuate(T_Algo_Arr);

private:
  
  // realize growing specific fluctuation change
  void oneStepGrowingChange();

  // Specifig growing dynamic (x0.1)
  std::array<int, cFrac::NrOfOrders+1> growingDynamic;
};
