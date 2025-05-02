// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "dbg_report.h"
#include "fractal.h"
#include "light.h"
#include "assert.h"
#include <SFML/Graphics/Color.hpp>
#include <array>

// Color definition in sf::Color:
// Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255);

struct StemColor {
  sf::Color begin_c;
  sf::Color end_c;
};

// Colors for 0-NrOfrders stems
using T_Col_Palet = std::array<StemColor, cFrac::NrOfOrders +1>;

struct ColorPal {

  // Diff algos for random palletes generation
  enum RandPalettes { palSpcRnd1, palJustRnd };

  // maximum range (Uint8) of Color component used in sf::Color
  constexpr static int cRGB_ColorMax {255};

  ColorPal() 
    : nr_rnd_palet { palSpcRnd1 }
    , m_indexPre { 0 }
  {
    Dbg::report_info("Init: ColorPal  ", preCalcColorPaletes.size());

    assert(preCalcColorPaletes.size() > 0);
    // default palette first from collection
    s_col_palet = preCalcColorPaletes[0];
    
    // Temporary initialize flash palette;
    // shall be initialize with right color by
    // derived LogKey class calling function: calc_flash_color_pallet()
    s_flash_col_palet = s_col_palet;
  }

  virtual ~ColorPal(){}

  // this is used by draw_stem() procedure (semi-global data)
  static T_Col_Palet s_col_palet;
  // used in flash effect
  static T_Col_Palet s_flash_col_palet;
  
  // switching global flash effect - used by draw_stem()
  static bool s_global_flash;
  static bool s_reset_flash_algo;
  
// called once in a display loop
// to switch off possible global control flags
// after signle frame (cycle)
  void one_step_flash_reset() {
    s_global_flash = false;
    s_reset_flash_algo = false;
  }
  
  // Restore unmodified pallet
  void reset_cur_color_pallet() { 
    s_col_palet = preCalcColorPaletes[0];
  
    // Start flash algo from beginning (AngleUnknown)
    s_reset_flash_algo = true;
  }
  
  // Reset only flash_algo
  void reset_flash_algo() {
      s_reset_flash_algo = true;
  }
  
  // key change pallette
  bool key_decodation(sf::Keyboard::Key key);
  
  // log current random pallete to file
  std::string log_rnd_color_pallet();

  // Calculate modified pallete used for flash effect
  void calc_flash_color_pallet(sf::Color active_light_col);

  bool demoGenerator(long int demoCnt, bool resetAction);
  
private:
  // Random color generator
  RandPalettes nr_rnd_palet;
  
  // Init collection of pre-calculated Color Palletes
  void initPreCalcColorPaletes();

  void rotate_fix_color_pallet();
  // rotate two algo's of random generation
  void generate_rnd_color_pallet();
  
  // diffrent random colors algo's
  T_Col_Palet just_random_colors();
  T_Col_Palet special_random_colors_v1();

  // Dimm or Reinforce colors
  void calc_updown_color_pallet(bool up);
  
  int m_indexPre;

