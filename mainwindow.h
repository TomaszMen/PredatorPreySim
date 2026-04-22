#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include "simulationwidgets.h"
#include "sidepanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleParametersChanged(
        float foodRegenMult, int bushFoodLimit,
        float predatorEnergyMult, float preyEnergyMult,
        int initialPrey, int initialPredators,
        float predatorVisionMult, float preyVisionMult);
    void handleStatisticsUpdated(int preyCount, int predatorCount, int generation,
                                 float avgPreySpeed, float avgPredatorSpeed,
                                 float avgPreySize, float avgPredatorSize,
                                 int births, int deaths);
    void handleHistoryUpdated(const QVector<int> &preyHistory, const QVector<int> &predatorHistory);

private:
    SimulationWidget *m_simulationWidget;
    SidePanel *m_sidePanel;
    QDockWidget *m_dockWidget;
};

#endif // MAINWINDOW_H
