#pragma once

#include <ViewPlugin.h>
#include <QWidget>
#include <Dataset.h>
#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>
#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <widgets/DropWidget.h>
#include "SettingsAction.h"

/** All plugin related classes are in the ManiVault plugin namespace */
using namespace mv::plugin;

/** Drop widget used in this plugin is located in the ManiVault gui namespace */
using namespace mv::gui;

/** Dataset reference used in this plugin is located in the ManiVault util namespace */
using namespace mv::util;

class QLabel;

class ChartLegendViewPlugin : public ViewPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     */
    ChartLegendViewPlugin(const PluginFactory* factory);

    /** Destructor */
    ~ChartLegendViewPlugin() override = default;
    
    /** This function is called by the core after the view plugin has been created */
    void init() override;

    /** Store a private reference to the data set that should be displayed */
    void loadData(const mv::Datasets& datasets) override;
    void initLoader();
    void clearChartFrame();
    void createNumericalChart(QVariantList chartData);
    void createCategoricalChart(QVariantList chartData);
    QColor selectColor(double value, double min, double max);

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;
public slots:
    /** Converts ManiVault's point data to a json-like data structure that Qt can pass to the JS code */
    void convertPointDataAndUpdateChart();
    void convertClusterDataAndUpdateChart();
    void updateChart();
private:
    /** Published selections received from the JS side to ManiVault's core */
    void publishClusterSelection(const std::vector<unsigned int>& selectedIDs);

    SettingsAction& getSettingsAction() { return _settingsAction; }
    VerticalToolbarAction _verticalToolbarAction;
    HorizontalToolbarAction _horizontalToolbarAction;

protected:
    DropWidget*             _dropWidget;                /** Widget for drag and drop behavior */
    SettingsAction            _settingsAction;
    QFrame*                   _legendWidget;
    QSet<QString>        _selectedClusters;
    Dataset<DatasetImpl>          _currentDataset;
};

/**
 * ChartLegend view plugin factory class
 *
 * Note: Factory does not need to be altered (merely responsible for generating new plugins when requested)
 */
class ChartLegendViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.ChartLegendViewPlugin"
                      FILE  "ChartLegendViewPlugin.json")

public:

    /** Default constructor */
    ChartLegendViewPluginFactory();

    /** Creates an instance of the chartLegend view plugin */
    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the chartLegend view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};
