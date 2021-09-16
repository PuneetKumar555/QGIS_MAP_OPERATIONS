#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLayout>
#include <qgssettings.h>
#include <qgsproject.h>
#include <qgslayertreelayer.h>
#include <qgspallabeling.h>
#include <qgsvectorlayerlabeling.h>
#include <qgsmaptopixel.h>
#include <QGraphicsSvgItem>
#include <qgslayertreelayer.h>
#include "gdal.h"
#include "gdal_csv.h"
#include <qgsproviderregistry.h>
#include <QFile>
#include <qgsmarkersymbollayer.h>


MainWindow *MainWindow::sInstance = nullptr;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    sInstance = this;
    ui->setupUi(this);


    QString myPluginsDir = "C:\\OSGeo4W64\\apps\\qgis\\plugins\\";
    QgsProviderRegistry::instance(myPluginsDir);

    qDebug()<< "data providers:: "<< QgsProviderRegistry::instance()->providerList();

    qDebug()<< "PLUGIN providers:: "<< QgsProviderRegistry::instance()->pluginList();



    QWidget *centralWidget = this->centralWidget();
    QGridLayout *centralLayout = new QGridLayout( centralWidget );
    centralWidget->setLayout( centralLayout );
    centralLayout->setContentsMargins( 0, 0, 0, 0 );

    mMapCanvas = new QgsMapCanvas(centralWidget);
    mMapCanvas->setVisible(true);
    mMapCanvas->enableAntiAliasing(true);

    qDebug()<< "theme:: " << mMapCanvas->theme();

    mCentralContainer = new QStackedWidget;
    mCentralContainer->insertWidget( 0, mMapCanvas );
    centralLayout->addWidget( mCentralContainer, 0, 0, 2, 1 );
    mCentralContainer->setCurrentIndex(0);

    mInfoBar = new QgsMessageBar( centralWidget );
    mInfoBar->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    centralLayout->addWidget( mInfoBar, 0, 0, 1, 1 );

    mLayerTreeView = new QgsLayerTreeView( this );
    mLayerTreeView->setObjectName( QStringLiteral( "theLayerTreeView" ) ); // "theLayerTreeView" used to find this canonical instance later



    mMapCanvas->setAnnotationsVisible(true);

    qDebug()<<"magnification factor ::"<< mMapCanvas->magnificationFactor();
    mMapCanvas->zoomByFactor(1.5);



    //mMapCanvas->setRotation(10.5);  // rotates the mapcanvas
    mMapCanvas->zoomToFullExtent();


    initLayerTreeView();


    connect(ui->actionAddVectorLayer, &QAction::triggered, this, &MainWindow::addVectorLayers);
    connect(ui->actionAddRasterLayer, &QAction::triggered, this, &MainWindow::addRasterLayers);
    connect(mLayerTreeView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(on_Layer_tableView_doubleClicked(const QModelIndex &)));

    mMapCanvas->setAnnotationsVisible(true);



    crs=new  QgsCoordinateReferenceSystem("EPSG:4326");   // stad crs I think
    qDebug()<< " is CRS valid :: "<< crs->isValid();
    QgsProject::instance()->setCrs(*crs);
    //qDebug()<< "Qgs project instance :: " << QgsProject::instance();
    mMapCanvas->setProject(QgsProject::instance());   // setting project to canvas;
    mMapCanvas->setDestinationCrs(*crs);


    mytool = new QgsMapToolEmitPoint(mMapCanvas);
    mPan = new QgsMapToolPan(mMapCanvas);

    mMapCanvas->setMapTool(mPan);
   //mMapCanvas->setMapTool(mytool);




    connect(mytool,SIGNAL(canvasClicked(QgsPointXY,Qt::MouseButton)),this,SLOT(getPoint(QgsPointXY,Qt::MouseButton)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::addVectorLayers()
{

    QgsRectangle rec;
    mMapCanvas->freeze();

    QgsSettings settings;
    QString lastUsedDir = settings.value( QStringLiteral( "UI/lastOgrDir" ), QDir::homePath() ).toString();

    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          tr("Add Vector Layer"),
                                                          lastUsedDir,
                                                          "OGR layers(*.shp *.geojson *.gmt *.kml *.mbtiles *.osm   *.dbf)");

    if(filePaths.isEmpty())
    {
        return;
    }
    qDebug()<<" filepaths:: " << filePaths ;
    for(QStringList::iterator it = filePaths.begin(); it != filePaths.end(); it++)
    {
        QString baseName = QFileInfo(*it).completeBaseName();
        qInfo()<< "vector layer basename::"<< baseName;
        qDebug()<< "Vector Layer input:: "<< *it ;
        QgsVectorLayer *layer = new QgsVectorLayer(*it, baseName, "ogr");
        globallayer=layer;    // global layer variable is used for label
        if( !layer || !layer->isValid())
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to open file\n%1").arg(*it));
            return;
        }

        addVectorLayer(layer);
        rec= layer->extent();
    }

    mMapCanvas->setDestinationCrs(*crs);
    mMapCanvas->freeze(false);
    mMapCanvas->refresh();

    mMapCanvas->zoomToFeatureExtent(rec);
    mMapCanvas->refresh();


    QFileInfo fi( filePaths.at(0) );
    QString path = fi.path();

    settings.setValue( QStringLiteral( "UI/lastOgrDir" ), path );




}

