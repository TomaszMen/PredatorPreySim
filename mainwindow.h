#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include "simulationwidgets.h"  // Dodaj tę linię
#include "sidepanel.h"         // Dodaj tę linię

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleParametersChanged(float preyReproduction, float predatorReproduction,
                                 float foodRegeneration, float energyConsumption,
                                 float mutationRate, int initialPrey, int initialPredators);

private:
    SimulationWidget *m_simulationWidget;
    SidePanel *m_sidePanel;
    QDockWidget *m_dockWidget;
};

#endif // MAINWINDOW_H
