#include "SettingsAction.h"

#include <iostream>
#include <set>
#include "ChartLegendViewPlugin.h"
#include<string>  
#include <QFileDialog>
#include <QPageLayout>
#include <QWebEngineView>
#include <string>
#include <typeinfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QSet>
#include <QJsonValue>
using namespace mv;
using namespace mv::gui;

SettingsAction::SettingsAction(ChartLegendViewPlugin& ChartLegendViewPlugin, mv::CoreInterface* core) :
    WidgetAction(&ChartLegendViewPlugin, "ChartLegendViewPlugin Chart"),
    _viewerPlugin(ChartLegendViewPlugin),
    _core(core),
    _colorSettingsHolder(*this)

{
    setSerializationName("Legend:Settings");
    _colorSettingsHolder.getColorDatasetAction().setSerializationName("Legend:ColorDataset");
    _colorSettingsHolder.getDimensionSelectionAction().setSerializationName("Legend:DimensionSelection");
    _colorSettingsHolder.getColorMapAction().setSerializationName("Legend:Colormap");
    _colorSettingsHolder.getClusterSelectionString().setSerializationName("Legend:ClusterSelectionString");
    _colorSettingsHolder.getDimSelectionString().setSerializationName("Legend:DimSelectionString");
    _colorSettingsHolder.getSelectionStringList().setSerializationName("Legend:SelectionStringList");
    _colorSettingsHolder.getSelectionColorAction().setSerializationName("Legend:SelectionColorAction");
    _colorSettingsHolder.getDelimiterString().setSerializationName("Legend:Delimiter");
    _colorSettingsHolder.getConstantColorAction().setSerializationName("Legend:ConstantColorAction");
    _colorSettingsHolder.getColorMapAction().initialize("Viridis");
    _colorSettingsHolder.getChartTitleAction().setSerializationName("Legend:ChartTitle");


    _colorSettingsHolder.getColorDatasetAction().setToolTip("ColorDataset");
    _colorSettingsHolder.getDimensionSelectionAction().setToolTip("DimensionSelection");
    _colorSettingsHolder.getClusterSelectionString().setToolTip("ClusterSelectionString");
    _colorSettingsHolder.getDimSelectionString().setToolTip("DimSelectionString");
    _colorSettingsHolder.getSelectionColorAction().setToolTip("SelectionColorAction");
    _colorSettingsHolder.getSelectionStringList().setToolTip("SelectionStringList");
    _colorSettingsHolder.getColorMapAction().setToolTip("Colormap");
    _colorSettingsHolder.getDelimiterString().setToolTip("Delimiter");
    _colorSettingsHolder.getDelimiterString().setString(",");
    _colorSettingsHolder.getConstantColorAction().setToolTip("ConstantColorAction");
    _colorSettingsHolder.getChartTitleAction().setToolTip("ChartTitle");
    _colorSettingsHolder.getChartTitleAction().setString("Chart Title");


    _colorSettingsHolder.getColorDatasetAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _colorSettingsHolder.getDimensionSelectionAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _colorSettingsHolder.getClusterSelectionString().setDefaultWidgetFlags(StringAction::LineEdit);
    _colorSettingsHolder.getDimSelectionString().setDefaultWidgetFlags(StringAction::LineEdit);
    _colorSettingsHolder.getSelectionStringList().setDefaultWidgetFlags(OptionsAction::ComboBox);
    _colorSettingsHolder.getSelectionColorAction().setDefaultWidgetFlags(ColorAction::Picker);
    _colorSettingsHolder.getDelimiterString().setDefaultWidgetFlags(StringAction::LineEdit);
    _colorSettingsHolder.getChartTitleAction().setDefaultWidgetFlags(StringAction::LineEdit);
    _colorSettingsHolder.getConstantColorAction().setDefaultWidgetFlags(ColorAction::Picker);
    _colorSettingsHolder.getSelectionColorAction().setColor(QColor(0, 162, 237, 255));

    _colorSettingsHolder.getColorMapAction().setDefaultWidgetFlags(ColorMap1DAction::Default);

}

inline SettingsAction::ColorSettingsHolder::ColorSettingsHolder(SettingsAction& settingsAction) :
    VerticalGroupAction(&settingsAction, "Cluster data options"),
    _settingsOptions(settingsAction),
    _colorDatasetAction(this, "Cluster dataset"),
    _clusterSelectionString(this, "Cluster Selection string"),
    _selectionColorAction(this, "Selection color"),
    _selectionStringList(this, "Selection string list"),
    _delimiterString(this, "Delimiter"),
    _dimensionSelectionAction(this, "Select dimension"),
    _constantColorAction(this, "Point dataset"),
    _colorMapAction(this, "Color map"),
    _dimSelectionString(this, "Dimension Selection string"),
    _chartTitleAction(this, "Chart Title")
{
    setText("Color Options");
    setIcon(mv::util::StyledIcon("braille"));
    setPopupSizeHint(QSize(500, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);
    addAction(&_colorDatasetAction);
    addAction(&_selectionStringList);
    addAction(&_clusterSelectionString);
    addAction(&_delimiterString);
    addAction(&_selectionColorAction);
    addAction(&_dimensionSelectionAction);
    addAction(&_dimSelectionString);
    addAction(&_colorMapAction);
}


void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _colorSettingsHolder.getColorDatasetAction().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getDimensionSelectionAction().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getColorMapAction().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getClusterSelectionString().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getDimSelectionString().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getSelectionStringList().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getSelectionColorAction().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getDelimiterString().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getChartTitleAction().fromParentVariantMap(variantMap);
    _colorSettingsHolder.getConstantColorAction().fromParentVariantMap(variantMap);

}

QVariantMap SettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();
    _colorSettingsHolder.getColorDatasetAction().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getDimensionSelectionAction().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getColorMapAction().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getClusterSelectionString().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getDimSelectionString().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getSelectionStringList().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getSelectionColorAction().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getDelimiterString().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getChartTitleAction().insertIntoVariantMap(variantMap);
    _colorSettingsHolder.getConstantColorAction().insertIntoVariantMap(variantMap);



    return variantMap;
}