bool MainWindow::addVectorLayer(QgsVectorLayer *layer)
{
    if(!layer)
    {
        qDebug() << "no vector layer";
        return false;
    }
    if(!layer->isValid())
    {
        qDebug() << "invalid vector layer";
        delete layer;
        return false;
    }

    // register this layer with the central layers registry
    QList<QgsMapLayer *> myList;
    myList << layer;
    QgsProject::instance()->addMapLayers( myList );

    qDebug()<<"after vector layer,,CRS  is valid or not :: "<< QgsProject::instance()->crs().isValid();
    qDebug() << "add layer, crs of project:" << QgsProject::instance()->crs().authid() << "," << QgsProject::instance()->crs().description();

    QgsLayerTreeLayer *layerTreeLayer = new QgsLayerTreeLayer(layer);
    qDebug() <<"layer treelayer name::" <<layerTreeLayer->name();

    qDebug()<< "QGS project Name :: " << QgsProject::instance()->fileName() ;

    return true;
}

void MainWindow::addRasterLayers()
{

    QgsRectangle rec;

    mMapCanvas->freeze();

    QgsSettings settings;
    QString lastUsedDir = settings.value( QStringLiteral( "UI/lastGdalDir" ), QDir::homePath() ).toString();

    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          tr("Add Raster Layer(s)"),
                                                          lastUsedDir,
                                                          "Raster layers(*.tif *.img *.grd *.hgt *.mbtiles *.TIFF *.gif  *.adf  *.vrt)");

    qInfo()<< "File is taken";

    if(filePaths.isEmpty())
    {
        return;
    }
    qInfo()<< "file path qstring list:::::"<<filePaths;
    for(QStringList::iterator it = filePaths.begin(); it != filePaths.end(); it++)
    {
        dtedfilePath = *it;
        QString baseName = QFileInfo(*it).completeBaseName();
        qInfo()<< "raster layer base name:: "<< baseName;
        QgsRasterLayer *layer = new QgsRasterLayer(*it, baseName, "gdal"); // this is where our project is breaking.
        //  qInfo()<<"before if loop";


        if( !layer || !layer->isValid())
        {
            // qInfo()<< "inside if loop";
            return;
        }
        // qDebug()<<"Height:: "<<   layer->height();
        layer->setCrs(*crs);
        qDebug()<< "crs of RASTER LAYER :: " <<  layer->crs().authid() << "   " << layer->crs().description() ;


        id= layer->id();
        qDebug()<< "***************"<< layer->maximumScale();

        tempi_rester= layer->extent();
        rec= layer->extent();
        globalraster= layer;
        addRasterLayer(layer);
    }
    qInfo()<<"after for loop";
    //qDebug()<< "theme:: " << mMapCanvas->theme();
    mMapCanvas->freeze(false);
    mMapCanvas->updateScale();
    mMapCanvas->refresh();


    mMapCanvas->zoomToFeatureExtent(rec);
    mMapCanvas->refresh();


    QFileInfo fi( filePaths.at(0) );
    QString path = fi.path();
    settings.setValue( QStringLiteral( "UI/lastGdalDir" ), path );
    qInfo()<< "rastor layer added";
}

bool MainWindow::addRasterLayer(QgsRasterLayer *layer)
{
    if ( ! layer )
    {
        return false;
    }

    if ( !layer->isValid() )
    {
        delete layer;
        return false;
    }

    // register this layer with the central layers registry
    QList<QgsMapLayer *> myList;
    myList << layer;
    QgsProject::instance()->addMapLayers( myList );
    qDebug()<<"raster layer added  ..CRS  is valid or not :: "<< QgsProject::instance()->crs().isValid();

    return true;
}

