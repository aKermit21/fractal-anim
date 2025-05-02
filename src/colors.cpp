// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "colors.h"
#include "dbg_report.h"
#include "fractal.h"
#include "light.h"
#include "demo_func.h"
#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <cassert>

// this is used by draw_stem() procedure (semi-global data)
T_Col_Palet ColorPal::s_col_palet;
// used in flash effect
T_Col_Palet ColorPal::s_flash_col_palet;

// switching global flash effect - used by draw_stem()
bool ColorPal::s_global_flash {false};
bool ColorPal::s_reset_flash_algo {false};
  
// Recalculate complete random colors
T_Col_Palet ColorPal::just_random_colors() {
  T_Col_Palet cpal;

  for (size_t i {0}; i <= cFrac::NrOfOrders; ++i) {
    // All components just random
    cpal[i] = {sf::Color(rand() % cRGB_ColorMax, rand() % cRGB_ColorMax,
                         rand() % cRGB_ColorMax),
               sf::Color(rand() % cRGB_ColorMax, rand() % cRGB_ColorMax,
                         rand() % cRGB_ColorMax)};
  }
  return cpal;
}

// Special random colors
// Considered only RGB combinations with at least on High, Low:
// for example High,Low,High
T_Col_Palet ColorPal::special_random_colors_v1() {

  // all needed constants below
  
  // Low, Low, Low = 0 - disabled
  // Low, Low, High = 1 - enabled
  //  ..all in between.... - enabled
  // High, High, High = 7 - disabled
  constexpr int cRGB_CombMax {6};

  // Two ranges: Low 0..100; High 155-255
  constexpr int cColorSpanMax {100};
  constexpr int cColorHighStart {cRGB_ColorMax - cColorSpanMax};

  // RGB
  constexpr unsigned int cRedCompMask {1u};
    // cGreenCompMask 2
    // cBlueCompMask 4

  // Color fading from Begin of Stem to its End
  constexpr int cColorBegin {0};
  constexpr int cColorEnd {1};

  T_Col_Palet cpal;

  unsigned int rgb_high_low_comb;
  // start from Red component
  unsigned int rgb_mask {cRedCompMask};
  std::array<std::array<unsigned int, 3>, 2> rand_two_colors;
  sf::Color rgb_color_begin, rgb_color_end;

  for (size_t br_i {0}; br_i <= cFrac::NrOfOrders; ++br_i) {           // branches
    for (size_t be_i {cColorBegin}; be_i <= cColorEnd; ++be_i) { // begin/end
      rgb_mask = cRedCompMask;
      for (size_t com_i {0}; com_i < 3; ++com_i) { // color component
        // generate random (low) level
        rand_two_colors[be_i][com_i] = (rand() % cColorSpanMax);
        // generate Low/High combination
        rgb_high_low_comb = (rand() % cRGB_CombMax) + 1; // 1..6
        if (rgb_high_low_comb & rgb_mask) {
          // shift to high level
          rand_two_colors[be_i][com_i] += cColorHighStart;
        }
        rgb_mask <<= 1; // shift mask to next RGB color/component
      }
    }
    // rgb_color_end = sf::Color(red_col, green_col, blue_col);

    cpal[br_i] = {sf::Color(rand_two_colors[cColorBegin][0],
                            rand_two_colors[cColorBegin][1],
                            rand_two_colors[cColorBegin][2]),
                  sf::Color(rand_two_colors[cColorEnd][0],
                            rand_two_colors[cColorEnd][1],
                            rand_two_colors[cColorEnd][2])};
  }
  return cpal;
}

void ColorPal::rotate_fix_color_pallet() {
  int maxElement = preCalcColorPaletes.size();

  assert(maxElement >= 1);
  assert((m_indexPre >= 0) and (m_indexPre < maxElement));

  ++m_indexPre;
  if (m_indexPre >= maxElement) {
    m_indexPre = 0;
  }
  
  s_col_palet = preCalcColorPaletes[m_indexPre];
}

