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
    pre_modif = precalc_sinuses();
    srand( time(NULL));
  }

  struct WindRec {
    bool started_up; // this is to start diffrent branches on diffrent time
    unsigned int modif_index_up; // indexing (rolling through) sinus table
    // The same for down branches
    bool started_down; 
    unsigned int modif_index_down; 
  };

  // animation related keys handling
  bool key_decodation(sf::Keyboard::Key key);

  // called once in a display loop
  // to realize animation
  void one_step_cfg_change();

  void stop_wind();

private:

  constexpr static unsigned int AngleLap = 10;
  // Max angle deviation of wind factor (0.1 deg)
  constexpr static unsigned int MaxDeviation = 15;

  constexpr static unsigned int AngleTableSize = static_cast<unsigned int>(360 / AngleLap);
  
  bool wind_anim_state;
  
  // Array of precalcuated modifications
  // (360 - going around with sinus)
  using T_PreSinuses = std::array<int, AngleTableSize>;
  T_PreSinuses pre_modif;

  // Unmodified (by wind algo) transformation data
  T_Algo_Arr original_algo_data;

  // modification to be impose on top of algo_data
  std::array<WindRec, cFrac::NrOfElements> modif_algo_data {};

  // modification indexes for Lower branch
  // to get asymmetrical wobbling
  std::array<WindRec, cFrac::NrOfElements> modif_algo_data_down {};

  T_PreSinuses precalc_sinuses();
};
