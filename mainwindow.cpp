#include "mainwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Symulacja Ekosystemu: Drapieznik-Ofiara z Ewolucja");
    setGeometry(100, 100, 1400, 800);

    // Glowne okno symulacji
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
    fileMenu->addAction("Nowa symulacja", this, [this]() {
        m_simulationWidget->resetSimulation();
    });
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Zamknij");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu *viewMenu = menuBar()->addMenu("Widok");
    viewMenu->addAction(m_dockWidget->toggleViewAction());

    QMenu *simulationMenu = menuBar()->addMenu("Symulacja");
    simulationMenu->addAction("Start", this, [this]() {
        m_simulationWidget->startSimulation();
    });
    simulationMenu->addAction("Pauza", this, [this]() {
        m_simulationWidget->pauseSimulation();
    });
    simulationMenu->addAction("Reset", this, [this]() {
        m_simulationWidget->resetSimulation();
    });
    simulationMenu->addSeparator();
    simulationMenu->addAction("Dodaj 10 ofiar", this, [this]() {
        m_simulationWidget->addPrey(10);
    });
    simulationMenu->addAction("Dodaj 3 drapiezniki", this, [this]() {
        m_simulationWidget->addPredator(3);
    });

    QMenu *helpMenu = menuBar()->addMenu("Pomoc");
    helpMenu->addAction("O programie", this, []() {
        QMessageBox::about(nullptr, "O programie",
                           "Symulacja ekosystemu drapieznik-ofiara z ewolucja\n\n"
                           "Sterowanie:\n"
                           "- LPM: przesuwanie kamery\n"
                           "- Kolko myszy: przyblizanie/oddalanie");
    });

    // Polaczenia sygnalow
    connect(m_sidePanel, &SidePanel::simulationParametersChanged,
            this, &MainWindow::handleParametersChanged);

    connect(m_sidePanel, &SidePanel::restartSimulationRequested,
            this, [this]() {
                // Ta lambda zostanie wywolana po nacisnieciu przycisku
                // Parametry sa juz przekazane przez simulationParametersChanged
            });

    connect(m_sidePanel, &SidePanel::addPreyRequested,
            m_simulationWidget, &SimulationWidget::addPrey);
    connect(m_sidePanel, &SidePanel::addPredatorRequested,
            m_simulationWidget, &SimulationWidget::addPredator);
    connect(m_sidePanel, &SidePanel::clearAllRequested,
            m_simulationWidget, &SimulationWidget::resetSimulation);
    connect(m_sidePanel, &SidePanel::addBushRequested,
            m_simulationWidget, &SimulationWidget::addBush);
    connect(m_sidePanel, &SidePanel::addWaterRequested,
            m_simulationWidget, &SimulationWidget::addWater);

    connect(m_simulationWidget, &SimulationWidget::statisticsUpdated,
            this, &MainWindow::handleStatisticsUpdated);
    connect(m_simulationWidget, &SimulationWidget::historyUpdated,
            this, &MainWindow::handleHistoryUpdated);

    statusBar()->showMessage("Gotowy do symulacji. Uzyj myszy do przesuwania i zoomowania.");
}

void MainWindow::handleParametersChanged(
    float foodRegenMult, int bushFoodLimit,
    float predatorEnergyMult, float preyEnergyMult,
    int initialPrey, int initialPredators,
    float predatorVisionMult, float preyVisionMult)
{
    m_simulationWidget->applyAndRestartSimulation(
        foodRegenMult, bushFoodLimit,
        predatorEnergyMult, preyEnergyMult,
        initialPrey, initialPredators,
        predatorVisionMult, preyVisionMult
        );
    statusBar()->showMessage("Symulacja zrestartowana z nowymi parametrami");
}

void MainWindow::handleStatisticsUpdated(int preyCount, int predatorCount, int generation,
                                         float avgPreySpeed, float avgPredatorSpeed,
                                         float avgPreySize, float avgPredatorSize,
                                         int births, int deaths)
{
    m_sidePanel->updateStatistics(preyCount, predatorCount, generation,
                                  avgPreySpeed, avgPredatorSpeed,
                                  avgPreySize, avgPredatorSize,
                                  births, deaths);

    statusBar()->showMessage(QString("Tick: %1 | Ofiary: %2 | Drapiezniki: %3 | Urodzenia: %4 | Zgony: %5")
                                 .arg(generation).arg(preyCount).arg(predatorCount).arg(births).arg(deaths));
}

void MainWindow::handleHistoryUpdated(const QVector<int> &preyHistory, const QVector<int> &predatorHistory)
{
    m_sidePanel->updateCharts(preyHistory, predatorHistory);
}
