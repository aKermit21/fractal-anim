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
#include <string>
#include <iostream>
#include <chrono>

// Switch off/on Asserts manually if NOT set correctly by make/cmake
// #define NDEBUG

// Collects and reports debug info from every other class
struct Dbg
{
  Dbg() {  
    report_info("Init: Debug  ", 0);
    info_cnt = 0; error_cnt = 0; warning_cnt = 0; elements = 0;
    minmax.minX = cSizeBeyondMax; minmax.minY = cSizeBeyondMax;
    minmax.maxX = 0; minmax.maxY = 0;
    time_beg = Clock::now();
  }
  // explicit destructor always virtual: learncpp 25.4
  virtual ~Dbg(){ report_summary(); }

  // Disable copy/move contructor/assignment - The rule of five
  Dbg(const Dbg & dbg) = delete;              // copy
  Dbg(Dbg && dbg) = delete;                   // move
  Dbg & operator=(const Dbg & dbg) = delete;  // copy
  Dbg & operator=(Dbg && dbg) = delete;       // move
  
  // Min/Max drawing used for info and for AutoScale
  struct VecMinMax {
    int minX;
    int minY;
    int maxX;
    int maxY;
  };
  
  // Log Errors/Warning Options
  // write (optional) info when at run time
  constexpr static bool cReportError { true };
  #ifdef NDEBUG
  // Release make
  constexpr static bool cReportWarning { false };
  constexpr static bool cReportInfo { false }; // timing, info - non recursive
  #else
  // Development
  constexpr static bool cReportWarning { true };
  constexpr static bool cReportInfo { true }; // timing, info - non recursive
  #endif
  constexpr static bool cReportTrace { false }; // Debug, trace, possible recursive info

  // Start multiple warnings thresholds
  // Too much elements created
  constexpr static long cCreateWarningThreshold { 2'000'000 };
  // Too much pointers created
  constexpr static long cCreatePointersThreshold { 10'000'000 };
  // Too much elements drawed per cycle (frame)
  constexpr static long cDrawWarningThreshold { 5'000'000 };

  enum MultipleWarning { mltplElementsCreate, mltplElementsDraw, mltplPointers };

  static void count_elements(int i);
  static void demo_frames(long int i);
  static void report_error(std::string s, long int i);
  static void report_warning(std::string s, long int i);
  //special multiple_warning (x2 formula)
  static void report_mltpl_warning(MultipleWarning mwtype, long int i);
  static void report_info(std::string s, long int i);
  static void report_trace(std::string s, long int i);
  static void find_minmax(const VecMinMax minmaxVec);
  static void report_summary(void) noexcept;

  // Timing, Counting elements - per frame
  void before_recurrence();
  void after_recurrence();
  
  private:

  // Exit after # of errors
  constexpr static int exitAfterErrors { 10 };
  // Maximum expected fractal size in points
  // for calculating min/max
  constexpr static int cSizeBeyondMax { 10000 };

  // Counters
  static int error_cnt;
  static long int warning_cnt; 
  static long int info_cnt; 
  static long int elements;
  static long int m_demoCnt;
  
  // timer
  // using Clock = std::chrono::steady_clock;
  using Clock = std::chrono::high_resolution_clock;
  using Milisec = std::chrono::duration<double, std::milli>;

  static std::chrono::time_point<Clock> time_beg;
  static VecMinMax minmax;
};

