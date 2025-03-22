#include "ChartLegendViewPlugin.h"

#include <event/Event.h>

#include <DatasetsMimeData.h>

#include <QDebug>
#include <QMimeData>

#include <vector>
#include <random>

#include <QString>
#include <QGraphicsTextItem>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QColor>

#include <QGradient>
#include <QLinearGradient>
#include <QMap>
#include <QPair>
#include <QCursor>
#include <algorithm> 
#include<QScrollBar>
Q_PLUGIN_METADATA(IID "studio.manivault.ChartLegendViewPlugin")

using namespace mv;





QColor interpolateColor(double value, const QColor& startColor, const QColor& endColor) {
    // Clamp the value to the range [0, 1]
    value = std::clamp(value, 0.0, 1.0);

    int red = static_cast<int>(startColor.red() + value * (endColor.red() - startColor.red()));
    int green = static_cast<int>(startColor.green() + value * (endColor.green() - startColor.green()));
    int blue = static_cast<int>(startColor.blue() + value * (endColor.blue() - startColor.blue()));

    // Clamp the color components to the range [0, 255]
    red = std::clamp(red, 0, 255);
    green = std::clamp(green, 0, 255);
    blue = std::clamp(blue, 0, 255);

    return QColor(red, green, blue);
}

QColor selectColorVal(double value, double min, double max, const QString& colorScheme, bool mirror) {
    double normalizedValue = (value - min) / (max - min);
    if (mirror) {
        normalizedValue = 1.0 - normalizedValue;
    }

    // Define color maps as a series of key colors
    QMap<QString, QVector<QColor>> colorSchemes = {
        {"qualitative", {QColor(255, 127, 14), QColor(44, 160, 44)}}, // Example qualitative scheme
        {"RdYlBu", {QColor(44, 123, 182), QColor(171, 217, 233), QColor(255, 255, 191), QColor(253, 174, 97), QColor(215, 25, 28)}}, // Inversed Red-Yellow-Blue
        {"YlGn", {QColor(255, 255, 204), QColor(161, 218, 180), QColor(65, 171, 93), QColor(0, 104, 55)}}, // Yellow-Green
        {"RdYlGn", {QColor(0, 104, 55), QColor(166, 217, 106), QColor(255, 255, 191), QColor(253, 174, 97), QColor(215, 25, 28)}}, // Inversed Red-Yellow-Green
        {"GnBu", {QColor(224, 243, 219), QColor(168, 221, 181), QColor(67, 162, 202), QColor(1, 97, 132)}}, // Green-Blue
        {"YlGnBu", {QColor(255, 255, 204), QColor(161, 218, 180), QColor(65, 171, 93), QColor(44, 127, 184), QColor(8, 81, 156)}}, // Yellow-Green-Blue
        {"Spectral", {QColor(94, 79, 162), QColor(50, 136, 189), QColor(102, 194, 165), QColor(171, 221, 164), QColor(230, 245, 152), QColor(255, 255, 191), QColor(254, 224, 139), QColor(253, 174, 97), QColor(244, 109, 67), QColor(158, 1, 66)}}, // Inversed Spectral
        {"BrBG", {QColor(140, 81, 10), QColor(216, 179, 101), QColor(246, 232, 195), QColor(245, 245, 245), QColor(199, 234, 229), QColor(90, 180, 172), QColor(1, 133, 113)}}, // Brown-Blue-Green
        {"YlOrBr", {QColor(255, 255, 212), QColor(254, 227, 145), QColor(254, 196, 79), QColor(254, 153, 41), QColor(217, 95, 14), QColor(127, 59, 8)}}, // Yellow-Orange-Brown
        {"RdBu", {QColor(5, 113, 176), QColor(146, 197, 222), QColor(209, 229, 240), QColor(247, 247, 247), QColor(253, 219, 199), QColor(244, 165, 130), QColor(202, 0, 32)}}, // Inversed Red-Blue
        {"RdPu", {QColor(254, 224, 210), QColor(252, 197, 192), QColor(250, 159, 181), QColor(247, 104, 161), QColor(221, 52, 151), QColor(174, 1, 126), QColor(122, 1, 119)}}, // Red-Purple
        {"Plasma", {QColor(13, 8, 135), QColor(84, 2, 163), QColor(139, 10, 165), QColor(185, 50, 137), QColor(219, 92, 104), QColor(244, 136, 73), QColor(254, 188, 43), QColor(240, 249, 33)}}, // Plasma
        {"PuOr", {QColor(45, 0, 75), QColor(84, 39, 136), QColor(128, 115, 172), QColor(178, 171, 210), QColor(216, 218, 235), QColor(247, 247, 247), QColor(254, 224, 182), QColor(253, 184, 99), QColor(224, 130, 20), QColor(179, 88, 6), QColor(127, 59, 8)}}, // Inversed Purple-Orange
        {"BuPu", {QColor(237, 248, 251), QColor(191, 211, 230), QColor(158, 188, 218), QColor(140, 150, 198), QColor(140, 107, 177), QColor(136, 65, 157), QColor(129, 15, 124), QColor(77, 0, 75)}}, // Blue-Purple
        {"Reds", {QColor(255, 245, 240), QColor(254, 224, 210), QColor(252, 187, 161), QColor(252, 146, 114), QColor(251, 106, 74), QColor(239, 59, 44), QColor(203, 24, 29), QColor(153, 0, 13), QColor(103, 0, 13)}}, // Reds
        {"Viridis", {QColor(255, 255, 0), QColor(200, 220, 50), QColor(92, 200, 99), QColor(39, 173, 129), QColor(33, 144, 141), QColor(44, 114, 142), QColor(59, 82, 139), QColor(72, 35, 116), QColor(68, 1, 84)}}, // Reversed Viridis
        {"Q_BlGrRd", {QColor(5, 48, 97), QColor(33, 102, 172), QColor(67, 147, 195), QColor(146, 197, 222), QColor(209, 229, 240), QColor(247, 247, 247), QColor(253, 219, 199), QColor(244, 165, 130), QColor(202, 0, 32), QColor(165, 0, 38)}}, // Custom Blue-Green-Red
        {"Magma", {QColor(0, 0, 4), QColor(28, 16, 68), QColor(79, 18, 123), QColor(129, 37, 129), QColor(181, 54, 122), QColor(229, 80, 100), QColor(251, 135, 97), QColor(254, 194, 135), QColor(252, 253, 191)}}, // Magma
       {"PiYG", {QColor(77, 172, 38), QColor(184, 225, 134), QColor(230, 245, 208), QColor(247, 247, 247), QColor(253, 224, 239), QColor(241, 182, 218), QColor(208, 28, 139)}}, // Inversed Pink-Yellow-Green
        {"default", {QColor(0, 0, 0), QColor(255, 255, 255)}} // Black to white
    };

    // Get the color map for the selected scheme
    QVector<QColor> colors = colorSchemes.value(colorScheme, colorSchemes["default"]);

    // Determine the segment of the color map to interpolate
    int numSegments = colors.size() - 1;
    double segmentSize = 1.0 / numSegments;
    int segmentIndex = static_cast<int>(normalizedValue / segmentSize);
    segmentIndex = std::clamp(segmentIndex, 0, numSegments - 1);

    // Interpolate within the segment
    double segmentValue = (normalizedValue - segmentIndex * segmentSize) / segmentSize;
    return interpolateColor(segmentValue, colors[segmentIndex], colors[segmentIndex + 1]);
}