void MainWindow::initLayerTreeView()
{
    mLayerTreeDock = new QgsDockWidget( tr( "" ), this );
    mLayerTreeDock->setObjectName( QStringLiteral( "Layers" ) );
    mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    QgsLayerTreeModel *model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );

    model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
    model->setFlag( QgsLayerTreeModel::AllowNodeRename );
    model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
    model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
    model->setFlag( QgsLayerTreeModel::UseEmbeddedWidgets );
    model->setAutoCollapseLegendNodes( 10 );

    mLayerTreeView->setModel( model );


    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setMargin( 0 );
    vboxLayout->setContentsMargins( 0, 0, 0, 0 );
    vboxLayout->setSpacing( 0 );
    vboxLayout->addWidget( mLayerTreeView );

    QWidget *w = new QWidget;
    w->setLayout( vboxLayout );
    mLayerTreeDock->setWidget( w );
    addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );
    mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), mMapCanvas, this );
}



void MainWindow::on_action_annotation_layer_triggered()
{
    iselevation= false;
    QgsRectangle rec;
    mMapCanvas->freeze();

    QgsSettings settings;
    QString lastUsedDir = settings.value( QStringLiteral( "UI/lastGdalDir" ), QDir::homePath() ).toString();

    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          tr("Add Annotation Layer(s)"),
                                                          lastUsedDir,
                                                          "Annotation layers(*.csv)");

    qInfo()<< "File is taken";

    if(filePaths.isEmpty())
    {
        return;
    }

    for(QStringList::iterator it = filePaths.begin(); it != filePaths.end(); it++)
    {

        QString baseName = QFileInfo(*it).completeBaseName();
        qInfo()<< "raster layer base name:: "<< baseName;
        QString filepath= *it;
        qDebug()<< "file path " << filepath;

        QString uri= QString("file:///")+ filepath + QString("?type='csv&maxFields=10000&detectTypes=yes&xField=longitude&yField=latitude&crs=EPSG:4326&spatialIndex=yes&subsetIndex=no&watchFile=no");

        qDebug()<< "uri path" << uri;

        QgsVectorLayer * csvLayer = new QgsVectorLayer( uri, baseName, "delimitedtext" );

        qDebug()<< "Layer validity::" << csvLayer->isValid();
        globallayer=csvLayer;

        if( !csvLayer || !csvLayer->isValid())
        {
            // qInfo()<< "inside if loop";
            return;
        }


        QgsPalLayerSettings  label_settings=  QgsPalLayerSettings();
        label_settings.fieldName= QString("Name");   // label type
        label_settings.drawLabels= true;

        QgsTextFormat text_format =QgsTextFormat() ;
        QgsTextBackgroundSettings background_color= QgsTextBackgroundSettings();
        background_color.setFillColor(QColor(255,255,255)); // point
        background_color.setEnabled(true);

        text_format.setBackground(background_color);
        text_format.setColor(QColor(0,0,0));    // point
        label_settings.setFormat(text_format);
        QgsAbstractVectorLayerLabeling *label= new   QgsVectorLayerSimpleLabeling(label_settings);
        csvLayer->setLabeling(label );
        csvLayer->setLabelsEnabled(true);
        csvLayer->triggerRepaint(true);

        addVectorLayer(csvLayer);
        rec= csvLayer->extent();


    }


    mMapCanvas->freeze(false);
    mMapCanvas->refresh();

    mMapCanvas->zoomToFeatureExtent(rec);
    mMapCanvas->refresh();

}

