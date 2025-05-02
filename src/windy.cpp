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
#include <cmath>

MovWind::T_PreSinuses MovWind::precalc_sinuses() {
  T_PreSinuses pre_copy;
  unsigned int angle;
  float angle_rad;
  float temp;
  // std::cout << "angle : sinus\n";
  for (unsigned int i=0; i < AngleTableSize; ++i) {
    angle = i * AngleLap;
    // Converting degrees to radians
    angle_rad = angle * 3.14159 / 180.f;
    temp = sin(angle_rad) * MaxDeviation;
    //Print this
    pre_copy[i] = static_cast<int>(temp);
    // std::cout << angle << " : " << temp << " : " << pre_copy[i] << "\n";
  }
  return pre_copy;
}

// animation related keys handling
bool MovWind::key_decodation(sf::Keyboard::Key key) {
  if (key == sf::Keyboard::Tilde) {
    if (!wind_anim_state) {
      // If first started store unmodified transformation
      original_algo_data = algo_data;
    }
    // start wind (wobbling) modification
    wind_anim_state = true;
    return true; // my key
  }
  else {
    // Not my key
    return false;
  }
}

void MovWind::stop_wind() {
  if (wind_anim_state) {
    // stop wind (wobbling) modification if it was running
    wind_anim_state = false;
    // restore non-modified transformation algo
    algo_data = original_algo_data;
    }
  // reset started flags
  modif_algo_data = {};
}


// called once in a display loop
void MovWind::one_step_cfg_change() {
  // call also other animation
  MovAnim::one_step_cfg_change();
  
  if (wind_anim_state) {
    // go through up branch
    for (unsigned int br = 0; br < cFrac::NrOfElements; ++br) {
      if (modif_algo_data[br].started_up) {
        // roll through sinuses values circular buffer
        auto temp_ind = modif_algo_data[br].modif_index_up;
        ++temp_ind;
        if (temp_ind > AngleTableSize -1) {
          temp_ind = 0;
        }
        // modify transformation algo_data
        algo_data[br].angle = original_algo_data[br].angle 
          + pre_modif[temp_ind];
        // store modification index
        modif_algo_data[br].modif_index_up = temp_ind;
      }
      else {
        // random start it
        if ((rand() % AngleTableSize) == 0)
          modif_algo_data[br].started_up = true; 
      }
    }    

    // go through down branch separatelly - assymetrical animation
    for (unsigned int br = 0; br < cFrac::NrOfElements; ++br) {
      if (modif_algo_data[br].started_down) {
        // roll through sinuses values circular buffer
        auto temp_ind = modif_algo_data[br].modif_index_down;
        ++temp_ind;
        if (temp_ind > AngleTableSize -1) {
          temp_ind = 0;
        }
        // modify transformation algo_data
        algo_data[br].angle_down = original_algo_data[br].angle_down 
          + pre_modif[temp_ind];
        // store modification index
        modif_algo_data[br].modif_index_down = temp_ind;
      }
      else {
        // random start it
        if ((rand() % AngleTableSize) == 0)
          modif_algo_data[br].started_down = true; 
      }
    }
  }
}


