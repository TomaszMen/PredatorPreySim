#include "mainwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Symulacja Drapieżnik-Ofiara z Ewolucją");
    setGeometry(100, 100, 1200, 700);

    // Główne okno symulacji
    m_simulationWidget = new SimulationWidget(this);
    setCentralWidget(m_simulationWidget);

    // Panel boczny
    m_sidePanel = new SidePanel(this);
    m_dockWidget = new QDockWidget("Panel Kontrolny", this);
    m_dockWidget->setWidget(m_sidePanel);
    m_dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, m_dockWidget);

    // Menu
    QMenu *fileMenu = menuBar()->addMenu("Plik");
    fileMenu->addAction("Nowa symulacja", this, [this]() { m_simulationWidget->resetSimulation(); });
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Zamknij");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu *viewMenu = menuBar()->addMenu("Widok");
    viewMenu->addAction(m_dockWidget->toggleViewAction());

    QMenu *simulationMenu = menuBar()->addMenu("Symulacja");
    simulationMenu->addAction("Start", this, [this]() { m_simulationWidget->startSimulation(); });
    simulationMenu->addAction("Pauza", this, [this]() { m_simulationWidget->pauseSimulation(); });
    simulationMenu->addAction("Reset", this, [this]() { m_simulationWidget->resetSimulation(); });

    // Połączenia sygnałów
    connect(m_sidePanel, &SidePanel::simulationParametersChanged,
            this, &MainWindow::handleParametersChanged);
    connect(m_sidePanel, &SidePanel::addPreyRequested,
            m_simulationWidget, &SimulationWidget::addPrey);
    connect(m_sidePanel, &SidePanel::addPredatorRequested,
            m_simulationWidget, &SimulationWidget::addPredator);
    connect(m_sidePanel, &SidePanel::clearAllRequested,
            m_simulationWidget, &SimulationWidget::resetSimulation);

    statusBar()->showMessage("Gotowy do symulacji");
}

void MainWindow::handleParametersChanged(float preyReproduction, float predatorReproduction,
                                         float foodRegeneration, float energyConsumption,
                                         float mutationRate, int initialPrey, int initialPredators)
{
    Q_UNUSED(preyReproduction);
    Q_UNUSED(predatorReproduction);
    Q_UNUSED(foodRegeneration);
    Q_UNUSED(energyConsumption);
    Q_UNUSED(mutationRate);
    Q_UNUSED(initialPrey);
    Q_UNUSED(initialPredators);

    // W przyszłości można tu dodać aktualizację parametrów symulacji
    statusBar()->showMessage("Parametry zaktualizowane");
}
