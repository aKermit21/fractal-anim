// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <array>
#include <iterator>
#include <memory>
#include "config.h"
#include <SFML/Graphics.hpp>

// Basic CONFIG Constants
namespace cFrac {
  inline const std::string ProgramName { cFullProgramName }; // from config.h
  inline const std::string DemoProgramName { "DEMO -- " +ProgramName+ " -- DEMO" };
  inline const std::string Version { VERSION_STR }; // from config.h
  
  inline constexpr int NrOfElements { 5 }; // number of elements in one child branch  
  // if > 5, consider extending definition of "Transformation Data" in transform.h

  // maximal possible nesting, down generations, counting from 0 (primary) to 7
  inline constexpr int NrOfOrders { 7 }; 
  // can be problematic to increase

  // Graphic visible window  
  inline constexpr int WindowXsize { 1100 }; 
  inline constexpr int WindowYsize { 900 }; 
  
  // Primary element size, position - assuming window 1100x900
  inline constexpr int PrimStartX { 30 };
  inline constexpr int PrimStartY { 530 };
  inline constexpr int PrimVecX { 1000 };
  inline constexpr int PrimVecY { -80 };
  inline constexpr int PrimStemWidth { 10 };
  
  // counter mark demo initialization
  inline constexpr int DemoInitCnt { 1 };
}

enum BranchType { upBranch, downBranch, firstBranch };
enum LightAngleCase {lAngleUnknown = 0, lAngleAbove90, lAngleBelow90};

// other Config Constants in tranform.h


// 2D vector and its current position - x1,y1 -> x2,y2 aka x,y dx,dy
// Warning: values are initially populated (overwritten) 
//          from parent then recalculated
struct Vec2D {
  int x;
  int y;
  int dx;
  int dy;
  // move along x,y and x+dx,y+dy line in promile of original line
  // affects x,y - shall be done before rotation
  void reposition(const int promile);
  // rotate and scale - affects dx,dy
  // angle in 0.1deg scale
  void rotate(const int angle, const float scale);

  // had angle between light rays and (this) vec changed (<90 vs >90 deg)
  bool light_vec_angle_flip();
private:
  template<typename T> // typically float or double
  void rotation_matrix(const T sin_val,
                       const T cos_val); // rotation matrix calculations
};

// Additional points used for drawing stem with thickness
// Warning: x#,y# values are recalculated each frame
struct Stem{
  // thickness type
  enum ThickLevel {thickNone, thick1, thick2};
  Vec2D vec_xy; // bare line along centre of stem
  int x1 {};  // additional points at base of stem
  int y1 {};  // deciding about their drawn thickness
  int x2 {};
  int y2 {};
  // move along x,y and x+dx,y+dy line - 
  // affects x,y and x1,y1,x2,y2 - shall be done before rotation;
  // used for contructing child element based on parent
  void reposition_stem(const int percent, const ThickLevel level);
  // move by given (absolute) dx dy
  void repositionStemAbsolute(const int dx, const int dy);
  // Shrink stem according to given (usable) window Center
  void shrinkStemCenter(float factor, float cumulativeFactor, int xCenter, int yCenter);
  // Calculate coordinates of stem with some possible adjustmement (due to autoscale)
  void recalculateStemWidthCoordinates(float cumulativeFactor);

  virtual void draw_stem(sf::RenderWindow &win, short level, const bool freezeTime);
  // // to be used by Flash Light version
  // virtual bool light_vec_angle_flip() = 0;
};

// Stem with additional Flash Light handling
// values remain from previous frame unless explicitelly changed
struct StemFlash : Stem {
  virtual void draw_stem(sf::RenderWindow & win, short level, const bool freezeTime);
  // light angle from previous frame / cycle
  LightAngleCase prev_l_angle;
  // active light flash of stem for # of frames  
  int flash_cnt { 0 }; // no light flash
  // had angle between light rays and stem/vec changed (<90 vs >90 deg)
  bool light_vec_angle_flip();
private:
  LightAngleCase light_vec_angle(int vx, int vy);
};


// Common transformation basics - see tranform.h
struct DRec {
  int repos;  // move (reposition) in promile
  int angle;  // of a rotation for up branch
  int angle_down;  // of a rotation for down branch (if symmetrical then just -angle)
  float scale; 
};

struct DRecSymm {
  int repos;  // move (reposition) in promile
  int angle;  // of a rotation in 0.1 deg
  float scale; 
};

using T_Algo_Arr = std::array<DRec, cFrac::NrOfElements>;
using T_Algo_Arr_Symm = std::array<DRecSymm, cFrac::NrOfElements>;


/* Single Element of Fractal */
struct Element {
  short order { 0 }; // nesting level
  short index {};     // position within current branch - 1..cTran::NrOfElements
  BranchType b_type = firstBranch; // First branch valid only for first element
  // vetor / delta coordinates / stem thickness / Flash Light
  StemFlash stem_xy;   
  // Attached up/downside child elements (of next order)
  // in form of link list
  std::array<Element, cFrac::NrOfElements> * children_down {};
  std::array<Element, cFrac::NrOfElements> * children_up {};
  // Link to parent (single one)
  Element *parent_ptr = {}; // pointer to previous already existing object
  // Tranform vec/stem from parent using special transformation array
  // - method for static (single frame) drawing
  void transform_vec_stem(T_Algo_Arr const & algo_data); 
  void initPrimary();   // Init data for first element 
};


