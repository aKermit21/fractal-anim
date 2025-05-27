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
#include "colors.h"
#include "dbg_report.h"
// #include "toml++/impl/forward_declarations.hpp"
#include "fractal.h"
#include "toml++/toml.hpp"


// Parsing and loading Toml snapshot/config
struct CfgToml {
  CfgToml()
    : m_fileconfigState{ cFileNotChecked }
    , m_configMaxNumber{ 0 }
    , m_currentConfig{ 0 }
  {
    Dbg::report_info("Init: CfgToml");
  }

  virtual ~CfgToml() { 
    // close config file ?
  }

  // struct ConfigResult {
  //   bool success;
  //   std::string str;
  // };
  
  std::string loadNextConfig(std::string filePath,
                      T_Algo_Arr & transform_algo, T_Col_Palet & colors);
  
private:
  bool loadNextConfigInternal(std::string filePath, std::string & info,
                      T_Algo_Arr & transform_algo, T_Col_Palet & colors);

  bool loadTomlConfig(std::string filePath);
  
  // config toml file state
  enum FileConfigState {cFileNotChecked, cFileDoesNotExist,
                      cFileConfigCorrupted, cFileConfigLoaded};

  toml::table allConfig;
  
  FileConfigState m_fileconfigState;

  int m_configMaxNumber;
  int m_currentConfig;
};
