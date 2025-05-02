// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "logtxt.h"
#include "dbg_report.h"
#include "fractal.h"
#include "text_draw.h"
#include "transform.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <string_view>
#include <time.h>


// Request for drawing Help per # of frames
void LogText::startHelpDraw(void) {
  help_draw_cnt = cHelpDrawFrames;
}

// Request for drawing Speed per # of frames
void LogText::startSpeedDraw(void) {
  speed_scale_draw_cnt = cPrintSpeedFrames; 
}


// Draw Help if requested (counter per frame)
void LogText::help_draw(sf::RenderWindow & win) {
  if (help_draw_cnt > 0) {
    textDraw.help_draw(win);
    --help_draw_cnt;
  }
}

// Draw Speed if requested (counter per frame)
void LogText::speed_draw(sf::RenderWindow & win, int speed) {
  if (speed_scale_draw_cnt > 0) {
    --speed_scale_draw_cnt;
    textDraw.speed_draw(win, speed);
  }
}

// Welcome 
void LogText::welcome_draw(sf::RenderWindow & win, int speed) const {
  textDraw.welcome_draw(win, speed);
}

void LogText::rescale_draw(sf::RenderWindow & win, float scale) const {
  textDraw.rescale_draw(win, scale);
} 

void LogText::log_snapshot(std::string sData) {
  // Assuming command run from build* subdirectory
  const std::string subdir_str { "../log/" };

  const std::filesystem::directory_entry log_path{subdir_str};

  static std::string filepath;

  // Check if subdirectory exists
  if (sDNotChecked == log_subdir_state) {
    // Check for subdirectory
    if (log_path.exists()) {
      log_subdir_state = sDExists;
      filepath = subdir_str + snapshot_file_str;
    } else {
      log_subdir_state = sDNotExists;
      filepath = snapshot_file_str;
    }
  }

  // generate time stamp
  std::time_t time = std::time({});
  char timeString[100]; // maximum expected date/time string size
  std::strftime(std::data(timeString), std::size(timeString),
                "%A %e-%b-%y  %T", std::localtime(&time));
  
  // Append log file
  if (!file_opened) {
    fout.open(filepath, std::ios_base::app);
  }
  if (fout.is_open()) {
    file_opened = true;
    // print log title then time stamp
    fout << "Snapshot taken - " << timeString << '\n';

    // Takes snpashot data from base classes
    fout << sData;

    log_close();
  } else {
    Dbg::report_warning("Log file cannot be opened", 0);
  }
};

void LogText::log_close() noexcept {
  if (file_opened) {
    fout << std::endl; // flash data
    fout.close();
    file_opened = false;
  }
}

