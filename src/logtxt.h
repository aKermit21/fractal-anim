// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "colors.h"
#include "fractal.h"
#include "opt_lyra.h"
#include "text_draw.h"
#include "cfg_toml.h"
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
    : file_opened { false } 
    , cHelpDrawFrames {35}
    , cPrintSpeedFrames {20}
    , cSavedDrawFrames {20}
    , snapshot_file_str { opts.optSnapshot }    
    , loaded_snapshot_info_str {}
    , help_draw_cnt { 0 }
    , speed_scale_draw_cnt { 0 }
    , m_saved_draw_cnt {0}
    , m_snapshot_info_active { false }
    , log_subdir_state { sDNotChecked }
  {
    Dbg::report_info("Init: LogText (speed=) ", opts.optSpeed);
  } 

  virtual ~LogText() { log_close(); }

  // log/snapshot subdirectory
  enum SubDirState {sDNotChecked, sDExists, sDNotExists};

  // Font and text display related functions
  TextDraw textDraw;

  // Toml stored snapshots configurations
  CfgToml cfgToml;

  // Log snapshot to file with timestamp and provided data
  void log_snapshot(std::string data_str);
   
  // retrieve (next) snapshot from file
  void load_next_snapshot(T_Algo_Arr & transf_arr, T_Col_Palet & col_palet);
  
  // Set enabling counter
  void startHelpDraw(void);
  void startSpeedDraw(void);
  // after loading snapshot with F3
  void startSnapshotDraw(void);
  void stopSnapshotDraw(void);
  // after saving snapshot with F2
  void startSavedDraw(void);
  
  // Conditional draws
  void help_draw(sf::RenderWindow & win); 
  void speed_draw(sf::RenderWindow & win, int speed);
  void snapshot_draw(sf::RenderWindow & win); 
  void saved_draw(sf::RenderWindow & win); 
  
  // Dispatch draw
  void welcome_draw(sf::RenderWindow & win, int speed) const; 
  void rescale_draw(sf::RenderWindow & win, float scale) const; 
  
private:
  std::string search_file_path(void);

  bool file_opened;

  const int cHelpDrawFrames;
  const int cPrintSpeedFrames;
  const int cSavedDrawFrames;

  const std::string snapshot_file_str;
  std::string loaded_snapshot_info_str;

  void log_close(void) noexcept;

  int help_draw_cnt;
  // To draw for several frames Speed after its change
  int speed_scale_draw_cnt;
  // To draw 'saved' confirmation
  int m_saved_draw_cnt;
  bool m_snapshot_info_active;
  
  SubDirState log_subdir_state;
  std::ofstream fout;
};
