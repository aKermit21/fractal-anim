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
#include "animation.h"
#include <array>

// Modification of Transformation Algorithm 
// by adding wiggling effect (like from wind) to angle component

struct MovWind : MovAnim {

  MovWind(int speed=10) 
    : MovAnim{ speed }
  { 
    Dbg::report_info("Init: TrWindy (speed=)", speed); 
    wind_anim_state = false;
    original_algo_data = algo_data;
  }

  // More general algo - each level has diffrent angle
  // using T_Wind_Algo_Arr = std::array<T_Algo_Arr, cFrac::NrOfOrders>;
  
  // struct WindRec {
  //   bool started_up; // this is to start diffrent branches on diffrent time
  //   unsigned int modif_index_up; // indexing (rolling through) sinus table
  //   // The same for down branches
  //   bool started_down; 
  //   unsigned int modif_index_down; 
  // };

  // animation related keys handling
  bool key_decodation(sf::Keyboard::Key key);

  // called once in a display loop
  // to realize animation
  void one_step_cfg_change();

  void stop_wind();

  bool wind_anim_state;
  
  // Transformation full algorithm data including windy effect
  // More specific algo: per level
  // TODO: Use this at re-draw
  T_Wind_Algo_Arr algo_data_wind;

private:

  T_Wind_Algo_Arr conv_to_wind(T_Algo_Arr);
  
  // Unmodified (by wind algo) transformation data
  T_Algo_Arr original_algo_data;

};
