// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <SFML/Graphics.hpp>
#include "dbg_report.h"
#include "transform.h"

// Drawing Texts on top of Picture
struct TextDraw {
  TextDraw()
    : font_subdir_state { sDNotChecked }
    , m_font_loaded { false }
  {
    Dbg::report_info("Init: TextDraw ", 0);
    m_font_loaded = init_font();
  }

  virtual ~TextDraw() { 
    // close font file ?
  }

  // Real draw
  void help_draw(sf::RenderWindow & win) const; 
  void welcome_draw(sf::RenderWindow & win, int speed) const; 
  void speed_draw(sf::RenderWindow & win, int speed) const; 
  void rescale_draw(sf::RenderWindow & win, float scale) const; 

private:
  // font subdirectory
  enum SubDirState {sDNotChecked, sDExists, sDNotExists};
  SubDirState font_subdir_state;
  bool m_font_loaded;
  sf::Font m_font;

  bool init_font(void);
};