void MainWindow::getPoint(QgsPointXY point, Qt::MouseButton button)
{
        X= point.x();
        Y= point.y();
    qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" ;
    if(iselevation){



        QByteArray ba = dtedfilePath.toLocal8Bit();
        const char *c_str2 = ba.data();
        GDALAllRegister();


        GDALDatasetH gtif = GDALOpen(c_str2,GA_ReadOnly);
        int count=  GDALGetRasterCount(gtif);
        qDebug()<< "count:: "<< count;
        GDALRasterBandH srcband=  GDALGetRasterBand(gtif, 1);

        double adfMinMax[2]={0,0};     // sucessfull  in geting values ...returs min and max of statistics
        GDALComputeRasterMinMax(srcband, 1,adfMinMax );

        qDebug()<< "min:: "<< adfMinMax[0];
        qDebug()<< "max:: "<< adfMinMax[1];

        double adfGeoTransform[6];
        auto temp = GDALGetGeoTransform(gtif,adfGeoTransform);

        int col=  GDALGetRasterBandXSize(srcband);
        int row=  GDALGetRasterBandYSize(srcband);
        auto origin_x= adfGeoTransform[0];
        qDebug()<< "origin_x" << origin_x;
        auto origin_y= adfGeoTransform[3];
        qDebug()<< "origin_y" << origin_y;
        auto cell_size=adfGeoTransform[1];
        qDebug()<< "cell_size" << cell_size;
        auto CRS= GDALGetProjectionRef(gtif);


        //      This is formula to define cell's column and row base on X and Y
        //      Try to sketch it so you can understand or maybe there's something wrong with my formula
        auto  col_x = int(((X - origin_x)/cell_size)+1) ;
        auto  row_y = int(((origin_y - Y)/cell_size)+1);

        float *pafScanline;
        int   nXSize = GDALGetRasterBandXSize(srcband);
        pafScanline = (float *)malloc(sizeof(float));


        GDALRasterIO( srcband, GF_Read,  col_x, row_y, 1, 1,
                      pafScanline, 1, 1, GDT_Float32,
                      0, 0 );

        if(pafScanline[0]>=adfMinMax[0]   &&pafScanline[0]<=adfMinMax[1])
            qDebug()<<"elevation "<< pafScanline[0] ;

            GDALClose(gtif);

    }
    else if(isline){
            MultiLinemeasurement(point);
    }
    else if(isarea)  {
        Areameasurement(point);

    }
    else if(isazimuthal){
          Anglemeasurement(point);
    }

}

