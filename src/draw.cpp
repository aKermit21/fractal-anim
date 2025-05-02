// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "fractal.h"
#include "dbg_report.h"
#include "light.h"
#include "transform.h"
#include "colors.h"
#include "animation.h"
#include "assert.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

void StemFlash::draw_stem(sf::RenderWindow &win, short order, const bool freezeTime) {

  // effect lasting # of frames
  constexpr static unsigned int FLASH_GLOBAL_CNT_MAX { 5 };
  //flash efect from 90deg light rays
  constexpr static unsigned int FLASH_LIGHT_CNT_MAX { 10 };

  // 90 deg transition vec / light vec
  if (light_vec_angle_flip()) {
    // show local (this stem) flash
    flash_cnt = FLASH_LIGHT_CNT_MAX;
  }
  
  // Further comtrol of flash effect 
  if (ColorPal::s_global_flash) {
    // Flash with all stems
    flash_cnt = FLASH_GLOBAL_CNT_MAX;
  } else if (ColorPal::s_reset_flash_algo) {
    prev_l_angle = lAngleUnknown;
    flash_cnt = 0;
  // Time freeze option (stop type)
  } else if ((this->flash_cnt > 0) and (!freezeTime)) {
    // keep flash effect for some time
    --flash_cnt;
  } else { }
  
  assert(order >= 0);
  
  if (order <= 2) {
    if (x1==0 or x2==0 or y1==0 or y2==0) {
      Dbg::report_warning(" Suspected (0) stem data coordinate(s), possible not initialized ", x1);
    } else {
      // Scale down for drawing
      float fx1 = x1/cTran::AccurMltp_f;
      float fy1 = y1/cTran::AccurMltp_f;
      float fx2 = x2/cTran::AccurMltp_f;
      float fy2 = y2/cTran::AccurMltp_f;
      float fvdx = (vec_xy.x + vec_xy.dx)/cTran::AccurMltp_f;
      float fvdy = (vec_xy.y + vec_xy.dy)/cTran::AccurMltp_f;

      if (flash_cnt > 0 and LightS::s_lightActive) {
        // Draw Flash version

        // Filled triangles
        sf::VertexArray triangle(sf::Triangles, 3);
        triangle[0].position = sf::Vector2f(fx1,fy1);
        triangle[1].position = sf::Vector2f(fvdx, fvdy);
        triangle[2].position = sf::Vector2f(fx2,fy2);

        // Flash colors
        triangle[0].color = ColorPal::s_flash_col_palet[order].begin_c;
        triangle[2].color = ColorPal::s_flash_col_palet[order].begin_c;
        triangle[1].color = ColorPal::s_flash_col_palet[order].end_c;
        
        win.draw(triangle);
      } else {
        // Draw ordinary version

        // Empty triangles
        sf::VertexArray two_lines(sf::Lines, 4);
        two_lines[0].position = sf::Vector2f(fx1,fy1);
        two_lines[1].position = sf::Vector2f(fvdx, fvdy);
        two_lines[2].position = sf::Vector2f(fx2,fy2);
        two_lines[3].position = sf::Vector2f(fvdx, fvdy);
        // Regular colors
        two_lines[0].color = ColorPal::s_col_palet[order].begin_c;
        two_lines[2].color = ColorPal::s_col_palet[order].begin_c;
        two_lines[1].color = ColorPal::s_col_palet[order].end_c;
        two_lines[3].color = ColorPal::s_col_palet[order].end_c;

        win.draw(two_lines);
      }

    }
    
  } else { // order > 2
    float fvx = vec_xy.x/cTran::AccurMltp_f;
    float fvy = vec_xy.y/cTran::AccurMltp_f;
    float fvdx = (vec_xy.x + vec_xy.dx)/cTran::AccurMltp_f;
    float fvdy = (vec_xy.y + vec_xy.dy)/cTran::AccurMltp_f;

    if (flash_cnt > 0 and LightS::s_lightActive) {
      // Draw Flash version
      // Double/Triple line thickness
      sf::VertexArray tri_line(sf::Lines, 6);
      tri_line[0].position = sf::Vector2f(fvx,fvy);
      tri_line[1].position = sf::Vector2f(fvdx, fvdy);
      tri_line[2].position = sf::Vector2f(fvx +1,fvy);
      tri_line[3].position = sf::Vector2f(fvdx +1, fvdy);
      tri_line[4].position = sf::Vector2f(fvx,fvy +1);
      tri_line[5].position = sf::Vector2f(fvdx, fvdy +1);

      // Flash colors
      tri_line[0].color = ColorPal::s_flash_col_palet[order].begin_c;
      tri_line[1].color = ColorPal::s_flash_col_palet[order].end_c;
      tri_line[2].color = ColorPal::s_flash_col_palet[order].begin_c;
      tri_line[3].color = ColorPal::s_flash_col_palet[order].end_c;
      tri_line[4].color = ColorPal::s_flash_col_palet[order].begin_c;
      tri_line[5].color = ColorPal::s_flash_col_palet[order].end_c;

      win.draw(tri_line);
    } else {
      // Draw ordinary version
      // Single line
      sf::VertexArray one_line(sf::Lines, 2);
      one_line[0].position = sf::Vector2f(fvx,fvy);
      one_line[1].position = sf::Vector2f(fvdx, fvdy);
      // Regular colors
      one_line[0].color = ColorPal::s_col_palet[order].begin_c;
      one_line[1].color = ColorPal::s_col_palet[order].end_c;

      win.draw(one_line);
    }
  }

}