void ColorPal::generate_rnd_color_pallet() {
  switch (nr_rnd_palet) {
  case palJustRnd:
    nr_rnd_palet = palSpcRnd1;
    s_col_palet = special_random_colors_v1();
    break;
  case palSpcRnd1:
    nr_rnd_palet = palJustRnd;
    s_col_palet = just_random_colors();
    break;
  default:
    Dbg::report_error("Unexpected Random Color Palette rotation: ", nr_rnd_palet);
    assert(false and "Unexpected Random Color Palette rotation"); // always assert
    break;
  }
}

// provide data with current random pallete to be logged
std::string ColorPal::log_rnd_color_pallet(){
  std::stringstream ss;
  ss << "Color Palette Log - ";
  switch (nr_rnd_palet) {
  case palJustRnd:
    ss << "  PalJustRnd :\n";
    break;
  case palSpcRnd1:
    ss << "  PalSpecialRnd1 :\n";
    break;
  default:
    Dbg::report_error("Unexpected Random Color Palette log attempt: ", nr_rnd_palet);
    break;
  }

  // print RGB colors in Uint32 (composite) format
  for (auto it { s_col_palet.begin() }; it != s_col_palet.end(); ++it ) {
    ss << " { sf::Color( " << it->begin_c.toInteger() << 
      "), sf::Color( "  << it->end_c.toInteger() << ")},\n";
  }

  return ss.str();
}


// Dimm or Reinforce colors
void ColorPal::calc_updown_color_pallet(bool up){
  float change_ratio;
  unsigned int temp;

  if (up) {
    // UP - reinforce all colors +20%
    change_ratio = 1.20;
  } else {
    // Dimm - reduce all colors -20%
    change_ratio = 0.80;
  }

  for (size_t order {0}; order <= cFrac::NrOfOrders; ++order) { 
    temp = static_cast<unsigned int>(s_col_palet[order].begin_c.b * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].begin_c.b = static_cast<sf::Uint8>(temp);
    temp = static_cast<unsigned int>(s_col_palet[order].end_c.b * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].end_c.b = static_cast<sf::Uint8>(temp);
    
    temp = static_cast<unsigned int>(s_col_palet[order].begin_c.r * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].begin_c.r = static_cast<sf::Uint8>(temp);
    temp = static_cast<unsigned int>(s_col_palet[order].end_c.r * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].end_c.r = static_cast<sf::Uint8>(temp);

    temp = static_cast<unsigned int>(s_col_palet[order].begin_c.g * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].begin_c.g = static_cast<sf::Uint8>(temp);
    temp = static_cast<unsigned int>(s_col_palet[order].end_c.g * change_ratio);
    if (temp > 255) {temp=255;}
    s_col_palet[order].end_c.g = static_cast<sf::Uint8>(temp);
  }
}
  

