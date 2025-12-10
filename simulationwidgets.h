#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QVector>
#include <QMouseEvent>
#include <QWheelEvent>
#include "prey.h"
#include "predator.h"
#include "water.h"
#include "bush.h"

class SimulationWidget : public QGraphicsView
{
    Q_OBJECT

public:
    enum AddMode {
        NONE,
        ADD_PREY,
        ADD_PREDATOR
    };

    SimulationWidget(QWidget *parent = nullptr);
    ~SimulationWidget();

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void addPrey(int count = 1);
    void addPredator(int count = 1);
    void setAddPreyMode() { m_addMode = ADD_PREY; }
    void setAddPredatorMode() { m_addMode = ADD_PREDATOR; }
    void zoomIn();
    void zoomOut();
    void resetZoom();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private slots:
    void updateSimulation();

private:
    void initializeSimulation();
    void setupEnvironment();
    void removeDeadOrganisms();
    void reproduceOrganisms();
    void updateStatistics();
    void addOrganismAtPosition(Organism::OrganismType type, QPointF position);
    void updateSceneGraphics();

    // Grafika
    QGraphicsScene *m_scene;

    // Organizmy
    QVector<Prey*> m_prey;
    QVector<Predator*> m_predators;

    // Środowisko
    QVector<WaterSource*> m_waterSources;
    QVector<Bush*> m_bushes;

    // Timer i stan
    QTimer *m_timer;
    bool m_isRunning;

    // Statystyki
    QVector<int> m_preyHistory;
    QVector<int> m_predatorHistory;
    int m_maxPopulation;
    int m_currentGeneration;

    // Parametry
    float m_foodRegenerationRate;
    float m_preyReproductionRate;
    float m_predatorReproductionRate;
    float m_energyConsumptionRate;
    float m_waterConsumptionRate;
    float m_mutationRate;

    // Kontrola
    AddMode m_addMode;

    // Wymiary
    int m_worldWidth;
    int m_worldHeight;
    float m_zoomLevel;

    // Grafika środowiska
    QVector<QGraphicsEllipseItem*> m_waterGraphics;
    QVector<QGraphicsEllipseItem*> m_bushGraphics;
};

#endif // SIMULATIONWIDGET_H