void MainWindow::MultiLinemeasurement(QgsPointXY point){

    auto newpoint=   new  QgsVertexMarker(mMapCanvas);// make points like this. but also collect these points in vector . so that you can delete them afterwards
    newpoint->setCenter(point);
    newpoint->setColor(QColor(255,0,0));
    newpoint->setIconSize(10);
    newpoint->setIconType(4);
    newpoint->setPenWidth(5);
    newpoint->setFillColor(QColor(0,200,0));

    clicked_point.append(point);
    vec_point_to_free.append(newpoint);

    if(clicked_point.size()>=2){

        int size= clicked_point.size();
        auto point1= clicked_point[size-2];
        auto point2= clicked_point[size-1];
        /****************************code for line between points*********************************/
        QgsPoint p1,p2;
        p1.setX(point1.x());
        p1.setY(point1.y());
        p2.setX(point2.x());
        p2.setY(point2.y());

        QgsPolyline polyline1;
        QgsRubberBand* rubberband1= new QgsRubberBand(mMapCanvas, QgsWkbTypes::LineGeometry);
        polyline1.push_back(p1);
        polyline1.push_back(p2);
        rubberband1->setToGeometry(QgsGeometry::fromPolyline(polyline1));
        rubberband1->setColor(QColor(255,255,0));
        rubberband1->setWidth(1);
        vec_lines_to_free.append(rubberband1);
        /*************************************************************************************/

        /*****************code for label  between points**********************************************/

        QgsDistanceArea d;
        d.setEllipsoid("WGS84");

        sym1=     new QgsMarkerSymbol();
        sym1->setSize(0);
        auto length= d.measureLine(point1,point2) /1000;

        txt1 = new QTextDocument(QString::number( d.measureLine(point1,point2) /1000) + QString(" Km")); // working fine for line
        lb1= new QgsTextAnnotation(mMapCanvas);
        lb1->setDocument(txt1);
        lb1->setFrameSizeMm(QSizeF(3.0,1.0));

        auto midpoint=  QgsPointXY((point1.x() + point2.x())/2,(point1.y() + point2.y())/2);

        lb1->setMapPosition(midpoint);
        // lb1->setMapPosition(point);
        lb1->setFrameSizeMm(QSizeF(txt1->size().width()/3,txt1->size().height()/3));
        lb1->setFrameOffsetFromReferencePointMm(QPoint(1,-10));//(-)symbol puts label on up side....so (-) symbol has its own effect
        sym1->setLineAngle(50.0);
        lb1->setMarkerSymbol(sym1);
        lb1->setVisible(true);
        sym1->setColor(QColor(0,0,255));
        sym1->setAngle(50);
        sym1->setLineAngle(60);
        vec_lbs.push_back(lb1);
        qDebug()<< " angle for symbol ::"  <<  sym1->angle();
        QgsMapCanvasAnnotationItem *i1 =new QgsMapCanvasAnnotationItem(lb1,mMapCanvas);

        vec_marker_to_free.append(sym1);
        vec_i_to_free.append(i1);

        /***************************************************************************************/

    }



}
void MainWindow::Areameasurement(QgsPointXY point){

    auto newpoint=   new  QgsVertexMarker(mMapCanvas);// make points like this. but also collect these points in vector . so that you can delete them afterwards
    newpoint->setCenter(point);
    newpoint->setColor(QColor(255,0,0));
    newpoint->setIconSize(10);
    newpoint->setIconType(4);
    newpoint->setPenWidth(5);
    newpoint->setFillColor(QColor(0,200,0));

    clicked_point.append(point);
    vec_point_to_free.append(newpoint);

    if(clicked_point.size()>2)
    {

        QgsRubberBand* rubberband= new QgsRubberBand(mMapCanvas, QgsWkbTypes::PolygonGeometry);

        QgsPolygonXY polygonxy;

        polygonxy.append(clicked_point);
        rubberband->setToGeometry(QgsGeometry::fromPolygonXY(polygonxy));
        rubberband->setColor(QColor(255,0,0));
        rubberband->setWidth(1);
        vec_lines_to_free.append(rubberband);

        QgsDistanceArea d;
        d.setEllipsoid("WGS84");
        qDebug()<< "aREA" <<   d.measurePolygon(clicked_point);


        auto area=  d.measurePolygon(clicked_point);
        qDebug()<<(area/1000000);


    }


}
void MainWindow::Anglemeasurement(QgsPointXY point){

    auto newpoint=   new  QgsVertexMarker(mMapCanvas);// make points like this. but also collect these points in vector . so that you can delete them afterwards
    newpoint->setCenter(point);
    newpoint->setColor(QColor(255,0,0));
    newpoint->setIconSize(10);
    newpoint->setIconType(4);
    newpoint->setPenWidth(5);
    newpoint->setFillColor(QColor(0,200,0));

    clicked_point.append(point);
    vec_point_to_free.append(newpoint);


    if(clicked_point.size()>=2){

        int size= clicked_point.size();
        auto point1= clicked_point[size-2];
        auto point2= clicked_point[size-1];
        /****************************code for line between points*********************************/
        QgsPoint p1,p2;
        p1.setX(point1.x());
        p1.setY(point1.y());
        p2.setX(point2.x());
        p2.setY(point2.y());

        QgsPolyline polyline1;
        QgsRubberBand* rubberband1= new QgsRubberBand(mMapCanvas, QgsWkbTypes::LineGeometry);
        polyline1.push_back(p1);
        polyline1.push_back(p2);
        rubberband1->setToGeometry(QgsGeometry::fromPolyline(polyline1));
        rubberband1->setColor(QColor(255,255,0));
        rubberband1->setWidth(1);
        vec_lines_to_free.append(rubberband1);
        /*************************************************************************************/

        /*****************code for label  between points**********************************************/

        QgsDistanceArea d;
        d.setEllipsoid("WGS84");

        sym1=     new QgsMarkerSymbol();
        sym1->setSize(0);
        auto angle= point1.azimuth(point2);

        txt1 = new QTextDocument(QString::number(point1.azimuth(point2)) + QString("degree")); // working fine for line
        lb1= new QgsTextAnnotation(mMapCanvas);
        lb1->setDocument(txt1);
        lb1->setFrameSizeMm(QSizeF(3.0,1.0));

        auto midpoint=  QgsPointXY((point1.x() + point2.x())/2,(point1.y() + point2.y())/2);

        lb1->setMapPosition(midpoint);
        // lb1->setMapPosition(point);
        lb1->setFrameSizeMm(QSizeF(txt1->size().width()/3,txt1->size().height()/3));
        lb1->setFrameOffsetFromReferencePointMm(QPoint(1,-10));//(-)symbol puts label on up side....so (-) symbol has its own effect
        sym1->setLineAngle(50.0);
        lb1->setMarkerSymbol(sym1);
        lb1->setVisible(true);
        vec_lbs.push_back(lb1);
        sym1->setColor(QColor(0,0,255));
        sym1->setAngle(50);
        sym1->setLineAngle(60);
        qDebug()<< " angle for symbol ::"  <<  sym1->angle();
        QgsMapCanvasAnnotationItem *i1 =new QgsMapCanvasAnnotationItem(lb1,mMapCanvas);

        vec_marker_to_free.append(sym1);
        vec_i_to_free.append(i1);

        /***************************************************************************************/

    }


}