QStringList convertToQStringList(const std::vector<QString>& vector) {
    QStringList qStringList;
    for (const auto& str : vector) {
        qStringList.append(str);
    }
    return qStringList;
}
int getIndexOfQString(const std::vector<QString>& vector, const QString& value) {
    auto it = std::find(vector.begin(), vector.end(), value);
    if (it != vector.end()) {
        return std::distance(vector.begin(), it);
    }
    else {
        return -1; // Value not found
    }
}

ChartLegendViewPlugin::ChartLegendViewPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dropWidget(nullptr),
    _settingsAction(*this, _core),
    _verticalToolbarAction(this, "Settings Holder Vertical"),
    _horizontalToolbarAction(this, "Settings Holder Horizontal"),
    _legendWidget(nullptr)
{
    setSerializationName("Legend:Plugin");
    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
}

void ChartLegendViewPlugin::init()
{


    getWidget().setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //




    _legendWidget = new QFrame(&getWidget());
    _legendWidget->setStyleSheet("background-color: white;"); // Set white background
    //_legendWidget->installEventFilter(this);
    // Create layout
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

   _verticalToolbarAction.addAction(&_settingsAction.getColorSettingsHolder(), 3, GroupAction::Vertical);
    //addDockingAction(&_horizontalToolbarAction);
   // addDockingAction(&_verticalToolbarAction, nullptr, gui::DockAreaFlag::Right, true, gui::AutoHideLocation::Right, QSize(500, 500));
    // Add horizontal toolbar action to layout
   //layout->addWidget(_horizontalToolbarAction.createWidget(&getWidget()), 0.5);
       //_dropWidget = new DropWidget(&getWidget());
    //_dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(&getWidget(), "No data loaded", "Drag the LegendData in this view"));
    //_dropWidget->setAcceptDrops(true);


    //_dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
    //    // A drop widget can contain zero or more drop regions
    //    DropWidget::DropRegions dropRegions;

    //    const auto datasetsMimeData = dynamic_cast<const DatasetsMimeData*>(mimeData);

    //    if (datasetsMimeData == nullptr)
    //        return dropRegions;

    //    if (datasetsMimeData->getDatasets().count() > 1)
    //        return dropRegions;

    //    const auto dataset = datasetsMimeData->getDatasets().first();
    //    const auto datasetGuiName = dataset->text();
    //    const auto datasetId = dataset->getId();
    //    const auto dataType = dataset->getDataType();
    //    const auto dataTypes = DataTypes({ PointType, ClusterType });

    //    if (dataTypes.contains(dataType)) {
    //        if (datasetId == _settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getId()) {
    //            dropRegions << new DropWidget::DropRegion(this, "Warning", "Data already loaded", "exclamation-circle", false);
    //        }
    //        else {
    //            if (dataType == ClusterType) {
    //                auto candidateDataset = mv::data().getDataset<Clusters>(datasetId);
    //                dropRegions << new DropWidget::DropRegion(this, "Clusters", QString("Visualize %1 as chart legend").arg(datasetGuiName), "map-marker-alt", true, [this, candidateDataset]() {
    //                    loadData({ candidateDataset });
    //                    _dropWidget->setShowDropIndicator(false);
    //                    });
    //            }
    //            else if (dataType == PointType) {
    //                auto candidateDataset = mv::data().getDataset<Points>(datasetId);
    //                dropRegions << new DropWidget::DropRegion(this, "Points", QString("Visualize %1 as parallel coordinates").arg(datasetGuiName), "map-marker-alt", true, [this, candidateDataset]() {
    //                    loadData({ candidateDataset });
    //                    _dropWidget->setShowDropIndicator(false);
    //                    });
    //            }
    //            else {
    //                dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);
    //            }
    //        }
    //    }
    //    else {
    //        dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);
    //    }
    //    return dropRegions;
    //    });

   layout->addWidget(_legendWidget, 9.5);
   //layout->addWidget(_dropWidget, 0.5);

    // Update data when data set changed
    //connect(&_settingsAction.getPositionSettingsHolder().getPositionDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, &ChartLegendViewPlugin::convertPointDataAndUpdateChart);

   const auto colorDatasetChange = [this]() -> void
   {
       auto currentDataset = _settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset();
       if (currentDataset.isValid())
       {
           connect(&currentDataset, &Dataset<DatasetImpl>::dataChanged, this, &ChartLegendViewPlugin::updateChart);
           connect(&currentDataset, &Dataset<DatasetImpl>::changed, this, &ChartLegendViewPlugin::updateChart);
           connect(&currentDataset, &Dataset<DatasetImpl>::dataSelectionChanged, this, &ChartLegendViewPlugin::updateChart);
           
           updateChart();
       }
   };
   connect(&_settingsAction.getColorSettingsHolder().getColorDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, colorDatasetChange);

    //connect(&_settingsAction.getColorSettingsHolder().getColorDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, &ChartLegendViewPlugin::updateChart);
    //connect(&_currentDataset, &Dataset<DatasetImpl>::dataChanged, this, &ChartLegendViewPlugin::updateChart);



    getWidget().setLayout(layout);
    const auto projectOpenDone = [this]() -> void
        {

        };
    connect(&mv::projects(), &AbstractProjectManager::projectOpened, this, projectOpenDone);
    /*
    const auto clusterDatasetSelection = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getClusterDatasetAction().getCurrentDataset().isValid())
            {
                auto data = mv::data().getDataset<Clusters>(_settingsAction.getColorSettingsHolder().getClusterDatasetAction().getCurrentDataset()->getId());
                auto clusters = data->getClusters();
                _currentClusterDataSet = data;
                auto clusterNames = _currentClusterDataSet->getClusterNames();
                QStringList stringList = convertToQStringList(clusterNames);


                auto getparentDataset = _currentClusterDataSet->getParent();
                if (getparentDataset.isValid())
                {
                    _clusterParentPointDataSet = mv::data().getDataset<Points>(getparentDataset->getId());
                }

                _settingsAction.getColorSettingsHolder().getSelectionStringList().setOptions(stringList);
                _settingsAction.getColorSettingsHolder().getSelectionStringList().setSelectedOptions({ "" });
                if (_currentClusterDataSet.isValid())
                {
                    events().notifyDatasetDataChanged(_currentClusterDataSet);
                }
            }
            else
            {
                _currentClusterDataSet = nullptr;
                _clusterParentPointDataSet = nullptr;
            }
        };
    //connect(&_settingsAction.getColorSettingsHolder().getClusterDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, clusterDatasetSelection);
    */
    /*const auto colorDatasetSelection = [this]() -> void
        {
            
            _currentDataset = _settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset();
    };
    connect(&_settingsAction.getColorSettingsHolder().getColorDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, colorDatasetSelection);*/

    const auto selectionStringListSelection = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
            {
                updateChart();
            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getSelectionStringList(), &OptionsAction::selectedOptionsChanged, this, selectionStringListSelection);

    const auto selectionColorSelection = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
            {
                updateChart();
            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getSelectionColorAction(), &ColorAction::changed, this, selectionColorSelection);



    const auto titleChange = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
            {
                updateChart();
            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getChartTitleAction(), &StringAction::stringChanged, this, titleChange);

    const auto clusterSelectionStringSelection = [this]() -> void
    {
        if (_settingsAction.getColorSettingsHolder().getClusterSelectionString().getString() != "")
        {
            auto clusterNames = _settingsAction.getColorSettingsHolder().getClusterSelectionString().getString();
            QStringList options = _settingsAction.getColorSettingsHolder().getSelectionStringList().getOptions();
            auto delimiter = _settingsAction.getColorSettingsHolder().getDelimiterString().getString();
            // Split clusterNames by delimiter
            QStringList list = clusterNames.split(delimiter);
            
            if (!list.isEmpty())
            {
                // Convert list to QSet to perform intersection
                QSet<QString> listSet = QSet<QString>(list.begin(), list.end());

                // Filter options that are present in listSet
                QStringList common;
                for (const QString& option : options)
                {
                    if (listSet.contains(option))
                    {
                        common.append(option);
                    }
                }
                
                if (!common.isEmpty())
                {
                    //qDebug() << "common: " << common;
                    _settingsAction.getColorSettingsHolder().getSelectionStringList().setSelectedOptions(common);
                }
                else
                {
                    _settingsAction.getColorSettingsHolder().getSelectionStringList().setSelectedOptions({ "" });
                }
            }
        }
        else
        {
            _settingsAction.getColorSettingsHolder().getSelectionStringList().setSelectedOptions({ "" });
        }
    };

    connect(&_settingsAction.getColorSettingsHolder().getClusterSelectionString(), &StringAction::stringChanged, this, clusterSelectionStringSelection);

    const auto delimiterStringSelection = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getClusterSelectionString().getString() != "")
            {
                auto string = _settingsAction.getColorSettingsHolder().getClusterSelectionString().getString();
                _settingsAction.getColorSettingsHolder().getClusterSelectionString().setString("");
                _settingsAction.getColorSettingsHolder().getClusterSelectionString().setString(string);
            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getDelimiterString(), &StringAction::stringChanged, this, delimiterStringSelection);

    const auto colorMapActionSelection = [this]() -> void
        {
            if(_settingsAction.getColorSettingsHolder().getColorMapAction().getColorMap()!="" && _settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
            {
                const auto& mirrorAction = _settingsAction.getColorSettingsHolder().getColorMapAction().getMirrorAction(ColorMapAction::Axis::X);
                QString s1 = _settingsAction.getColorSettingsHolder().getColorMapAction().getColorMap();
                QString s2 = "*%*";
                QString s3;
                if (mirrorAction.isChecked())
                {
                    s3 = "T";
                }
                else
                {
                    s3 = "F";
                }

                if (_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid() && s1 != "")
                {
                    QVariantMap colorMap;
                    colorMap.insert("color", s1);
                    colorMap.insert("mirror", s3);
                    QVariantList full = QVariantList{ colorMap };
                    
                    //TODO: emit _chartWidget->getCommunicationObject().qt_js_setPointDataColor(full);

                        //qDebug() << "sending color: ";
                        updateChart();


                }

            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getColorMapAction(), &ColorMap1DAction::imageChanged, this, colorMapActionSelection);

    const auto dimSelectionStringSelection = [this]() -> void
        {
            if (!_settingsAction.getColorSettingsHolder().getDimSelectionString().getString().isEmpty()) {
                auto dimensionName = _settingsAction.getColorSettingsHolder().getDimSelectionString().getString();
                auto options = _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().getDimensionNames();
                int currentIndex = _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().getCurrentDimensionIndex();
                if (options[currentIndex] != dimensionName) {
                    int newIndex = options.indexOf(dimensionName);
                    if (newIndex != -1) {
                        _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().setCurrentDimensionName(dimensionName);
                    }
                }
            }
        };
    connect(&_settingsAction.getColorSettingsHolder().getDimSelectionString(), &StringAction::stringChanged, this, dimSelectionStringSelection);

    const auto dimensionSelectionSelection = [this]() -> void
        {
            //_settingsAction.getColorSettingsHolder().getDimSelectionString().setString("");
            if (_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
            {
                updateChart();
            }

        };
    connect(&_settingsAction.getColorSettingsHolder().getDimensionSelectionAction(), &DimensionPickerAction::currentDimensionIndexChanged, this, dimensionSelectionSelection);
   /*
    const auto pointDatasetSelection = [this]() -> void
        {
            if (_settingsAction.getColorSettingsHolder().getPointDatasetAction().getCurrentDataset().isValid())
            {

                auto data = mv::data().getDataset<Points>(_settingsAction.getColorSettingsHolder().getPointDatasetAction().getCurrentDataset()->getId());
                auto dimensionNames = data->getDimensionNames();
                QStringList stringList = convertToQStringList(dimensionNames);
                _currentPointDataSet = data;
                //qDebug() << "pointDatasetSelection: " << stringList.size();
                _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().setOptions(stringList);
                _settingsAction.getColorSettingsHolder().getDimSelectionString().setString("");
                _settingsAction.getColorSettingsHolder().getDimSelectionString().setString(dimensionNames[0]);

            }
            else
            {
                _currentPointDataSet = nullptr;
            }

        };
    connect(&_settingsAction.getColorSettingsHolder().getPointDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, pointDatasetSelection);
    */
    //if (_currentClusterDataSet.isValid())
    //{
    //    events().notifyDatasetDataChanged(_currentClusterDataSet);
    //}
    //else
    //{

    //    clearChartFrame();
    //    //_dropWidget->setShowDropIndicator(true);
    //}

}



void ChartLegendViewPlugin::loadData(const mv::Datasets& datasets)
{
    // Exit if there is nothing to load
    if (datasets.isEmpty())
    {
        qDebug() << "LegendPlugin::loadData: No data set to load";
        return;
    }

    auto dataset = datasets.first();

    try {
        _settingsAction.getColorSettingsHolder().getColorDatasetAction().setCurrentDataset(dataset);
    }
    catch (const std::exception& e) {
        qCritical() << "Exception caught in loadData:" << e.what();
    }
    catch (...) {
        qCritical() << "Unknown exception caught in loadData";
    }
}

void ChartLegendViewPlugin::initLoader()
{


    //if (_currentClusterDataSet.isValid() || _currentPointDataSet.isValid())
    //{
    //    if (_settingsAction.getSelectionSettingsHolder().getPointTypeSelected().isChecked() && !_settingsAction.getSelectionSettingsHolder().getClusterTypeSelected().isChecked())
    //    {
    //        if (_currentPointDataSet.isValid())
    //        {

    //            convertPointDataAndUpdateChart();
    //        }
    //    }
    //    else if (_settingsAction.getSelectionSettingsHolder().getClusterTypeSelected().isChecked() && !_settingsAction.getSelectionSettingsHolder().getPointTypeSelected().isChecked())
    //    {
    //        if (_currentClusterDataSet.isValid())
    //        {

    //            convertClusterDataAndUpdateChart();
    //        }
    //    }
    //}
    //else
    //{

    //    qDebug() << "LegendPlugin::initLoader: No data set to load";
    //}
}

void ChartLegendViewPlugin::clearChartFrame() {
    if (_legendWidget) {
        QLayout* layout = _legendWidget->layout();
        if (layout) {
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();  // Remove and delete all child widgets
                delete item;
            }
            delete layout;  // Delete the existing layout
        }
    }
}


void ChartLegendViewPlugin::createNumericalChart(QVariantList chartDataNumerical) {
    clearChartFrame();  // Clear existing chart elements

    if (chartDataNumerical.isEmpty()) {
        qDebug() << "Numerical chart data is empty.";
        return;
    }

    QVariantMap data = chartDataNumerical[0].toMap();
    double min = data["min"].toDouble();
    double max = data["max"].toDouble();
    _legendWidget->setCursor(Qt::ArrowCursor);
    // Create a scroll area
    QScrollArea* scrollArea = new QScrollArea(_legendWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);  // Remove the border

    // Create a container widget for the numerical chart
    QWidget* containerWidget = new QWidget();
    containerWidget->setStyleSheet("border: none;");  // Ensure no border
    QVBoxLayout* layout = new QVBoxLayout(containerWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignTop);  // Align the layout to the top

    // Create scene and view for numerical chart
    QGraphicsScene* scene = new QGraphicsScene(containerWidget);
    QGraphicsView* view = new QGraphicsView(scene, containerWidget);
    view->setFixedSize(70, 250);  // Fixed dimensions for the numerical chart
    view->setFrameStyle(QFrame::NoFrame);  // Remove the border

    // Create a vertical gradient from min to max values
    QLinearGradient gradient(0, 0, 0, 200);
    for (int i = 0; i <= 100; i += 10) {
        double value = min + (i / 100.0) * (max - min);
        QColor color = selectColor(value, min, max);  // Use selectColor to get the gradient color
        gradient.setColorAt(i / 100.0, color);
    }

    // Add the gradient rectangle to the scene
    QGraphicsRectItem* rect = scene->addRect(0, 0, 15, 180, Qt::NoPen, QBrush(gradient));

    // Add numerical labels
    int numTicks = 5;
    for (int i = 0; i <= numTicks; ++i) {
        double value = min + i * (max - min) / numTicks;
        double y = 180 - (i * 180.0 / numTicks);  // Y position for labels

        QGraphicsTextItem* label = scene->addText(QString::number(value, 'f', 1));
        label->setDefaultTextColor(Qt::black);  // Set text color to black
        label->setPos(20, y - 10);  // Adjust position for better alignment
    }

    // Add the view to the container widget
    layout->addWidget(view);
    containerWidget->setLayout(layout);

    // Set the container widget as the scroll area's widget
    scrollArea->setWidget(containerWidget);

    // Add the scroll area to the legend widget's layout
    QVBoxLayout* mainLayout = new QVBoxLayout(_legendWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // Ensure no additional margins
    mainLayout->setAlignment(Qt::AlignTop);  // Align the layout to the top

    // Add title
    QLabel* titleLabel = new QLabel(_settingsAction.getColorSettingsHolder().getChartTitleAction().getString(), _legendWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: black;");  // Set text color to black
    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(scrollArea);
    _legendWidget->setLayout(mainLayout);
}

void ChartLegendViewPlugin::createCategoricalChart(QVariantList chartDataCategorical) {
    // Capture the current scroll position
    int scrollPos = 0;
    if (_legendWidget->layout()) {
        QScrollArea* existingScrollArea = _legendWidget->findChild<QScrollArea*>();
        if (existingScrollArea) {
            scrollPos = existingScrollArea->verticalScrollBar()->value();
        }
    }

    clearChartFrame();  // Clear existing chart elements

    if (chartDataCategorical.isEmpty()) {
        qDebug() << "Categorical chart data is empty.";
        return;
    }

    QVariantMap data = chartDataCategorical[0].toMap();
    QVariantList clusters = data["clusters"].toList();

    // Create a scroll area
    QScrollArea* scrollArea = new QScrollArea(_legendWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    // Create a container widget for the legend rows
    QWidget* containerWidget = new QWidget();
    containerWidget->setStyleSheet("border: none;");
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* layout = new QVBoxLayout(containerWidget);
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    int itemHeight = 30;
    QStringList selectionStrings = _settingsAction.getColorSettingsHolder().getSelectionStringList().getSelectedOptions();
    QColor colorSelection = _settingsAction.getColorSettingsHolder().getSelectionColorAction().getColor();
    //qDebug() << "Selection Strings:" << selectionStrings;
    //qDebug() << "Selection Color:" << colorSelection;

    _selectedClusters.clear();
    for (const QString& name : selectionStrings) {
        _selectedClusters.insert(name);
    }

    for (const QVariant& clusterItem : clusters) {
        QVariantMap cluster = clusterItem.toMap();
        QString name = cluster["name"].toString();
        int count = cluster["count"].toInt();
        QString color = cluster["color"].toString();

        // Create a custom widget for the legend row
        QWidget* legendRow = new QWidget(containerWidget);
        legendRow->setFixedHeight(itemHeight);
        legendRow->setStyleSheet("border: none;");
        QHBoxLayout* rowLayout = new QHBoxLayout(legendRow);
        rowLayout->setSpacing(5);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        // Add a color box
        QLabel* colorBox = new QLabel(legendRow);
        colorBox->setFixedSize(12, 12);
        colorBox->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(color));

        // Add the text label
        QLabel* textLabel = new QLabel(QString("%1 (%2)").arg(name).arg(count), legendRow);
        if (selectionStrings.contains(name)) {
            textLabel->setStyleSheet(QString("color: %1; font-size: 13px;").arg(colorSelection.name()));
        } else {
            textLabel->setStyleSheet("font-size: 12px; color: black;");
        }

        // Add the color box and label to the row
        rowLayout->addWidget(colorBox);
        rowLayout->addWidget(textLabel);

        // Add the legend row to the frame layout
        layout->addWidget(legendRow);
    }

    layout->addStretch();

    scrollArea->setWidget(containerWidget);

    // Add the scroll area to the legend widget's layout
    QVBoxLayout* mainLayout = new QVBoxLayout(_legendWidget);
    // Add title
    QLabel* titleLabel = new QLabel(_settingsAction.getColorSettingsHolder().getChartTitleAction().getString(), _legendWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: black;");  // Set text color to black
    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(scrollArea);
    _legendWidget->setLayout(mainLayout);

    // Restore previous scroll position
    scrollArea->verticalScrollBar()->setValue(scrollPos);
}


QColor ChartLegendViewPlugin::selectColor(double value, double min, double max) {
    QString colorScheme = _settingsAction.getColorSettingsHolder().getColorMapAction().getColorMap();
    bool mirror = _settingsAction.getColorSettingsHolder().getColorMapAction().getMirrorAction(ColorMapAction::Axis::X).isChecked();
    return selectColorVal(value, min, max, colorScheme, mirror);
}

void ChartLegendViewPlugin::convertPointDataAndUpdateChart()
{
    auto currentPointDataset = mv::data().getDataset<Points>(_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getId());
    if (!currentPointDataset.isValid() && currentPointDataset->getDataType() !=PointType)
    {

        clearChartFrame();
        //_dropWidget->setShowDropIndicator(true);
        return;
    }

    //qDebug() << "LegendPlugin::convertDataAndUpdateChart: Prepare payload";
    //_dropWidget->setShowDropIndicator(false);
    try {
        // Corrected visString to be a QVariantList
        std::vector<float> resultContainer(currentPointDataset->getNumPoints());
        auto dimensionNames = currentPointDataset->getDimensionNames();
        auto dimensionName = _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().getCurrentDimensionName();
        int dimensionIndex = getIndexOfQString(dimensionNames, dimensionName);

        if (dimensionIndex != -1) {
            currentPointDataset->extractDataForDimension(resultContainer, dimensionIndex);
        }
        else {
            qDebug() << "Dimension name not found in dimension names.";
            return;
        }
        if (resultContainer.empty())
        {
            qDebug() << "LegendPlugin::convertPointDataAndUpdateChart: No data found";
            return;
        }
        float min = *std::min_element(resultContainer.begin(), resultContainer.end());
        float max = *std::max_element(resultContainer.begin(), resultContainer.end());

        // Convert std::vector<float> to QVariantList
        QVariantList values;
        for (const auto& value : resultContainer) {
            values.append(value);
        }

        QVariantList visString = QVariantList{
            QVariantMap{
                {"type", "numerical"},
                {"values", values},
                {"min", min},
                {"max", max}
            }
        };

        //qDebug() << "LegendPlugin::convertDataAndUpdateChart: Send data from Qt cpp to D3 js";
        createNumericalChart(visString);
        //auto currentColorMap = _settingsAction.getColorSettingsHolder().getColorMapAction().getColorMap();
        //_settingsAction.getColorSettingsHolder().getColorMapAction().setColorMap("");
        //_settingsAction.getColorSettingsHolder().getColorMapAction().setColorMap(currentColorMap);
    }
    catch (const std::exception& e) {
        qCritical() << "Exception caught in convertPointDataAndUpdateChart:" << e.what();
    }
    catch (...) {
        qCritical() << "Unknown exception caught in convertPointDataAndUpdateChart";
    }
}
void ChartLegendViewPlugin::updateChart()
{
    auto type = _settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getDataType();
    if (!_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().isValid())
    {
        clearChartFrame();
        return;
}

    if (type==PointType || type ==ClusterType)
    {
        if (type == PointType)
        {
            
            {

                auto data = mv::data().getDataset<Points>(_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getId());
                _settingsAction.getColorSettingsHolder().getDimensionSelectionAction().setPointsDataset(data);
                //_settingsAction.getColorSettingsHolder().getDimensionSelectionAction().setCurrentDimensionIndex(0);
                //_settingsAction.getColorSettingsHolder().getDimSelectionString().setString("");
                //_settingsAction.getColorSettingsHolder().getDimSelectionString().setString(dimensionNames[0]);

            }
            
            
            convertPointDataAndUpdateChart();
     
        }
        else if (type == ClusterType)
        {
            

            auto data = mv::data().getDataset<Clusters>(_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getId());
            auto clusters = data->getClusters();
            auto options = _settingsAction.getColorSettingsHolder().getSelectionStringList().getSelectedOptions();
            auto clusterNames = data->getClusterNames();
            QStringList stringList = convertToQStringList(clusterNames);
            _settingsAction.getColorSettingsHolder().getSelectionStringList().setOptions(stringList);
            _settingsAction.getColorSettingsHolder().getSelectionStringList().setSelectedOptions(options);
            convertClusterDataAndUpdateChart();
        }  
        else
        {
            clearChartFrame();
        }



      }


}
void ChartLegendViewPlugin::convertClusterDataAndUpdateChart()
{

    auto currentClusterDataset = mv::data().getDataset<Clusters>(_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset()->getId());
    if (!currentClusterDataset.isValid() && currentClusterDataset->getDataType() != ClusterType)
    {

        clearChartFrame();
        //_dropWidget->setShowDropIndicator(true);
        return;
    }

    //qDebug() << "LegendPlugin::convertDataAndUpdateChart: Prepare payload";
    //_dropWidget->setShowDropIndicator(false);
    try {
        // Corrected visString to be a QVariantList
        const auto& clustersValues = currentClusterDataset->getClusters();
        QVariantList clusterData;
        if (clustersValues.empty())
        {
            qWarning() << "LegendPlugin::convertClusterDataAndUpdateChart: No clusters found";
            return;
        }
        for (const auto& clusterValue : clustersValues)
        {
            QVariantMap clusterMap;
            auto clusterName = clusterValue.getName();
            auto clusterColor = clusterValue.getColor().name();
            auto     clusterIndicesCount = clusterValue.getNumberOfIndices();
            clusterMap.insert("name", clusterName);
            clusterMap.insert("color", clusterColor);
            clusterMap.insert("count", clusterIndicesCount);
            clusterData.append(clusterMap);
        }
        std::sort(clusterData.begin(), clusterData.end(), [](const QVariant& a, const QVariant& b) {
            return a.toMap().value("count").toInt() > b.toMap().value("count").toInt();
            });
        QVariantList visString = QVariantList{
            QVariantMap{
                {"type", "categorical"},
                {"clusters", clusterData}
            }
        };

        //qDebug() << "LegendPlugin::convertDataAndUpdateChart: Send data from Qt cpp to D3 js";
        createCategoricalChart(visString);
    }
    catch (const std::exception& e) {
        qCritical() << "Exception caught in convertClusterDataAndUpdateChart:" << e.what();
    }
    catch (...) {
        qCritical() << "Unknown exception caught in convertClusterDataAndUpdateChart";
    }
}

void ChartLegendViewPlugin::publishClusterSelection(const std::vector<unsigned int>& selectedIDs)
{

    auto parentColorDataset = mv::data().getDataset<Clusters>(_settingsAction.getColorSettingsHolder().getColorDatasetAction().getCurrentDataset().getDatasetId());

    parentColorDataset->setSelectionNames({});
    
    events().notifyDatasetDataSelectionChanged(parentColorDataset);
}


ChartLegendViewPluginFactory::ChartLegendViewPluginFactory()
{
    setIconByName("list");
    getPluginMetadata().setDescription("ChartLegend view plugin");
    getPluginMetadata().setSummary("This chartLegend shows how to implement a basic view plugin in ManiVault Studio.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "A. Vieth", { "Plugin developer", "Maintainer" }, { "LUMC", "TU Delft" } },
    	{ "T. Kroes", { "Lead software architect" }, { "LUMC" } },
        { "J. Thijssen", { "Software architect" }, { "LUMC", "TU Delft" } }
	});
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
	});
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}


ViewPlugin* ChartLegendViewPluginFactory::produce()
{
    return new ChartLegendViewPlugin(this);
}

mv::DataTypes ChartLegendViewPluginFactory::supportedDataTypes() const
{
    // This legend plugin is compatible with points datasets
    DataTypes supportedTypes;
    supportedTypes.append(PointType);
    supportedTypes.append(ClusterType);
    return supportedTypes;
}

mv::gui::PluginTriggerActions ChartLegendViewPluginFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this]() -> ChartLegendViewPlugin* {
        return dynamic_cast<ChartLegendViewPlugin*>(plugins().requestViewPlugin(getKind()));
        };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets == 1)
    {
        auto type = datasets.at(0)->getDataType();
        if (type == PointType || type == ClusterType)
        {
            auto pluginTriggerAction = new PluginTriggerAction(const_cast<ChartLegendViewPluginFactory*>(this), this, "Chart Legend View", "View JavaScript visualization", StyledIcon("list"), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance()->loadData(Datasets({ dataset }));

                });

        pluginTriggerActions << pluginTriggerAction;
        }
    }

    return pluginTriggerActions;
}
void ChartLegendViewPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);
    mv::util::variantMapMustContain(variantMap, "Legend:Settings");
    _settingsAction.fromVariantMap(variantMap["Legend:Settings"].toMap());
}

QVariantMap ChartLegendViewPlugin::toVariantMap() const
{
    QVariantMap variantMap = ViewPlugin::toVariantMap();

    _settingsAction.insertIntoVariantMap(variantMap);
    return variantMap;
}