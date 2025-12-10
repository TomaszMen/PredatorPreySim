#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include "prey.h"
#include "predator.h"

class QPushButton;
class QVBoxLayout;

class SimulationWidget : public QWidget
{
    Q_OBJECT

public:
    SimulationWidget(QWidget *parent = nullptr);
    ~SimulationWidget();

public slots:  // Zmienione z private slots na public slots
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void addPrey(int count = 1);
    void addPredator(int count = 1);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateSimulation();

private:
    void initializeSimulation();
    void removeDeadOrganisms();
    void reproduceOrganisms();
    void updateStatistics();

    QVector<Prey*> m_prey;
    QVector<Predator*> m_predators;

    QTimer *m_timer;
    bool m_isRunning;

    // Statystyki
    QVector<int> m_preyHistory;
    QVector<int> m_predatorHistory;
    int m_maxPopulation;

    // Parametry symulacji
    float m_foodRegenerationRate;
    float m_preyReproductionRate;
    float m_predatorReproductionRate;
    float m_energyConsumptionRate;

    QPushButton *m_startButton;
    QPushButton *m_pauseButton;
    QPushButton *m_resetButton;
};

#endif // SIMULATIONWIDGET_H
