#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMap>

#include <qgsmapcanvas.h>
#include <qgsmapoverviewcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrasterlayer.h>

#include <qgslayertreeview.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgslayertreemodel.h>
#include <qgslayertreeregistrybridge.h>
#include <qgsdockwidget.h>
#include <qgsmessagebar.h>
#include <qgsmaptoolpan.h>
#include <qgsmaplayer.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsmaprendererparalleljob.h>
#include <qgsmaptoolemitpoint.h>
#include <qgssvgannotation.h>

#include "qgsmaptoolzoom.h"
#include <qgsmaptoolpan.h>
#include <qgsvertexmarker.h>
#include <qgsrubberband.h>
#include <qgssymbol.h>
#include <QTextDocument>
#include <qgstextannotation.h>
#include <qgsmapcanvasannotationitem.h>
#include "qgslayertreeviewdefaultactions.h"


namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QgsLayerTreeView *layerTreeView()
    {
        return mLayerTreeView;
    }

    QgsLayerTreeMapCanvasBridge *layerTreeCanvasBridge()
    {
        return mLayerTreeCanvasBridge;
    }


    void MultiLinemeasurement(QgsPointXY point);
    void Areameasurement(QgsPointXY point);
    void Anglemeasurement(QgsPointXY point);
    void DeleteAllPoints();


public slots:
    void addVectorLayers();
    bool addVectorLayer(QgsVectorLayer *layer);

    void addRasterLayers();
    bool addRasterLayer(QgsRasterLayer *layer);

    void on_action_annotation_layer_triggered();

   void  getPoint(QgsPointXY,Qt::MouseButton);

   void on_Layer_tableView_doubleClicked(const QModelIndex &index);

private slots:


    void on_pushButtonZoomOut_clicked();

    void on_pushButton_clicked();

    void on_actionZoom_In_triggered();

    void on_actionZoomOut_triggered();

    void on_actionlabel_ON_triggered();

    void on_actionvector_symbol_change_triggered();

    void on_actionelevation_DTED_Layer_triggered();

    void on_actionpoint_geometry_triggered();

    void on_actionarea_triggered();

    void on_actionazimuthal_triggered();

    void on_actionPanning_triggered();

private:
    Ui::MainWindow *ui;
    static MainWindow *sInstance;

    QgsMapCanvas *mMapCanvas = nullptr;
    QgsMapToolPan * mPan= nullptr;
    QgsMapOverviewCanvas *mOverviewCanvas = nullptr;

   QgsMapToolEmitPoint *mytool=nullptr;
    QgsLayerTreeView *mLayerTreeView = nullptr;
    QgsLayerTreeMapCanvasBridge *mLayerTreeCanvasBridge = nullptr;

    QgsDockWidget *mLayerTreeDock = nullptr;    
    QgsDockWidget *mOverviewDock = nullptr;     
    QStackedWidget *mCentralContainer = nullptr; 
    QgsMessageBar *mInfoBar = nullptr;
    QgsCoordinateReferenceSystem *crs=nullptr;
    QgsVertexMarker*  marker1=nullptr;
    QgsVertexMarker*  marker2=nullptr;
    QgsMarkerSymbol * sym1= nullptr;
    QgsMarkerSymbol * sym2= nullptr;
    QTextDocument *  txt1=nullptr;
     QTextDocument *  txt2=nullptr;
    QgsTextAnnotation * lb1= nullptr;
    QgsTextAnnotation * lb2= nullptr;
    QgsSvgAnnotation* svgicon=nullptr;
    QgsVectorLayer * tempo= nullptr;
    QgsRectangle tempi;
    QgsRectangle tempi_rester;
    QgsVectorLayer *globallayer= nullptr;
    QgsRasterLayer *globalraster= nullptr;
    QString id;
    QVector<QgsPointXY>vec_point;
    QVector<QgsVertexMarker*>vec_point_to_free;
    QVector<QgsRubberBand*>vec_lines_to_free;
    double distancce_point=0;
    QString dtedfilePath;
    double X;
    double Y;
    bool iselevation =false;
    bool isline=false;
    bool isarea=false;
    bool isazimuthal=false;
    QVector<QgsPointXY>clicked_point;
    QVector<QgsMarkerSymbol*>vec_marker_to_free;
    QVector<QgsMapCanvasAnnotationItem*> vec_i_to_free;
    QVector<QgsTextAnnotation*>vec_lbs;

    //QgsMapRendererParallelJob *render= nullptr;
    //QgsRectangle  tempo;

    void initLayerTreeView();
    void creatOverview();


};

#endif // MAINWINDOW_H
