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

#ifndef __QTTRACKBALLWIDGET__
#define __QTTRACKBALLWIDGET__

#include <GL/glew.h>

#include "tucano/utils/objimporter.hpp"
#include "tucano/utils/plyimporter.hpp"

#include "tucano/tucano.hpp"
#include "tucano/utils/trackball.hpp"

#include <QGLWidget>
#include <QMouseEvent>
#include <QFileDialog>

using namespace std;

namespace Tucano
{

/**
 * @brief This class is just to make sure that GLEW is
 * initialized before anything else, so the constructor of
 * this class is called before the QtTrackballWidget
 * constructor.
 */
class QtGlewInitializer : public QGLWidget
{
public:
    QtGlewInitializer (QWidget* parent) : QGLWidget(parent)
    {
        makeCurrent();
        Misc::initGlew();
    }

};

/**
 * @brief A widget with a trackball iteration for a single mesh and a single light source.
 * Extends QGLWidget class to include common methods for using ShaderLib
 * this widget already has a default camera and light trackball and associated mouse methods
 */
class QtTrackballWidget : public QtGlewInitializer
{
    Q_OBJECT

protected:

    /// Triangle mesh.
    Mesh mesh;

    /// Trackball for manipulating the camera.
    Trackball camera;

    /// Trackball for manipulating the light position.
    Trackball light_trackball;

public:

    /**
     * @brief Default constructor.
     * @param parent Parent widget.
     */
    explicit QtTrackballWidget(QWidget *parent) : QtGlewInitializer(parent) {}

    /**
     * @brief Default destructor.
     */
    ~QtTrackballWidget () {}

    /**
     * @brief Initializes openGL and GLEW.
     */
    virtual void initializeGL (void)
    {
        makeCurrent();

        #ifdef TUCANODEBUG
        QGLFormat glCurrentFormat = this->format();
        cout << "QT GL version : " << glCurrentFormat.majorVersion() << " , " << glCurrentFormat.minorVersion() << endl;
        #endif
    }

    /**
     * @brief Resize callback for the widget.
     */
    virtual void resizeGL (void)
    {
        camera.setViewport(Eigen::Vector2f ((float)this->width(), (float)this->height()));
        camera.setPerspectiveMatrix(camera.getFovy(), (float)this->width()/(float)this->height(), 0.1f, 100.0f);
        light_trackball.setViewport(Eigen::Vector2f ((float)this->width(), (float)this->height()));
        updateGL();
    }

    /**
     * @brief Initializes the trackball and the mesh with a given filename
     */
    void initialize (void)
    {
        camera.setPerspectiveMatrix(60.0, (float)this->width()/(float)this->height(), 0.1f, 100.0f);
        camera.setRenderFlag(true);
        camera.setViewport(Eigen::Vector2f ((float)this->width(), (float)this->height()));

        light_trackball.setRenderFlag(false);
        light_trackball.setViewport(Eigen::Vector2f ((float)this->width(), (float)this->height()));

        updateGL();
    }

    /**
     * @brief Opens a mesh from file.
     *
     * @param filename Given mesh file.
     */
    virtual void openMesh (string filename)
    {
        QString str (filename.c_str());
        QStringList strlist = str.split(".");
        QString extension = strlist[strlist.size()-1];

        if (extension.compare("ply") != 0 && extension.compare("obj") != 0)
        {
            cerr << "file format [" << extension.toStdString() << "] not supported" << endl;
            return;
        }

		mesh = Mesh();

        if (extension.compare("ply") == 0)
        {
            MeshImporter::loadPlyFile(&mesh, filename);
        }
        if (extension.compare("obj") == 0)
        {
            MeshImporter::loadObjFile(&mesh, filename);
        }

        mesh.normalizeModelMatrix();
    }

protected:

    /**
     * @brief Callback for key press event.
     * @param event The key event that triggered the callback.
     */
    void keyPressEvent (QKeyEvent * event)
    {
        if (event->key() == Qt::Key_O)
        {
            QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Mesh Files (*.obj *.ply)"));
            if (!filename.isEmpty())
            {
                openMesh (filename.toStdString());
            }
        }
        event->ignore();
        updateGL();
    }

    /**
     * @brief Callback for mouse press event.
     *
     * The mouse press starts a rotation or a translation if Shift is pressed.
     * @param event The mouse event that triggered the callback.
     */
    void mousePressEvent (QMouseEvent * event)
    {
        setFocus ();
        Eigen::Vector2f screen_pos (event->x(), event->y());
        if (event->modifiers() & Qt::ShiftModifier)
        {
            if (event->button() == Qt::LeftButton)
            {
                camera.translateCamera(screen_pos);
            }
        }
        else
        {
            if (event->button() == Qt::LeftButton)
            {
                camera.rotateCamera(screen_pos);
            }
            if (event->button() == Qt::RightButton)
            {
                light_trackball.rotateCamera(screen_pos);
            }
        }
        updateGL ();
    }

    /**
     * @brief Callback for mouse move event.
     *
     * If rotating or translating, this method updates the trackball position.
     * @param event The mouse event that triggered the callback.
     */
    void mouseMoveEvent (QMouseEvent * event)
    {
        Eigen::Vector2f screen_pos (event->x(), event->y());
        if (event->modifiers() & Qt::ShiftModifier && event->buttons() & Qt::LeftButton)
        {
            camera.translateCamera(screen_pos);
        }
        else
        {
            if (event->buttons() & Qt::LeftButton)
            {
                camera.rotateCamera(screen_pos);
            }
            if (event->buttons() & Qt::RightButton)
            {
                light_trackball.rotateCamera(screen_pos);
            }
        }

        updateGL ();

    }

    /**
     * @brief Callback for mouse release event.
     *
     * Stops rotation or translation.
     * @param event The mouse event that triggered the callback.
     */
    void mouseReleaseEvent (QMouseEvent * event)
    {
        if (event->button() == Qt::LeftButton)
        {
            camera.endTranslation();
            camera.endRotation();
        }
        if (event->button() == Qt::RightButton)
        {
            light_trackball.endRotation();
        }

        updateGL ();
    }

    /**
     * @brief Callback for mouse wheel event.
     *
     * Changes the camera zoom, or the camera FOV is Shift is pressed.
     * @param event The mouse event that triggered the callback.
     */
    void wheelEvent (QWheelEvent * event)
    {
        const int WHEEL_STEP = 120;

        float pos = event->delta () / float (WHEEL_STEP);

        if (event->modifiers() & Qt::ShiftModifier) // change FOV
        {
            camera.incrementFov(pos);
        }
        else // change ZOOM
        {
            if( (pos > 0) )
            {
                camera.increaseZoom(1.05);
            }

            else if(pos < 0)
            {
                camera.increaseZoom(1.0/1.05);
            }
        }
        updateGL ();
    }

signals:

public slots:

};

}
#endif
