#ifndef ORGANISM_H
#define ORGANISM_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QtMath>
#include <algorithm>
#include <QVector>
#include "environment.h"

class Organism : public QObject
{
    Q_OBJECT

public:
    enum OrganismType { PREY, PREDATOR };
    enum State { WANDERING, HUNTING, FLEEING, EATING, DRINKING, MATING, RESTING };

    Organism(QObject *parent = nullptr);
    virtual ~Organism();

    QPointF position() const { return m_position; }
    QColor color() const { return m_color; }
    OrganismType type() const { return m_type; }
    float energy() const { return m_energy; }
    float speed() const { return m_speed; }
    float size() const { return m_size; }
    float visionRange() const { return m_visionRange; }
    int age() const { return m_age; }
    float hydration() const { return m_hydration; }
    State state() const { return m_state; }
    QPointF direction() const { return m_direction; }

    void setPosition(const QPointF &pos) { m_position = pos; }
    void setEnergy(float energy) { m_energy = std::min(200.0f, std::max(0.0f, energy)); }
    void setHydration(float hydration) { m_hydration = std::min(100.0f, std::max(0.0f, hydration)); }

    virtual void update() = 0;
    virtual Organism* reproduce() = 0;
    virtual void updateAI() = 0;

    void setAvailablePrey(const QVector<Organism*> &prey) { m_availablePrey = prey; }
    void setAvailablePredators(const QVector<Organism*> &predators) { m_availablePredators = predators; }
    void setEnvironment(const QVector<Environment*> &environment) { m_environment = environment; }

    bool canMate() const {
        return m_energy > 120 && m_hydration > 60 &&
               m_age > 50 && m_age < 500 &&
               !m_justReproduced;
    }

    bool isInWater() const;

protected:
    QPointF m_position;
    QColor m_color;
    OrganismType m_type;
    float m_energy;
    float m_speed;
    float m_size;
    float m_visionRange;
    float m_hydration;
    int m_age;
    State m_state;
    bool m_justReproduced;
    int m_reproductionCooldown;

    // Genetyczne cechy
    float m_speedGene;
    float m_sizeGene;
    float m_visionGene;
    float m_reproductionRateGene;
    float m_hydrationRateGene;

    // Listy otoczenia
    QVector<Organism*> m_availablePrey;
    QVector<Organism*> m_availablePredators;
    QVector<Environment*> m_environment;

    // AI pomocnicze
    QPointF m_direction;
    Organism* m_currentTarget;
    Environment* m_currentEnvironmentTarget;
    int m_stateTimer;

    // Potrzeby
    float m_fearLevel;
    float m_hungerLevel;
    float m_thirstLevel;
    float m_matingUrge;

    // Czas od ostatnich czynności
    float m_lastMealTime;
    float m_lastDrinkTime;
    float m_lastKillTime;

    void mutateGenes();
    QPointF avoidWater();
    QPointF seekWater();
    Environment* findNearestWater();
    Environment* findNearestBush();
    Organism* findNearestPrey();
    Organism* findNearestMate();
    Organism* findNearestPredator();
    float calculateFitness() const;

    void moveTowards(const QPointF &target, float weight = 1.0f);
    void moveAwayFrom(const QPointF &target, float weight = 1.0f);
    void wander();

    void applyBoundaries();
};

#endif // ORGANISM_H
