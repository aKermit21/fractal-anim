// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "cfg_toml.h"
// #include "toml++/impl/forward_declarations.hpp"
#include "colors.h"
#include "dbg_report.h"
#include "fractal.h"
#include "toml++/impl/node.hpp"
#include "toml++/impl/parse_error.hpp"
#include "toml++/impl/parser.hpp"
#include "toml++/toml.hpp"
#include "transform.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>



// Inteface - common error handling, return string to display info
std::string CfgToml::loadNextConfig(std::string filePath,
                             T_Algo_Arr & transform_algo, T_Col_Palet & colors) {
  std::string info {};
  auto success = loadNextConfigInternal(filePath, info, transform_algo, colors);
  if (!success) {
    if (m_fileconfigState != cFileDoesNotExist) {
      Dbg::report_warning("Error parsing config file " + filePath +
                          "\n          correct file or delete it!");
      
      m_fileconfigState = cFileConfigCorrupted; 
    }
  }
  return info;
}


// next config from loaded file
bool CfgToml::loadNextConfigInternal(std::string filePath, std::string & info,
                             T_Algo_Arr & tranform_algo, T_Col_Palet & colors) {

 
  if (m_fileconfigState != cFileConfigLoaded) {
  
    // Try to load configs from file
    auto success = loadTomlConfig(filePath);
    if (!success) {
      if (m_fileconfigState != cFileDoesNotExist) {
        Dbg::report_warning("Error parsing config file " + filePath);
        m_fileconfigState = cFileConfigCorrupted; 
      }
      return false;
    }
  }

  assert(m_configMaxNumber > -1);

  // m_fileconfigState = cFileConfigCorrupted; // after succesfull parsing change this
   
  toml::table * table_ptr = allConfig["config"][m_currentConfig].as_table();
  if (!table_ptr) return false; // error
  
  auto thisConfig = *table_ptr;
  std::stringstream ss;
  ss << thisConfig << '\n';
  Dbg::report_trace(ss.str());

  T_Algo_Arr tmp_tran_algo;
  T_Col_Palet tmp_colors;
  
  // std::cout << thisConfig.at_path("colors.level[0]").type() << '\n';
  // std::cout << thisConfig.at_path("colors.level[0].begin").type() << '\n';
  // std::cout << thisConfig["colors"]["level"][0]["begin"]["red"].type() << '\n';
  // std::cout << thisConfig["colors"]["level"][0]["begin"]["red"].is_integer() << '\n';

  // Retrieving transform part
  // 
  // Reposition reading
  for (size_t i=0; i < cFrac::NrOfElements; ++i) {

    if (!thisConfig["transform"]["element"][i]["reposition"].is_integer()) return false;
    std::optional<int> tmpint = thisConfig["transform"]["element"][i]
      ["reposition"].value<int>();
    if (!tmpint) return false;
    tmp_tran_algo[i].repos = *tmpint;
  
    // Angle reading
    if (!thisConfig["transform"]["element"][i]["angle"].is_integer()) return false;
    tmpint = thisConfig["transform"]["element"][i]["angle"].value<int>();
    if (!tmpint) return false;

    // See log_trans_config() and conv_to_assym() transformation
    tmp_tran_algo[i].angle = *tmpint * cTran::accurAngleMltp / 10L; 
    tmp_tran_algo[i].angle_down = -tmp_tran_algo[i].angle;
  
    // Scale reading
    if (!thisConfig["transform"]["element"][i]["scale"].is_floating_point()) return false;
    std::optional<float> tmpfloat = thisConfig["transform"]["element"][i]
      ["scale"].value<float>();
    if (!tmpfloat) return false;
    tmp_tran_algo[i].scale = *tmpfloat;

    std::stringstream ss;
    ss << "repos " << tmp_tran_algo[i].repos << "; "
       << "angle " << tmp_tran_algo[i].angle << "; "
       << "angle-down " << tmp_tran_algo[i].angle_down << "; "
       << "scale " << tmp_tran_algo[i].scale; 
    Dbg::report_trace(ss.str());
  }

  // Copy this to live config/algo
  tranform_algo = tmp_tran_algo;
  
  // Retrieving Color part
  // 
  for (size_t i=0; i < cFrac::NrOfOrders+1; ++i) {
    // Begin stem color
    // Red
    if(!thisConfig["colors"]["level"][i]["begin"]["red"].is_integer()) return false;
    std::optional<int> tmpint = thisConfig["colors"]["level"][i]["begin"]["red"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].begin_c.r = *tmpint;
  
    // Green
    if(!thisConfig["colors"]["level"][i]["begin"]["green"].is_integer()) return false;
    tmpint = thisConfig["colors"]["level"][i]["begin"]["green"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].begin_c.g = *tmpint;
   
    // Blue
    if(!thisConfig["colors"]["level"][i]["begin"]["blue"].is_integer()) return false;
    tmpint = thisConfig["colors"]["level"][i]["begin"]["blue"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].begin_c.b = *tmpint;
  
    // End stem color
    // Red
    if(!thisConfig["colors"]["level"][i]["end"]["red"].is_integer()) return false;
    tmpint = thisConfig["colors"]["level"][i]["end"]["red"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].end_c.r = *tmpint;
  
    // Green
    if(!thisConfig["colors"]["level"][i]["end"]["green"].is_integer()) return false;
    tmpint = thisConfig["colors"]["level"][i]["end"]["green"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].end_c.g = *tmpint;
   
    // Blue
    if(!thisConfig["colors"]["level"][i]["end"]["blue"].is_integer()) return false;
    tmpint = thisConfig["colors"]["level"][i]["end"]["blue"].value<int>();
    if (!tmpint) return false;
    tmp_colors[i].end_c.b = *tmpint;
  }

  // Copy this to live color palete
  colors = tmp_colors;

  // Prepare info text
  bool description_success = false;
  std::stringstream ss_info {};
  std::optional<std::string> tmpstr;
  ss_info << '[' << m_configMaxNumber-m_currentConfig+1 << '/'
          << m_configMaxNumber+1 << "] ";
  if (thisConfig["description"].is_string()) {
    tmpstr = thisConfig["description"].value<std::string>();
    if (tmpstr) {
      if (!(*tmpstr).empty()) {
        ss_info << *tmpstr;
        description_success = true;
      }
    }
  }
  if (!description_success) {
    if (thisConfig["time-date"].is_string()) {
      tmpstr = thisConfig["time-date"].value<std::string>();
      if (tmpstr) {
        if (!(*tmpstr).empty()) {
          ss_info << *tmpstr;
        } else {
          Dbg::report_warning("neither description nor time-date field can be displayed");
        }
      }
    } else {
      Dbg::report_warning("neither description nor time-date field can be displayed");
    }
  }
  info = ss_info.str();
  Dbg::report_info(info);
  
  // Next time config
  if (m_currentConfig == 0) {
    m_currentConfig = m_configMaxNumber;
  } else {
    --m_currentConfig;
  }

  m_fileconfigState = cFileConfigLoaded; 
  return true; // success
}

  
// Parsing and loading Toml snapshot/config
bool CfgToml::loadTomlConfig(std::string filePath){
  Dbg::report_info("Loading config file " + filePath);

  // Check if cfg (snapshot) file exists in given directory
  std::filesystem::path logFile{ filePath };
  if (!exists(logFile)) {
    m_fileconfigState = cFileDoesNotExist;
    Dbg::report_warning("File - " + filePath + " - Do NOT exists.");
    return false; // error
  }

  toml::array allConfigArr;
  
  try {
    allConfig = toml::parse_file(filePath);
    auto ptr = allConfig.get_as<toml::array>("config");
    // toml::array* ptr = allConfig.get_as<toml::array>("config");
    // std::cout << "ptr: " << ptr << '\n';
    if (!ptr) return false;
    allConfigArr = *ptr;
    Dbg::report_info("Loaded configurations: ", allConfigArr.size());
  }
  catch (const toml::parse_error & err){
    std::stringstream ss;
    ss << "Error parsing config file '" << *err.source().path
       << "':\n" << err.description()
       << "\n (" << err.source().begin << ")\n"
       << "  Correct config or delete/restore the file";
    Dbg::report_warning(ss.str());
    m_fileconfigState = cFileConfigCorrupted;
    return false; // error
  }

  // Shall be non-empty array
  if (!allConfigArr.size()) return false; 
  m_configMaxNumber = allConfigArr.size() -1;

  // start from last/latest config
  m_currentConfig = m_configMaxNumber;

  m_fileconfigState = cFileConfigLoaded;
  return true; // success
}
 