// Calculate modified pallete used for flash effect 
// considering current light color
void ColorPal::calc_flash_color_pallet(sf::Color act_light_color) {
  // *2.5 +70(/256)
  const float FLASH_COLOR_MULTI = 2.5;
  const unsigned int FLASH_COLOR_PLUS = 70; 

  // First Base Color
  const sf::Uint32 BASE_COLOR_MASK = 0x00FF; // First base color is actually FF00
  // Go to Next Color
  const unsigned int NEXT_COLOR_SHIFT = 8;

  unsigned int color_start_begin, color_begin;
  unsigned int color_start_end, color_end;

  for (size_t order {0}; order <= cFrac::NrOfOrders; ++order) { 

    // Initialize flash colors acoording to current pallete
    s_flash_col_palet[order].begin_c = s_col_palet[order].begin_c;
    s_flash_col_palet[order].end_c = s_col_palet[order].end_c;

    // Base colors iteration
    sf::Uint32 col_mask = BASE_COLOR_MASK;
    unsigned int color_shift = 0;
    for (size_t base_col {0}; base_col < 3; ++base_col) {

      col_mask <<= NEXT_COLOR_SHIFT;
      color_shift += NEXT_COLOR_SHIFT;

      // derive given color component strength from main Light
      unsigned int light_color = (act_light_color.toInteger() & col_mask) >> color_shift;
      // Dbg::report_trace("Main light base color: ", base_col);
      // Dbg::report_trace("Main light color strength: ", t_light_color);
      assert(light_color <= 0xFF); // range of component
      
      // derive the same base color from palette
      color_start_begin = (s_col_palet[order].begin_c.toInteger() & col_mask) >> color_shift;
      color_begin = color_start_begin;

      // Reinforce matching light formula
      color_begin *= static_cast<unsigned int>(FLASH_COLOR_MULTI * light_color) / cRGB_ColorMax;
      if (color_begin < color_start_begin) {
        // Do NOT diminish color
        color_begin = color_start_begin;
      }
      color_begin += (FLASH_COLOR_PLUS * light_color) / cRGB_ColorMax;
      if (color_begin > cRGB_ColorMax) {
        color_begin = cRGB_ColorMax;
      }
      // Dbg::report_trace("Begin flash component: ", color_begin);
  
      color_start_end = (s_col_palet[order].end_c.toInteger() & col_mask) >> color_shift;
      // color_start_end  = col_palet[order].end_c.r;
      color_end = color_start_end;
      color_end *= static_cast<unsigned int>(FLASH_COLOR_MULTI * light_color) / cRGB_ColorMax;
      if (color_end < color_start_end) {
        // Do NOT diminish color
        color_end = color_start_end;
      }
      color_end += (FLASH_COLOR_PLUS * light_color) / cRGB_ColorMax;
      if (color_end > cRGB_ColorMax) {
        color_end = cRGB_ColorMax;
      }
      // Dbg::report_trace("End flash component: ", color_end);
      switch (base_col) {
        case 0: s_flash_col_palet[order].begin_c.b = color_begin;
                s_flash_col_palet[order].end_c.b = color_end;
                break;
        case 1: s_flash_col_palet[order].begin_c.g = color_begin;
                s_flash_col_palet[order].end_c.g = color_end;
                break;
        case 2: s_flash_col_palet[order].begin_c.r = color_begin;
                s_flash_col_palet[order].end_c.r = color_end;
                break;
        default: Dbg::report_error("Wrong color base in flash pallete algo: ", base_col);
          assert(false and "Unexpected state"); // always assert
      }
    }

  }
}

// change or rotate palette from keyboard
bool ColorPal::key_decodation(sf::Keyboard::Key key) {
  // Precalculated Colors Palletes rotation
  if (key == sf::Keyboard::K) {
    rotate_fix_color_pallet();
    // Prepare flash (flare-up) color pallete based on current one
    calc_flash_color_pallet(LightS::s_lightColor);
    return true; // key found
  }
  if (key == sf::Keyboard::I) {
    generate_rnd_color_pallet();
    // Prepare flash (flare-up) color pallete based on current one
    calc_flash_color_pallet(LightS::s_lightColor);
    return true; // key found
  }
  if (key == sf::Keyboard::J) {
    // Dimm colors
    calc_updown_color_pallet(false);
    // update flash color pallet
    calc_flash_color_pallet(LightS::s_lightColor);
    return true; // key found
  }
  if (key == sf::Keyboard::U) {
    // Reinforce colors
    calc_updown_color_pallet(true);
    // update flash color pallet
    calc_flash_color_pallet(LightS::s_lightColor);
    return true; // key found
  }
  if (key == sf::Keyboard::H) {
    // Switch on global flash flag; 
    // it will be off at one_step_cfg_change (after single frame)
    s_global_flash = true;
    return true; // key found
  }

  // Not my key
  return false; 
}


bool ColorPal::demoGenerator(long int demoCnt, bool resetAction) {

  static long int lastActionDistance { 0 };
  
  if (demoCnt == cFrac::DemoInitCnt) {
    // none
  } else if (resetAction) {
    lastActionDistance = 0;
  }
  
  // Active event occured from this generator
  bool actionDone = false;
  ++lastActionDistance;

  if (demoRand(270, lastActionDistance))  {
    generate_rnd_color_pallet();
    // Dimm colors by one step for better contrast of flash effect
    calc_updown_color_pallet(false);
    // Prepare flash (flare-up) color pallete based on current one
    calc_flash_color_pallet(LightS::s_lightColor);
    actionDone = true;
  }

  return actionDone;
}