  // Collection of Colors Palettes
  // to be rotated by 'K'
  const std::array<T_Col_Palet, 9U> preCalcColorPaletes = {{

  // Green contrast
   {{{ sf::Color( 731908607), sf::Color( 2627899391)},
   { sf::Color( 2618069247), sf::Color( 1011033343)},
   { sf::Color( 303196927), sf::Color( 2384414463)},
   { sf::Color( 912034047), sf::Color( 1888918783)},
   { sf::Color( 861969663), sf::Color( 1885248767)},
   { sf::Color( 459802879), sf::Color( 436417791)},
   { sf::Color( 2022977023), sf::Color( 420882431)},
   { sf::Color( 731908607), sf::Color( 2627899391)}}},
   
  // Green Magenta (out in) rather dark
   {{{ sf::Color( 2461533951), sf::Color( 3073869567)},
   { sf::Color( 371171583), sf::Color( 1633070847)},
   { sf::Color( 2634051327), sf::Color( 580331775)},
   { sf::Color( 73313535), sf::Color( 562789119)},
   { sf::Color( 760625151), sf::Color( 783045119)},
   { sf::Color( 2343711231), sf::Color( 142633471)},
   { sf::Color( 2265474303), sf::Color( 2220589055)},
   { sf::Color( 2461533951), sf::Color( 3073869567)}}},

  // Red light Blue
   {{{ sf::Color( 254982143), sf::Color( 3155692799)},
   { sf::Color( 529421823), sf::Color( 2720104703)},
   { sf::Color( 52460799), sf::Color( 1354069247)},
   { sf::Color( 2617609215), sf::Color( 221485311)},
   { sf::Color( 1437441023), sf::Color( 942948607)},
   { sf::Color( 1780516607), sf::Color( 496080383)},
   { sf::Color( 2887459071), sf::Color( 1731543039)},
   { sf::Color( 254982143), sf::Color( 3155692799)}}},

  // Magenta some yellow
   {{{ sf::Color( 2727846911), sf::Color( 3193132543)},
   { sf::Color( 4194103807), sf::Color( 1382775295)},
   { sf::Color( 3910341887), sf::Color( 1361961727)},
   { sf::Color( 1444908799), sf::Color( 660015359)},
   { sf::Color( 3557140479), sf::Color( 2186465535)},
   { sf::Color( 460683263), sf::Color( 3981089279)},
   { sf::Color( 1782405375), sf::Color( 3577107199)},
   { sf::Color( 254982143), sf::Color( 3155692799)}}},
  
  // Orange Green (light) - make some dimm
   {{{ sf::Color( 857206015), sf::Color( 4012588799)},
   { sf::Color( 3854343935), sf::Color( 4079067135)},
   { sf::Color( 733078271), sf::Color( 3978169855)},
   { sf::Color( 475018239), sf::Color( 3114148351)},
   { sf::Color( 2652132607), sf::Color( 734064127)},
   { sf::Color( 3069096191), sf::Color( 1237862655)},
   { sf::Color( 3407483391), sf::Color( 3636280831)}}},

  // Sandy and Dark Blue
   {{{ sf::Color( 3114879231), sf::Color( 2920590335)},
   { sf::Color( 682481407), sf::Color( 3636337407)},
   { sf::Color( 3927388415), sf::Color( 3198239231)},
   { sf::Color( 2907304191), sf::Color( 1034231551)},
   { sf::Color( 1475226367), sf::Color( 927742207)},
   { sf::Color( 741852415), sf::Color( 1430035711)},
   { sf::Color( 4276432639), sf::Color( 506412031)}}},

  // Gray Red
   {{{ sf::Color( 1970106367), sf::Color( 3868171007)},
   { sf::Color( 176790527), sf::Color( 2604803839)},
   { sf::Color( 2372798975), sf::Color( 2543045119)},
   { sf::Color( 1122227455), sf::Color( 2510834943)},
   { sf::Color( 755072767), sf::Color( 4097909503)},
   { sf::Color( 1664180991), sf::Color( 1925961727)},
   { sf::Color( 4217847551), sf::Color( 3171764735)}}},
  
  // Blue White (colored)
   {{{ sf::Color( 2534622719), sf::Color( 579972095)},
   { sf::Color( 421378303), sf::Color( 1001003007)},
   { sf::Color( 478846719), sf::Color( 373039871)},
   { sf::Color( 2248441855), sf::Color( 2453081599)},
   { sf::Color( 330614015), sf::Color( 958628095)},
   { sf::Color( 2813534719), sf::Color( 1294770943)},
   { sf::Color( 2370774271), sf::Color( 2617771263)}}},

  // Colored - more jagged than previous
   {{{ sf::Color( 1228813311), sf::Color( 1337086463)},
   { sf::Color( 490523647), sf::Color( 1086274047)},
   { sf::Color( 289129471), sf::Color( 95401727)},
   { sf::Color( 2699553791), sf::Color( 87555839)},
   { sf::Color( 283451647), sf::Color( 3055362815)},
   { sf::Color( 1176701695), sf::Color( 3712573695)},
   { sf::Color( 2671986943), sf::Color( 3720109567)}}},
  // The Array can be extended or elements replaced taking data from snapshot log
 }};

};