void MainWindow::on_pushButtonZoomOut_clicked()
{
    //qDebug()<< "Zoom out clicked";
    iselevation= false;
}

void MainWindow::on_pushButton_clicked()
{
    //qDebug()<< "Zoom in clicked";
     iselevation= false;
}

void MainWindow::on_actionZoom_In_triggered()
{
    qDebug()<< "Zoom In clicked";
     iselevation= false;







}

void MainWindow::on_actionZoomOut_triggered()
{

       iselevation= false;
}





void MainWindow::on_Layer_tableView_doubleClicked(const QModelIndex &index)
{
    int  selecIndex = index.row();
    QAbstractItemModel *model = mLayerTreeView->model();
    QString GotoLayerName;
    GotoLayerName = model->data(model->index(selecIndex,0)).toString();

    auto layerlist =  mMapCanvas->layers();

    for(auto layer: layerlist)
    {
        if(operator==(layer->name(),GotoLayerName))
        {
            qDebug()<< "temp name " << layer->name();
            QgsRectangle rec= layer->extent();
            mMapCanvas->zoomToFeatureExtent(rec);
            mMapCanvas->refresh();
        }
    }


}

void MainWindow::on_actionlabel_ON_triggered()
{
      iselevation= false;
    auto attribute_list = globallayer->attributeList();
    for(auto attribute: attribute_list){
        qDebug()<< "atttribute name of selected layer"<< globallayer->attributeDisplayName(attribute);
    }

    QgsPalLayerSettings  label_settings = QgsPalLayerSettings();
    label_settings.fieldName="name";   //( you can give any name from abouve atttribute display name in the form of  string )
    label_settings.drawLabels= true;

    if(globallayer->geometryType() == QgsWkbTypes::GeometryType::LineGeometry) // special for lines because alignment of line vector layer is not always horizontal(any direction)
        label_settings.placement= QgsPalLayerSettings::Line;

    QgsTextFormat text_format =QgsTextFormat() ;
    QgsTextBackgroundSettings background_color= QgsTextBackgroundSettings();
    background_color.setFillColor(QColor("yellow")); //  back ground color of label
    background_color.setEnabled(true);

    text_format.setBackground(background_color);
    text_format.setColor(QColor("red"));    // text color
    //text_format.setFont(font);

    label_settings.setFormat(text_format);
    QgsAbstractVectorLayerLabeling *label= new   QgsVectorLayerSimpleLabeling(label_settings);
    globallayer->setLabeling(label );
    globallayer->setLabelsEnabled(true);
    globallayer->triggerRepaint(true);

    mMapCanvas->refresh();
    mMapCanvas->refreshAllLayers();
}

