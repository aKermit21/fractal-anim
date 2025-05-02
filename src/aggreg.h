// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "autoscale.h"
#include "colors.h"
#include "dbg_report.h"
#include "light.h"
#include "logtxt.h"
#include "opt_lyra.h"
#include "windy.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

// Main program aggregate (collection of) structs
// containing MovWind/MovAnim(animation), Color Palette,
// and Light Source

struct MainProgAggr {
  explicit MainProgAggr(OptParams opts)
      : logtxt{opts}
      , movWind{opts.optSpeed}
      , m_demoActive{opts.optDemo} 
  {
    Dbg::report_info("Init: MainProgAggr (demo=) ", opts.optDemo);
  }

  // General key decodation
  // can be dispatched to subordinate classes/structs
  void key_decodation(const sf::Keyboard::Key key);
  
  // (Re)Draw some possible artefacts on top of fractal structure
  void draw_artefacts(sf::RenderWindow & win, AutoScale & rescale);

  // Post Construction (very Initialization) Init and sync
  void postInitSync(void);

  // One step per display loop - to be disapthed to subordinate classes/struct
  void one_step_cfg_change();

  // current transformation data (array) and Colors
  std::string prepareSnapshotData(void);
  
  // Single demo step
  void demoGenerator(Element &primEl, AutoScale &autoScale);

  // aggregate of Structs/classes
  LogText logtxt;     // Text, Logging snapshots
  MovWind movWind;   // animation: open, close, wind
  ColorPal colorPal; // color palette
  LightS lightS;     // light source

protected:
  
  void resetConfig(bool keyAction);

  const bool m_demoActive;
};
