// Copyright (c) 2025-2026 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "transform.h"
#include "dbg_report.h"
#include "fractal.h"
#include "aux_func.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstdlib>
#include <sstream>
#include <assert.h>

// Small vector - below this size stop recursive search/draw of children
float TranAlg::s_SmallVect;
float TranAlg::s_SmallVecAnim;


void Element::initPrimary() {
  // Use multiplied values for better accuracy tranformation
  stem_xy.vec_xy.x = cFrac::PrimStartX; 
  stem_xy.vec_xy.y = cFrac::PrimStartY; 
  stem_xy.vec_xy.dx = cFrac::PrimVecX; 
  stem_xy.vec_xy.dy = cFrac::PrimVecY; 
  // Make a copy of dx and dy
  stem_xy.vec_xy.originalDx = cFrac::PrimVecX; 
  stem_xy.vec_xy.originalDy = cFrac::PrimVecY; 
  // stem width
  stem_xy.x1 = stem_xy.vec_xy.x;
  stem_xy.x2 = stem_xy.vec_xy.x;
  stem_xy.y1 = stem_xy.vec_xy.y - cFrac::PrimStemWidth; 
  stem_xy.y2 = stem_xy.vec_xy.y + cFrac::PrimStemWidth; 
  // prevent initial angle change
  stem_xy.prev_l_angle = lAngleUnknown;
}


// Tranform parent vector (also stem data) to the child one 
// considering index and branch type
void Element::transform_vec_stem(const T_Fluctuate_Algo_Arr & algo_fluct_data) { 
  
  float fraction; // reposition begining of stem along parent line (in fraction)
  float angle;  // rotation
  float scale; // diminishing
  
  // Only (possible) special limited transformation of primary object
  if (order == 0) {
    // Dbg::report_info("Primary element tranformation" );
    stem_xy.vec_xy.dx =
      algo_fluct_data[0][0].scale * stem_xy.vec_xy.originalDx;
    stem_xy.vec_xy.dy =
      algo_fluct_data[0][0].scale * stem_xy.vec_xy.originalDy;
  } else  {
    // Transformation depends on index (element #)
    // but our branch index has range 1..cFrac::NrOfElements
    auto arr_index = index -1;
    if (arr_index < 0) {
      assert(false and "negative index"); // always assert
      Dbg::report_error("Branch Index out of range: ", index);
      fraction = 0.0; angle = 0.0; scale = 1.0;
    } else {
      fraction = algo_fluct_data.at(order).at(arr_index).repos; 
      if (b_type == upBranch) {
          angle = algo_fluct_data.at(order).at(arr_index).angle;
          scale = algo_fluct_data.at(order).at(arr_index).scale;
      }
      else {
          angle = algo_fluct_data.at(order).at(arr_index).angle_down;
          scale = algo_fluct_data.at(order).at(arr_index).scale;
        // For symmetrical
        //   angle = -angle; // reverse angle
      }
    }

    if (order == 1) {
      stem_xy.reposition_stem(fraction, Stem::thick2);
    } else  if (order == 2) {
      stem_xy.reposition_stem(fraction, Stem::thick1);
    } else {
      // Reposition vector only
      // for higher orders line without width
      stem_xy.vec_xy.reposition(fraction);
    }

    stem_xy.vec_xy.rotate(angle, scale);
    // dbg.report_trace("  rotation at angle", angle);
  }
}

void TranAlg::rotate_pre_cfg(){
  int maxElement = preCalcAlgoData.size();
  
  assert(maxElement >= 1);
  assert((m_indexPre >= 0) and (m_indexPre < maxElement));

  ++m_indexPre;
  if (m_indexPre >= maxElement) {
    m_indexPre = 0;
  }
  
  // Take next Predefined configuration
  // Working copy
  algo_data_init = conv_to_assym(preCalcAlgoData[m_indexPre]);
  // Initial setting
  algo_data = algo_data_init;
}

void TranAlg::reset_pre_cfg(){
  // Take first pre-calc algo from list
  algo_data = conv_to_assym(preCalcAlgoData[0]);
}


