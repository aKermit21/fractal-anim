// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "windy.h"
#include "animation.h"
#include "dbg_report.h"
#include "fractal.h"
#include "transform.h"
#include <cmath>
#include <cstdlib>

// animation related keys handling
bool MovWind::key_decodation(sf::Keyboard::Key key) {
  if (key == sf::Keyboard::Tilde) {
    if (!wind_anim_state) {
      // If just started store unmodified transformation
      original_algo_data = algo_data;
      // copy Algo per level
      algo_data_wind = conv_to_wind(algo_data);
      srand( time(NULL));
      // start wind (wobbling) modification
      wind_anim_state = true;
    } else {
      wind_anim_state = false;
    }
    return true; // my key
  }
  else {
    // Not my key
    return false;
  }
}

T_Wind_Algo_Arr MovWind::conv_to_wind(T_Algo_Arr assym_algo){
  T_Wind_Algo_Arr temp_algo;
  for (size_t level {0}; level < cFrac::NrOfOrders; ++level) {
    for (size_t elem {0}; elem < cFrac::NrOfElements; ++elem) {
      temp_algo[level][elem].angle = assym_algo[elem].angle;
      temp_algo[level][elem].angle_down = assym_algo[elem].angle_down;
      // Those below two shall not be varing
      temp_algo[level][elem].repos = assym_algo[elem].repos;
      temp_algo[level][elem].scale = assym_algo[elem].scale;
    }
  }
  return temp_algo;
}

void MovWind::stop_wind() {
  if (wind_anim_state) {
    // stop wind (wobbling) modification if it was running
    wind_anim_state = false;
    // restore non-modified transformation algo
    algo_data = original_algo_data;
    // reset flash algo done by higher aggreg class
    }
}


// called once in a display loop
void MovWind::one_step_cfg_change() {
  // call also other animation
  MovAnim::one_step_cfg_change();
  constexpr static int cTolerance { 25 };

  if (wind_anim_state) {
    for (size_t level {0}; level < cFrac::NrOfOrders; ++level) {
      int step = cTran::accurAngleMltp / 3;
      // higher level bigger trembling
      step *= level+1;
      for (size_t elem {0}; elem < cFrac::NrOfElements; ++elem) {
        auto delta = algo_data_wind[level][elem].angle - algo_data[elem].angle;
        auto delta_down = algo_data_wind[level][elem].angle_down
                          - algo_data[elem].angle_down;

        // Assymetric random changes for too big deviations
        if (delta > cTran::accurAngleMltp * cTolerance) {
          algo_data_wind[level][elem].angle += (rand() % (2*step)) - 3*step/2;
        } else if (delta < - cTran::accurAngleMltp * cTolerance) {
          algo_data_wind[level][elem].angle += (rand() % (2*step)) - 1*step/2;
        } else {
          // Random symmetric changes otherwise
          algo_data_wind[level][elem].angle += (rand() % (2*step)) - step;
        }
        
        // Assymetric random changes for too big deviations
        if (delta_down > cTran::accurAngleMltp * cTolerance) {
          algo_data_wind[level][elem].angle_down += (rand() % (2*step)) - 3*step/2;
        } else if (delta_down < - cTran::accurAngleMltp * cTolerance) {
          algo_data_wind[level][elem].angle_down += (rand() % (2*step)) - 1*step/2;
        } else {
          // Random symmetric changes otherwise
          algo_data_wind[level][elem].angle_down += (rand() % (2*step)) - step;
        }
      }
    }
  }
  
}


