// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "animation.h"
#include "fractal.h"
#include "transform.h"
#include "demo_func.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

bool MovAnim::key_decodation(sf::Keyboard::Key key) {
  switch (key) {
  // Angle Animation Group
  case sf::Keyboard::O: // switch to opening
    if (angle_anim_state != angleOpening) {
      make_angle_delta_table();
    }
    angle_anim_state = angleOpening;
    return true;
  case sf::Keyboard::C: // switch to Closing
    if (angle_anim_state != angleClosing) {
      make_angle_delta_table();
    }
    angle_anim_state = angleClosing;
    return true;

  case sf::Keyboard::P: // Rotate 
    stopAnimation();
    // Change to another pre-calculated configuration
    rotate_pre_cfg();
    stopAtZero = false;
    return true;

  // Size Animation Group
  case sf::Keyboard::V:
    // Enlarge first / larger
    resize_anim_state = resizeMaxLarger;
    return true; // my key found
  case sf::Keyboard::B:
    // Diminish first / larger
    resize_anim_state = resizeMaxSmaller;
    return true;
  case sf::Keyboard::N:
    // Enlarge last / smaller
    resize_anim_state = resizeMinLarger;
    return true;
  case sf::Keyboard::M:
    // Diminish last / smaller
    resize_anim_state = resizeMinSmaller;
    return true;
  default:
    // Not an error: other keys can be handled by another key_decodation 
    return false;
  }
}

bool MovAnim::isAnimationActive() const {
  return ((angleStop != angle_anim_state) or 
          (resizeStop != resize_anim_state));
}

void MovAnim::stopAnimation() {
  stopType = justStop;
  angle_anim_state = angleStop;
  resize_anim_state = resizeStop;
}

// Freeze (time) or Stop animation
void MovAnim::stopFreezeAnimation() {
  if (isAnimationActive() or (justStop == stopType)) {
    // First Stop - freeze time
    stopType = freezeTimeStop;
  }
  else {
    // Second Stop
    resumeTimeFlow();
  }

  angle_anim_state = angleStop;
  resize_anim_state = resizeStop;
}


bool MovAnim::ifFreezeTimeStopActive() const {
  if (stopType == freezeTimeStop) return true;
  else return false;
} 

void MovAnim::resumeTimeFlow() {
  stopType = justStop; // No freezeTimeStop
}


void MovAnim::make_angle_delta_table() {
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    // Make tracking copy of angles
    angles_tf[ind] = static_cast<float>(algo_data[ind].angle);
    // Calculate single step of angle animation
    angle_anim_delta[ind] = static_cast<float>(algo_data_init[ind].angle)/NR_OF_STEPS_ANIM;
  }  
}

// Single step display loop
void MovAnim::one_step_cfg_change(){
  // Angle change animation
  switch (angle_anim_state) {
  case angleClosing:
    one_step_closing(); 
    break;
  case angleOpening:
    one_step_opening(); 
    break;
  default: break;
  }
  
  // Size change animation
  if (resize_anim_state != resizeStop) {
    one_step_resizing();
  }
}

void MovAnim::one_step_closing() {
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    angles_tf[ind] -= angle_anim_delta[ind];
    if (angles_tf[ind] <= 0.0f) { // Stop at zero
      stopAtZero = true;
      angles_tf[ind] = 0.0f;
      algo_data[ind].angle = 0;
    } else {
      stopAtZero = false;
      algo_data[ind].angle = static_cast<int>(angles_tf[ind]);
      // symmetrical animation
      algo_data[ind].angle_down = -algo_data[ind].angle;  
    }
  }  
}

void MovAnim::one_step_opening() {
  for (size_t ind {0}; ind < cFrac::NrOfElements; ++ind) {
    angles_tf[ind] += angle_anim_delta[ind];
    if (angles_tf[ind] >= 160.0f * cTran::accurAngleMltp_f) { // Stop at wide angle
      angles_tf[ind] = 160.0f * cTran::accurAngleMltp_f;
      algo_data[ind].angle = 160 * cTran::accurAngleMltp;
    } else {
      algo_data[ind].angle = static_cast<int>(angles_tf[ind]);
      // symmetrical animation
      algo_data[ind].angle_down = -algo_data[ind].angle;  
    }
  }  
}

void MovAnim::one_step_resizing(){
  float scale_resize;
  
  switch (resize_anim_state) {
  case resizeMaxLarger: 
    // enlarge first element
    scale_resize = algo_data.front().scale;
    if (scale_resize < ANIM_SCALE_MAX) {
      scale_resize += ENLARGE_SIZE_STEP;
      // recalculate whole transf cfg to be proportional
      // and keep current angles
      algo_data = get_prop_cfg_arr( scale_resize, algo_data.back().scale);
    }
    break;
  case resizeMaxSmaller: 
    // diminish first element
    scale_resize = algo_data.front().scale;
    if (scale_resize > ANIM_SCALE_MIN) {
      scale_resize *= DIMINISH_SIZE_STEP;
      // recalculate whole transf cfg to be proportional
      // and keep current angles
      algo_data = get_prop_cfg_arr( scale_resize, algo_data.back().scale);
    }
    break;
  case resizeMinLarger:
    // enlarge last element
    scale_resize = algo_data.back().scale;
    if (scale_resize < ANIM_SCALE_MAX) {
      scale_resize += ENLARGE_SIZE_STEP;
      // recalculate whole transf cfg to be proportional
      // and keep current angles
      algo_data = get_prop_cfg_arr( algo_data.front().scale, scale_resize);
    }
    break;
  case resizeMinSmaller:
    // diminish last element
    scale_resize = algo_data.back().scale;
    if (scale_resize > ANIM_SCALE_MIN) {
      scale_resize *= DIMINISH_SIZE_STEP;
      // recalculate whole transf cfg to be proportional
      // and keep current angles
      algo_data = get_prop_cfg_arr( algo_data.front().scale, scale_resize);
    }
    break;
  default: break;
  }
}

bool MovAnim::demoGenerator(long int demoCnt, bool resetAction) {

  static long int lastActionDistance { 0 };
  
  if (demoCnt == cFrac::DemoInitCnt) {
    // seed generation
    srand(time(NULL));
  } else if (resetAction) {
    lastActionDistance = 0;
  }
  
  // Active event occured from this generator
  bool actionDone = false;
  ++lastActionDistance;

  if (angle_anim_state != angleStop ) {
    if (demoRand(30, lastActionDistance))  {
      if (angleClosing == angle_anim_state) {
        stopAnimation();
      } else {
        angle_anim_state = angleClosing;
      }
      
    }
  } else {
    
    // try to move
    if (demoRand(60, lastActionDistance))  {
      // Prepare animation close or open
      make_angle_delta_table();
      if (algo_data[0].angle < algo_data_init[0].angle) {
        // Angle is currently more closed then initial
        angle_anim_state = angleOpening;
      } else {
        angle_anim_state = angleClosing;
      }
      actionDone = true;
    }
  
  }

  return actionDone;
}
