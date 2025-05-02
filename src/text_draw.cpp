// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "text_draw.h"
#include "dbg_report.h"
#include "fractal.h"
#include "config.h"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <type_traits>
#include <assert.h>

bool TextDraw::init_font() {
  const std::string font_file_str = "text_fonts.ttf";
  const std::string home_dir = getenv("HOME");
  // Possible location of fonts file
  constexpr size_t cSubdirsNum { 4 };
  const std::string subdirs_str[cSubdirsNum] = {
    "fonts/", "../fonts/",
    // linux standard user installation of data by meson --prefix $HOME/.local
    home_dir +  "/.local/share/" + PROJECT_STR "/", 
    // linux standard system installation of data by meson
    "/usr/local/share/"  PROJECT_STR  "/" }; 

  std::string dirpath_str;
  std::string filepath_str;
  size_t dir_index = 0;
  do {
    dirpath_str = subdirs_str[dir_index];
    filepath_str = dirpath_str + font_file_str;
    std::filesystem::path font_file{ filepath_str };
    // Check if font file exists in given directory
    if (exists(font_file)) {
      font_subdir_state = sDExists;
      Dbg::report_info("Font file found: " + filepath_str + "   ", 0);
    } else {
      font_subdir_state = sDNotExists;
      Dbg::report_info("Font file Not found: " + filepath_str + "    ", 0);
      // if (sub)directory not found, try to load from current directory
      filepath_str = font_file_str;
    }
    dir_index++;
  } while ((sDNotExists == font_subdir_state) and (dir_index <= cSubdirsNum));
  
  assert(font_subdir_state != sDNotChecked);
  assert(dir_index <= cSubdirsNum);
  
  // Load fonts from a file
  if (!m_font.loadFromFile(filepath_str)) {
    Dbg::report_warning("Font file "+ font_file_str +" cannot be find/open ", 0);
    return false;
  } else {
    return true; // fonts loaded
  }
}

void TextDraw::help_draw(sf::RenderWindow &win) const {
  const static std::string help_text { "<F1> - Help \n"
    " X - EXit\n<Space> - Stop and Log\n"
    " R - Reset\n\n"
    " PageUp - Speed Up\n"
    " PageDown - Speed Down\n\n"
    " L - Light on/off toggle\n"
    " W - Light color rotation\n"
    " S - rotation reversed\n"
    " A - left smooth movement\n"
    " D - right smooth movement\n"
    " Q - left jump movement\n"
    " D - right jump movement\n"
    " G - Switch Grid of rays visualisation\n\n"
    " K - Rotate Kolor Palette pre-calculated\n"
    " I - Generate random Color Palette\n"
    " J - Dimm all colors (dJimm)\n"
    " U - Up all colors\n"
    " H - Global flash of active color\n\n"
    " ~   Animation: Wind (wobbling)\n"
    " O   Animation: Opening\n"
    " C - Animation: Closing\n\n"
    " P - Rotate Pre-calculated configurations" };
  
  if (m_font_loaded) {
    sf::Text text(help_text, m_font);
    text.setCharacterSize(25);
    text.setStyle(sf::Text::Regular);
    text.setFillColor(sf::Color::White);
    text.setPosition(20,10);
    // Draw it
    win.draw(text);
  }
}

void TextDraw::welcome_draw(sf::RenderWindow &win, int speed) const {
  if (m_font_loaded) {
    std::stringstream text_ss;;
    text_ss << "Initialization...(wait)\n";
    text_ss << "F1 for help\n";
    text_ss << "Speed scale - " << speed << '\n';
    sf::Text text(text_ss.str(), m_font);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Regular);
    text.setFillColor(sf::Color::White);
    text.setPosition(30,30);
    // Draw it
    win.draw(text);
  }
}

void TextDraw::speed_draw(sf::RenderWindow &win, int speed) const {
  if (m_font_loaded) {
    std::stringstream text_ss;;
    text_ss << "Speed scale - " << speed;
    sf::Text text(text_ss.str(), m_font);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Regular);
    text.setFillColor(sf::Color::White);
    text.setPosition(30,30);
    // Draw it
    win.draw(text);
  }
}

void TextDraw::rescale_draw(sf::RenderWindow & win, float scale) const {
  constexpr static int cFontSize { 20 };
  if (m_font_loaded) {
    // Convert to percentage
    int iScale = static_cast<int>(scale * 100);
    std::stringstream text_ss;;
    text_ss << "Auto-Rescale active [" << iScale << "%]";
    sf::Text text(text_ss.str(), m_font, cFontSize);
    text.setStyle(sf::Text::Regular);
    text.setFillColor(sf::Color::Magenta);
    // Put text at the bottom of window
    text.setPosition(10, cFrac::WindowYsize - cFontSize *2);
    // Draw it
    win.draw(text);
  }
}
