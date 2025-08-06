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

#ifndef SHADER_GEN_AI_AGENT_FITNESS_PLOTTER_HPP
#define SHADER_GEN_AI_AGENT_FITNESS_PLOTTER_HPP

#include <QWidget>
#include <QDateTime>
#include <vector>

class QTimer;

class FitnessPlotterWidget : public QWidget {
    Q_OBJECT
public:
    explicit FitnessPlotterWidget(QWidget* parent = nullptr);
    ~FitnessPlotterWidget();

    void add_point(unsigned long fitness);
    void start_timer();
    void stop_timer();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QTimer* timer;
    std::vector<std::pair<QDateTime, unsigned long>> points;
};

#endif  // SHADER_GEN_AI_AGENT_FITNESS_PLOTTER_HPP