std::string TranAlg::log_trans_config() {
  std::stringstream ss;

  // Prepare config in form of toml string
  ss << "  [config.transform]\n";
  ss << "    # consecutive elements/branches\n";
  ss << "    element = [\n";
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    // inline toml table
    ss << "      {reposition = " << algo_data[ind].repos << ", ";  
    // 0.1deg used for easy manual reference
    ss << "angle = " <<
      static_cast<int>(myAux::radiansToZeroOneDegrees((algo_data[ind].angle)));
    ss << ", " << "scale = " << algo_data[ind].scale << "},\n";
  }
  ss <<"     ]\n";
  return ss.str();
}


// controlling smallest vector size to be drawn
// controls speed vs accurracy drawing
void TranAlg::speedIncrement(void){
  // Increase size thus speed
  ++m_speedScale;
  speedScaleVerify();
}

void TranAlg::speedDecrement(void){
  // Decrease size thus speed (but increase drawing details)
  --m_speedScale;
  speedScaleVerify();
  assert(m_speedScale >= 0); // index
}

// speed to small vector size calculation
void TranAlg::speedScaleVerify(void) {
  // Verify allowed range
  if (m_speedScale < 0) { m_speedScale = 0;}
  if (m_speedScale > SpeedScaleDataSize -1 ) { m_speedScale = SpeedScaleDataSize -1; }

  // Initial Speed vs Detailed drawing scale table
  s_SmallVect = SpeedScalaData[m_speedScale];
  s_SmallVecAnim = s_SmallVect *1.3f; // Animation rate
}


int TranAlg::get_speedScale() const {
  return m_speedScale;
}
  

// Convert Symmetrical initial algo to (potentially) Asymmetrical
// also convert 0.1deg (for manual reference) to internal radians
T_Algo_Arr TranAlg::conv_to_assym(T_Algo_Arr_Symm symm_algo) {
  T_Algo_Arr assym_algo;
  
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    assym_algo[ind].repos = symm_algo[ind].repos;
    // Use radians (float) from now on
    assym_algo[ind].angle =
      myAux::zeroOneDegreesToRadians(static_cast<float>(symm_algo[ind].angle));
    // initially just mirror angle
    assym_algo[ind].angle_down = - assym_algo[ind].angle;
    assym_algo[ind].scale = symm_algo[ind].scale;
  }
  return assym_algo;
}


// Calculate proportional cfg/transformation data
// changing only scale; angles are kept current
T_Algo_Arr TranAlg::get_prop_cfg_arr(float scale_max, float scale_min){
  T_Algo_Arr arr;

  // Take current version of ANGLE_MAX, ANGLE_MIN (first and last angle)
  float angle_max_f = static_cast<float>(algo_data.front().angle);
  float angle_min_f = static_cast<float>(algo_data.back().angle);
  Dbg::report_info("ANgle MAX: ", static_cast<long>(angle_max_f));
  Dbg::report_info("ANgle MIN: ", static_cast<long>(angle_min_f));

  // Needed for linear change
  float angle_step_f = (angle_max_f - angle_min_f) / (cFrac::NrOfElements -1);
  float scale_step = (scale_max - scale_min) / (cFrac::NrOfElements -1);

  float sum_repo = scale_max; // first lenght (scale) is counted twice
  // Linear change from Max to Min value
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    arr[ind].angle = angle_max_f - angle_step_f * ind;
    arr[ind].scale = scale_max - scale_step * ind;
    // symmetrical angles
    arr[ind].angle_down = -arr[ind].angle;
    sum_repo += arr[ind].scale; // will be needed for proportional calculations
  }

  // Branch reposition proportional to its high (scale)
  //  d1~h1  d2~h1  d3~h2 d4~h3 d5~h4 d6~h5  (d1=d2 - first high) 
  //  ______|______|_____|_____|_____|____

  float t_repo_prop = 0;   // reposition in proportional scale
  auto prev_it = arr.begin();  // iterator to previous element except first run
  for (auto it { arr.begin() }; it != arr.end(); ++it ) {
    t_repo_prop += prev_it->scale;   
    it->repos = t_repo_prop / sum_repo; // real reposition
    // for use in next loop
    prev_it = it;
  }
  
  return arr;
}

