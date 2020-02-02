#ifndef HPGLINTERPRETER_H
#define HPGLINTERPRETER_H

#pragma once

#include "defines.h"
#include "matrix.h"

#include <vector>
#include <string>
#include <exception>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/container/flat_map.hpp>
#include <QRect>



class CPlotterDC;

class HPGLInterpreter
{
public:
    HPGLInterpreter();

    void setPlotterDC(CPlotterDC*aDC) { mDC = aDC; }
    void readFile(const QString& sFileName);
    void parseText(const QString& aText);

private:
    enum eNames { unknown=-1, default_instruction, initialize_instructon,
                  scaling_point, scale, input_window, rotate, page_output,
                  pen_up, pen_down, plot_absolute, pen_move_relative, absolute_arc_plot, relative_arc_plot, circle,
                  fill_type, line_type, pen_width, symbol_mode, select_pen, tick_length, x_tick, y_tick, pen_thickness,
                  edge_rectangle_absolute, edge_rectangle_relative, edge_wedge,
                  fill_absolute_rectangle, fill_relative_rectangle, fill_wedge,
                  label, select_standard_font, relative_character_size, absolute_character_size,
                  standard_font_definition, scalable_or_bitmap_fonts, select_alternate_font, alternate_font_definition
                };

    void parseHPGL(const std::string& aLine);

    size_t decodeNumbers(const std::string& aString,const std::string&aSeparator, std::vector<FLOAT>& aNumbers);
    typedef boost::container::flat_map< std::string, eNames>      name_map;

    CPlotterDC *mDC;
    boost::function< void (const std::string& ) > mParse;
    name_map mNameMap;
    bool  mPenDown;
    FLOAT mScaleX, mScaleY;
};

#endif // HPGLINTERPRETER_H
