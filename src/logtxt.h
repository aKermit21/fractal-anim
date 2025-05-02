// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "fractal.h"
#include "opt_lyra.h"
#include "text_draw.h"
#include "windy.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <filesystem>
#include <string_view>

// Logfile and Text Printing
// including help printing with F1
struct LogText {
  explicit LogText(OptParams opts) 
    : cHelpDrawFrames{35}
    , cPrintSpeedFrames{20}
    , snapshot_file_str { opts.optSnapshot }    
    , help_draw_cnt { 0 }
    , speed_scale_draw_cnt { 0 }
    , log_subdir_state { sDNotChecked }
    , file_opened { false } 
  {
    Dbg::report_info("Init: LogText (speed=) ", opts.optSpeed);
  } 

  virtual ~LogText() { log_close(); }

  // log subdirectory
  enum SubDirState {sDNotChecked, sDExists, sDNotExists};

  // Font related functions
  TextDraw textDraw;

  // Log snapshot to file with timestamp and provided data
  void log_snapshot(std::string str);

  void startHelpDraw(void);
  void startSpeedDraw(void);
  
  // Conditional draws
  void help_draw(sf::RenderWindow & win); 
  void speed_draw(sf::RenderWindow & win, int speed);
  
  // Dispatch draw
  void welcome_draw(sf::RenderWindow & win, int speed) const; 
  void rescale_draw(sf::RenderWindow & win, float scale) const; 
  

private:
  const int cHelpDrawFrames;
  const int cPrintSpeedFrames;
  const std::string snapshot_file_str;

  void log_close(void) noexcept;

  int help_draw_cnt;
  // To draw for several frames Speed after its change
  int speed_scale_draw_cnt;
  
  SubDirState log_subdir_state;
  bool file_opened;
  std::ofstream fout;
};
