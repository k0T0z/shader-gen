/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2024 Seif Kandil (k0T0z)                                       */
/*                                                                               */
/*  This file is a part of the ENIGMA Development Environment.                   */
/*                                                                               */
/*                                                                               */
/*  ENIGMA is free software: you can redistribute it and/or modify it under the  */
/*  terms of the GNU General Public License as published by the Free Software    */
/*  Foundation, version 3 of the license or any later version.                   */
/*                                                                               */
/*  This application and its source code is distributed AS-IS, WITHOUT ANY       */
/*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/*  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        */
/*  details.                                                                     */
/*                                                                               */
/*  You should have recieved a copy of the GNU General Public License along      */
/*  with this code. If not, see <http://www.gnu.org/licenses/>                   */
/*                                                                               */
/*  ENIGMA is an environment designed to create games and other programs with a  */
/*  high-level, fully compilable language. Developers of ENIGMA or anything      */
/*  associated with ENIGMA are in no way responsible for its users or            */
/*  applications created by its users, or damages caused by the environment      */
/*  or programs made in the environment.                                         */
/*                                                                               */
/*********************************************************************************/

#include "gui/controller/fitness_plotter.hpp"

#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>
#include <limits>
#include <QPainterPath>

FitnessPlotterWidget::FitnessPlotterWidget(QWidget* parent) : QWidget(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]{ update(); });
}

FitnessPlotterWidget::~FitnessPlotterWidget() {
    delete timer;
}

void FitnessPlotterWidget::add_point(unsigned long fitness) {
    points.push_back(std::make_pair(QDateTime::currentDateTime(), fitness));
    update(); // Request an immediate repaint when a new point is added
}

void FitnessPlotterWidget::start_timer() {
    timer->start(100); // Refresh every 100ms
}

void FitnessPlotterWidget::stop_timer() {
    timer->stop();
}

void FitnessPlotterWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    if (points.size() < 1) {
        return; // Nothing to draw yet
    }

    // Find time and fitness ranges
    auto t_min_it = std::min_element(points.begin(), points.end(),
                                     [](const auto& a, const auto& b) { return a.first < b.first; });
    auto t_max_it = std::max_element(points.begin(), points.end(),
                                     [](const auto& a, const auto& b) { return a.first < b.first; });
    auto f_min_it = std::min_element(points.begin(), points.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; });
    auto f_max_it = std::max_element(points.begin(), points.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; });

    QDateTime t_min = t_min_it->first;
    QDateTime t_max = t_max_it->first;
    unsigned long f_min = f_min_it->second;
    unsigned long f_max = f_max_it->second;

    // Avoid division by zero
    if (t_min == t_max || f_min == f_max) {
        return;
    }

    // Define widget dimensions
    double width = this->width();
    double height = this->height();

    // Mapping functions
    auto map_x = [&](const QDateTime& t) {
        qint64 t_ms = t_min.msecsTo(t);
        qint64 total_ms = t_min.msecsTo(t_max);
        return (t_ms * width) / static_cast<double>(total_ms);
    };
    auto map_y = [&](unsigned long f) {
        // Invert y-axis: f_max at bottom (height), f_min at top (0)
        return height - ((f - f_min) * height) / static_cast<double>(f_max - f_min);
    };

    // Draw the graph
    painter.setPen(Qt::blue);
    QPainterPath path;
    auto it = points.begin();
    path.moveTo(map_x(it->first), map_y(it->second));
    ++it;
    for (; it != points.end(); ++it) {
        path.lineTo(map_x(it->first), map_y(it->second));
    }
    painter.drawPath(path);
}