// Old draw - without light flash
[[deprecated("use StemFlash::draw_step instead")]] 
void Stem::draw_stem(sf::RenderWindow &win, short order,
                     [[maybe_unused]]const bool freezeTime) {
  
  assert(order >= 0);
  
  if (order <= 2) {
    if (x1==0 or x2==0 or y1==0 or y2==0) {
      Dbg::report_warning(" Suspected (0) stem data coordinate(s), possible not initialized ", x1);
    } else {
      // Scale down for drawing
      float fx1 = x1/cTran::AccurMltp_f;
      float fy1 = y1/cTran::AccurMltp_f;
      float fx2 = x2/cTran::AccurMltp_f;
      float fy2 = y2/cTran::AccurMltp_f;
      float fvdx = (vec_xy.x + vec_xy.dx)/cTran::AccurMltp_f;
      float fvdy = (vec_xy.y + vec_xy.dy)/cTran::AccurMltp_f;
      sf::VertexArray two_lines(sf::Lines, 4);
      two_lines[0].position = sf::Vector2f(fx1,fy1);
      two_lines[1].position = sf::Vector2f(fvdx, fvdy);
      two_lines[2].position = sf::Vector2f(fx2,fy2);
      two_lines[3].position = sf::Vector2f(fvdx, fvdy);

      two_lines[0].color = ColorPal::s_col_palet[order].begin_c;
      two_lines[2].color = ColorPal::s_col_palet[order].begin_c;
      two_lines[1].color = ColorPal::s_col_palet[order].end_c;
      two_lines[3].color = ColorPal::s_col_palet[order].end_c;
      win.draw(two_lines);
    }
    
  } else { // order > 2
    float fvx = vec_xy.x/cTran::AccurMltp_f;
    float fvy = vec_xy.y/cTran::AccurMltp_f;
    float fvdx = (vec_xy.x + vec_xy.dx)/cTran::AccurMltp_f;
    float fvdy = (vec_xy.y + vec_xy.dy)/cTran::AccurMltp_f;
    sf::VertexArray one_line(sf::Lines, 2);
    one_line[0].position = sf::Vector2f(fvx,fvy);
    one_line[1].position = sf::Vector2f(fvdx, fvdy);
    one_line[0].color = ColorPal::s_col_palet[order].begin_c;
    one_line[1].color = ColorPal::s_col_palet[order].end_c;
    win.draw(one_line);
  }
}
