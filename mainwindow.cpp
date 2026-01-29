#include "mainwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Symulacja Ekosystemu: Drapieżnik-Ofiara z Ewolucją");
    setGeometry(100, 100, 1400, 800);

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
    viewMenu->addSeparator();
    viewMenu->addAction("Zoom +", this, [this]() { /* Można dodać */ });
    viewMenu->addAction("Zoom -", this, [this]() { /* Można dodać */ });
    viewMenu->addAction("Resetuj widok", this, [this]() { /* Można dodać */ });

    QMenu *simulationMenu = menuBar()->addMenu("Symulacja");
    simulationMenu->addAction("Start", this, [this]() { m_simulationWidget->startSimulation(); });
    simulationMenu->addAction("Pauza", this, [this]() { m_simulationWidget->pauseSimulation(); });
    simulationMenu->addAction("Reset", this, [this]() { m_simulationWidget->resetSimulation(); });
    simulationMenu->addSeparator();
    simulationMenu->addAction("Dodaj 10 ofiar", this, [this]() { m_simulationWidget->addPrey(10); });
    simulationMenu->addAction("Dodaj 3 drapieżniki", this, [this]() { m_simulationWidget->addPredator(3); });

    QMenu *helpMenu = menuBar()->addMenu("Pomoc");
    helpMenu->addAction("O programie", this, []() {
        QMessageBox::about(nullptr, "O programie",
                           "Symulacja ekosystemu drapieżnik-ofiara z ewolucją\n\n"
                           "Funkcje:\n"
                           "- Realistyczna AI zwierząt\n"
                           "- System głodu i pragnienia\n"
                           "- Woda i roślinność\n"
                           "- Ewolucja cech\n"
                           "- Statystyki i wykresy\n"
                           "- Poruszalna kamera\n\n"
                           "Sterowanie:\n"
                           "- LPM: przesuwanie kamery\n"
                           "- Kółko myszy: przybliżanie/oddalanie\n"
                           "- Przyciski: kontrola symulacji");
    });

    // Połączenia sygnałów
    connect(m_sidePanel, &SidePanel::simulationParametersChanged,
            this, &MainWindow::handleParametersChanged);
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

    statusBar()->showMessage("Gotowy do symulacji. Użyj myszy do przesuwania i zoomowania.");
}

void MainWindow::handleParametersChanged(float preyReproduction, float predatorReproduction,
                                         float foodRegeneration, float energyConsumption,
                                         float mutationRate, int initialPrey, int initialPredators)
{
    m_simulationWidget->setSimulationParameters(preyReproduction, predatorReproduction,
                                                foodRegeneration, energyConsumption,
                                                mutationRate, initialPrey, initialPredators);
    statusBar()->showMessage("Parametry zaktualizowane");
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

    // Aktualizuj status bar
    statusBar()->showMessage(QString("Tick: %1 | Ofiary: %2 | Drapieżniki: %3 | Urodzenia: %4 | Zgony: %5")
                                 .arg(generation).arg(preyCount).arg(predatorCount).arg(births).arg(deaths));
}

void MainWindow::handleHistoryUpdated(const QVector<int> &preyHistory, const QVector<int> &predatorHistory)
{
    m_sidePanel->updateCharts(preyHistory, predatorHistory);
}
