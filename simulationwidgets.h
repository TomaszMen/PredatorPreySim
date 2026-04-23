#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <qmutex.h>
#include "prey.h"
#include "predator.h"
#include "environment.h"

class QPushButton;
class QVBoxLayout;
class Herd;

class SimulationWidget : public QWidget
{
    Q_OBJECT

public:
    SimulationWidget(QWidget *parent = nullptr);
    ~SimulationWidget();

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void addPrey(int count = 1);
    void addPredator(int count = 1);
    void addBush();
    void addWater();
    void setSimulationParameters(float preyReproduction, float predatorReproduction,
                                 float foodRegeneration, float energyConsumption,
                                 float mutationRate, int initialPrey, int initialPredators);
    void applyAndRestartSimulation(float foodRegenMult, int bushFoodLimit,
                                   float predatorEnergyMult, float preyEnergyMult,
                                   int initialPrey, int initialPredators,
                                   float predatorVisionMult, float preyVisionMult);

signals:
    void statisticsUpdated(int preyCount, int predatorCount, int generation,
                           float avgPreySpeed, float avgPredatorSpeed,
                           float avgPreySize, float avgPredatorSize,
                           int births, int deaths);
    void historyUpdated(const QVector<int> &preyHistory, const QVector<int> &predatorHistory);

    void evolutionDataUpdated(
        const QVector<float> &preySpeedHistory,
        const QVector<float> &predatorSpeedHistory,
        const QVector<float> &preySizeHistory,
        const QVector<float> &predatorSizeHistory,
        const QVector<float> &preyVisionHistory,
        const QVector<float> &predatorVisionHistory
        );

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateSimulation();

private:
    void initializeSimulation();
    void generateEnvironment();
    void removeDeadOrganisms();
    void reproduceOrganisms();
    void updateStatistics();
    void updateEnvironment();
    QPointF screenToWorld(const QPoint &screenPos) const;
    QPoint worldToScreen(const QPointF &worldPos) const;

    // Organizmy i środowisko
    QVector<Prey*> m_prey;
    QVector<Predator*> m_predators;
    QVector<Environment*> m_environment;

    // Kamera
    QPointF m_cameraPosition;
    float m_zoom;
    bool m_isDragging;
    QPoint m_lastMousePos;

    // Symulacja
    QTimer *m_timer;
    bool m_isRunning;
    int m_generation;

    // Statystyki
    QVector<int> m_preyHistory;
    QVector<int> m_predatorHistory;
    QVector<int> m_birthHistory;
    QVector<int> m_deathHistory;
    int m_totalBirths;
    int m_totalDeaths;
    int m_maxPopulation;

    // Parametry symulacji
    float m_foodRegenerationRate;
    float m_preyReproductionRate;
    float m_predatorReproductionRate;
    float m_energyConsumptionRate;
    float m_mutationRate;

    int m_initialPreyCount = 120;
    int m_initialPredatorCount = 15;

    float m_globalPreyReproductionFactor;
    float m_globalPredatorReproductionFactor;
    float m_globalEnergyConsumptionFactor;
    float m_globalMutationRate;

    // Kontrolki
    QPushButton *m_startButton;
    QPushButton *m_pauseButton;
    QPushButton *m_resetButton;

    QVector<Herd*> m_herds;

    void updateHerds();
    void assignPreyToHerds();
    bool m_isCleaningUp = false;

    mutable QMutex m_envMutex;
    QVector<Environment*> getEnvironmentSnapshot() const;
    void updateEnvironmentSafe();

    QVector<Prey*> m_pendingPreyRemoval;
    QVector<Predator*> m_pendingPredatorRemoval;
    QVector<Environment*> m_pendingEnvironmentRemoval;

    void processPendingRemovals();

    void addEnvironmentSafe(Environment* env);
    void removeEnvironmentSafe(Environment* env);
    QVector<Environment*> getEnvironmentCopy() const;

    QVector<float> m_preySpeedHistory;
    QVector<float> m_predatorSpeedHistory;
    QVector<float> m_preySizeHistory;
    QVector<float> m_predatorSizeHistory;
    QVector<float> m_preyVisionHistory;
    QVector<float> m_predatorVisionHistory;
};

#endif // SIMULATIONWIDGET_H
