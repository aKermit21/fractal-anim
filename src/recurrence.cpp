// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "animation.h"
#include "autoscale.h"
#include "fractal.h"
#include "dbg_report.h"
#include "garbage_coll.h"
#include "transform.h"
#include "windy.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <chrono>

bool recurance_elements_creation(Element * const parent_ptr, const short level)
{
  static unsigned long recur_funct_cnt { 0 };

  if (level > cFrac::NrOfOrders) { 
    //Stop further branches
    parent_ptr->children_down = nullptr;
    parent_ptr->children_up = nullptr;
    return false; // nothing more to create
  }

  if (level == 1) {
    recur_funct_cnt = 0; // reset recurrance counter so it will count per cycle
    Dbg::report_info("Element size(of): ", sizeof(Element) );
  } else { ++recur_funct_cnt; }

  // Warn if too much elemnts created
  if (recur_funct_cnt >= Dbg::cCreateWarningThreshold) {
    Dbg::report_mltpl_warning(Dbg::mltplElementsCreate, recur_funct_cnt);
  }
  
  //Setup DOWN branch
  auto u_ptr_down_temp { std::make_unique<std::array<Element, cFrac::NrOfElements>>() };
  parent_ptr->children_down = u_ptr_down_temp.get(); // ordinary ptr to a structure
  // Move Unique Ptr ownership to dedicated (garbage) Collection
  MemAndDebug::collect_u_ptr(move(u_ptr_down_temp));
  Dbg::count_elements(cFrac::NrOfElements);
  short ind;
  ind = 0;
  for(auto it = parent_ptr->children_down->begin(); it != parent_ptr->children_down->end(); ++it ) {
    it->order = level;
    it->b_type = downBranch;
    it->index = ++ind;  // 1..cFrac::NrOfElements
    it->stem_xy.prev_l_angle = lAngleUnknown;
    it->parent_ptr = parent_ptr; // link to already existing parent
    // Try create next level
    recurance_elements_creation(it, level+1);
  }
  
  // Setup UP branch
  auto&& ptr_up_temp = std::make_unique<std::array<Element, cFrac::NrOfElements>>();
  parent_ptr->children_up = ptr_up_temp.get();  // ordinary ptr to a structure
  // Move Unique Ptr ownership to dedicated (garbage) Collection
  MemAndDebug::collect_u_ptr(move(ptr_up_temp));
  Dbg::count_elements(cFrac::NrOfElements);
  ind = 0;
  for(auto it = parent_ptr->children_up->begin(); it != parent_ptr->children_up->end(); ++it ) {
    it->order = level;
    it->b_type = upBranch;
    it->index = ++ind;  // 1..cFrac::NrOfElements
    it->stem_xy.prev_l_angle = lAngleUnknown;
    it->parent_ptr = parent_ptr; // link to already existing parent
    // Try create next level
    recurance_elements_creation(it, level+1);
  }
  
  return true; // something created
 
}


bool recurance_elements_redraw(Element * const parent_ptr, const short level, 
           sf::RenderWindow &win, const MovWind &algo_anim, AutoScale & autoscale)
{
  static long recur_funct_cnt { 0 };

  // Only for testing/debugging
  if (Dbg::cReportWarning) {
    // needed calculation of time between frames
    static auto prev_time = std::chrono::high_resolution_clock::now();
    
    if (level == 0) {
      Dbg::report_info("Elements Drawn per cycle: ", recur_funct_cnt);
      recur_funct_cnt = 0; // reset recurrance counter so it will count per cycle

      // time between frames
      auto next_time = std::chrono::high_resolution_clock::now();
      double elapsed_time_ms = 
        std::chrono::duration<double, std::milli>(next_time - prev_time).count();
      Dbg::report_info("Time per frame (ms): ", elapsed_time_ms);
      prev_time = next_time;
    } else { ++recur_funct_cnt; }

    // Warn if too much elemnts drawed per cycle
    if (recur_funct_cnt >= Dbg::cDrawWarningThreshold) {
      Dbg::report_mltpl_warning(Dbg::mltplElementsDraw, recur_funct_cnt);
      return false;
    }
  }

  if (level > 0) {  // level 0 does NOT undergo transformation
    // Tranform this vector (base on settings copied from parent) to the new one 
    parent_ptr->transform_vec_stem(algo_anim.algo_data,
                                   algo_anim.algo_data_wind,
                                   algo_anim.wind_anim_state);
  }

  autoscale.findMinMax(parent_ptr->stem_xy.vec_xy);

  // Draw the element
  parent_ptr->stem_xy.draw_stem(win, level, algo_anim.ifFreezeTimeStopActive());

  if (level >= cFrac::NrOfOrders) { 
    return false; // no more branches to scan
  }

  // Take approx vector length : |dx| + |dy| ~ sqrt(dx2 + dy2)
  auto approx_vec =  abs(parent_ptr->stem_xy.vec_xy.dx) + 
                     abs(parent_ptr->stem_xy.vec_xy.dy); 
  
  // Consider element size limits on going to deeper branch
  // If size below threshold do not continue with children
  if (algo_anim.isAnimationActive()) {
    if (approx_vec < TranAlg::s_SmallVecAnim) { // animation vector length threshold
      return false; // do not go deeper 
      }
  } else {
    if (approx_vec < TranAlg::s_SmallVect) { // static vector length threshold
      return false; // do not go deeper 
    }
  }  

  if (parent_ptr->children_down == nullptr) {
    Dbg::report_error("Next level not initialized: ", level+1);
    return false;
  }

  // Follow DOWN branch
  for(auto it = parent_ptr->children_down->begin(); it != parent_ptr->children_down->end(); ++it ) {
    // Traverse next level
    // Propagate (copy) parent position/vector to child (vec_xy is overriten!)
    it->stem_xy.vec_xy = parent_ptr->stem_xy.vec_xy; 
    recurance_elements_redraw(it, level+1, win, algo_anim, autoscale);
  }
  
  // Follow UP branch
  for(auto it = parent_ptr->children_up->begin(); it != parent_ptr->children_up->end(); ++it ) {
    // Traverse next level
    // Propagate (copy) parent position/vector to child (vec_xy is overriten!)
    it->stem_xy.vec_xy = parent_ptr->stem_xy.vec_xy; 
    recurance_elements_redraw(it, level+1, win, algo_anim, autoscale);
  }
  
  return true; // recurance continue
 
}