void MainWindow::on_actionvector_symbol_change_triggered()
{
      iselevation= false;
    if(globallayer->geometryType()== QgsWkbTypes::GeometryType::PointGeometry){
        qDebug()<< "point vectorlayer";
        qDebug()<<" it is point vector layer";
        globallayer->startEditing();
        qDebug()<< "is editable " << globallayer->isEditable();
        QgsFeatureRenderer * layerRenderer= globallayer->renderer();
        QgsSingleSymbolRenderer *mSingleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
        QgsMarkerSymbol symbol ;
        QgsStringMap mp;
        mp[QString("name")]=  QString("pentagon");;
        mp[QString("color")]=QString("#000000");

        auto newsym=     symbol.createSimple(mp);
        // newsym->setAngle(rotation);
        // newsym->setSize(size);
        mSingleRenderer->setSymbol(newsym);


        //auto   image=         newsym->bigSymbolPreviewImage();
        //image.save(QString("C:\\Users\\Puneet\\Desktop\\symbology\\symbol.png")); // this saves the preview of symbol

        globallayer->setRenderer(mSingleRenderer);
        globallayer->triggerRepaint();
        // mLayerTreeView->refreshLayerSymbology(layer->id());
        globallayer->commitChanges();
        qDebug()<< "is editable after commiting  " << globallayer->isEditable();


    }
    else if(globallayer->geometryType()==  QgsWkbTypes::GeometryType::LineGeometry){


        globallayer->startEditing();
        QgsFeatureRenderer * layerRenderer= globallayer->renderer();
        QgsSingleSymbolRenderer *mSingleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
        QgsLineSymbol * symbol= new QgsLineSymbol();
        QgsStringMap mp;
        mp[QString("line_style")]=QString("dash");   // dot , solid also works
        mp[QString("color")]=QString("#FFFF00");;
        auto newsym=     symbol->createSimple(mp);
        //newsym->setWidth(size);
        mSingleRenderer->setSymbol(newsym);
        globallayer->setRenderer(mSingleRenderer);
        globallayer->triggerRepaint();
        globallayer->commitChanges();



    }
    else if(globallayer  ->geometryType()==QgsWkbTypes::GeometryType::PolygonGeometry){

        globallayer->startEditing();
        QgsFeatureRenderer * layerRenderer= globallayer->renderer();
        QgsSingleSymbolRenderer *mSingleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
        QgsFillSymbol * symbol= new QgsFillSymbol();
        QgsStringMap mp;

        mp[QString("color")]=QString("#FFFF00");;

        mp[QString("line_width")]=2;
        auto newsym=     symbol->createSimple(mp);
        newsym->setAngle(20); // double value (0-360)

        mSingleRenderer->setSymbol(newsym);
        globallayer->setRenderer(mSingleRenderer);
        globallayer->triggerRepaint();
        globallayer->commitChanges();


    }

    mMapCanvas->refresh();
    mMapCanvas->refreshAllLayers();



    /*************************only for point vector layer******************************************************/
//    globallayer->startEditing();
//    qDebug()<< "is editable " << globallayer->isEditable();
//    QgsFeatureRenderer * layerRenderer= globallayer->renderer();
//    QgsSingleSymbolRenderer *mSingleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
//    QgsMarkerSymbol* symbol = new  QgsMarkerSymbol();
//    QgsStringMap mp;
//    mp[QString("fill")]= QString("red");
//    mp[QString("name")]= QString("food_fastfood.svg");
//    mp[QString("size")]= QString("6");
//    QgsSvgMarkerSymbolLayer svglayer("");   // this also works ...no idea why
//    QgsSymbolLayer* svgsymbol=svglayer.create(mp);
//    auto newsym=     symbol->createSimple(mp);
//    newsym->changeSymbolLayer(0,svgsymbol);

//    mSingleRenderer->setSymbol(newsym);
//    globallayer->setRenderer(mSingleRenderer);
//    globallayer->triggerRepaint();
//    globallayer->commitChanges();
//    qDebug()<< "is editable after commiting  " << globallayer->isEditable();


//    mMapCanvas->refresh();
//    mMapCanvas->refreshAllLayers();

    /***********************************************************************************************************/
}

void MainWindow::on_actionelevation_DTED_Layer_triggered()
{

  mMapCanvas->setMapTool(mytool);


     iselevation= true;

}

void MainWindow::on_actionpoint_geometry_triggered()
{
    mMapCanvas->setMapTool(mytool);
     isline=true;
}

void MainWindow::on_actionarea_triggered()
{
    mMapCanvas->setMapTool(mytool);
   isarea= true;
}

void MainWindow::on_actionazimuthal_triggered()
{
    mMapCanvas->setMapTool(mytool);
    isazimuthal=true;
}

void MainWindow::on_actionPanning_triggered()
{
     mMapCanvas->setMapTool(mPan);
     DeleteAllPoints();
}


void MainWindow::DeleteAllPoints(){


    if(clicked_point.length() > 0)
        clicked_point.clear();


    for(int i= 0; i<vec_point_to_free.length();i++)
    {
        if(vec_point_to_free[i]!=nullptr)
            delete vec_point_to_free[i];
    }

    if(vec_point_to_free.length() > 0 )
    {

        vec_point_to_free.clear();
    }





    if(!vec_lbs.empty()){
        for(int i=0;i<vec_lbs.size();i++){
            vec_lbs[i]->setVisible(false);
            delete vec_lbs[i];
        }
        vec_lbs.clear();
    }

    if(!vec_lines_to_free.empty()){
        for(int i=0;i<vec_lines_to_free.size();i++){
            vec_lines_to_free[i]->setVisible(false);
            delete vec_lines_to_free[i];
        }
        vec_lines_to_free.clear();
    }



}
