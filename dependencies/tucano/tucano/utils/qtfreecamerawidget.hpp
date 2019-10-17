/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __QTFREECAMERAWIDGET__
#define __QTFREECAMERAWIDGET__

#include "tucano/utils/qtflycamerawidget.hpp"
#include "tucano/utils/freecamera.hpp"

namespace Tucano
{

/**
 * @brief A widget with a free camera.
 * Extends QGLWidget class to include common methods for using Tucano
 * this widget already has a default camera and light trackball and associated mouse methods
 */
class QtFreecameraWidget : public QtFlycameraWidget
{
	
public:
	
	explicit QtFreecameraWidget( QWidget *parent, const QGLWidget* shareWidget = Q_NULLPTR )
	: QtFlycameraWidget( parent, shareWidget )
	{
		camera = new Freecamera();
	}
	
    void mousePressEvent (QMouseEvent * event) override
    {
        setFocus ();
        Eigen::Vector2f screen_pos (event->x(), event->y());
		if (event->button() == Qt::LeftButton)
		{
			camera->startRotation(screen_pos);
			camera->updateViewMatrix();
		}
		if (event->button() == Qt::MiddleButton)
		{
			camera->startRotation(screen_pos);
			camera->updateViewMatrix();
		}
		if (event->button() == Qt::RightButton)
		{
			light_trackball.rotateCamera(screen_pos);
		}
        updateGL ();
    }

    void mouseMoveEvent (QMouseEvent * event) override
    {
        Eigen::Vector2f screen_pos (event->x(), event->y());
		if (event->buttons() & Qt::LeftButton)
		{
			camera->rotate(screen_pos);
			camera->updateViewMatrix();
		}
		if(event->buttons() & Qt::MiddleButton)
		{
			camera->rotateZ(screen_pos);
			camera->updateViewMatrix();
		}
		if (event->buttons() & Qt::RightButton)
		{
			light_trackball.rotateCamera(screen_pos);
		}

        updateGL ();

    }

    /**
     * @brief Callback for mouse release event.
     *
     * Stops rotation or translation.
     * @param event The mouse event that triggered the callback.
     */
    virtual void mouseReleaseEvent (QMouseEvent * event)
    {
        if (event->button() == Qt::RightButton)
        {
            light_trackball.endRotation();
        }

        updateGL ();
    }
};

}
#endif