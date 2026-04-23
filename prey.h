#ifndef PREY_H
#define PREY_H

#include "organism.h"
#include <QRandomGenerator>

class Herd;

class Prey : public Organism
{
    Q_OBJECT

public:
    Prey(QPointF position = QPointF(0, 0),
         float speed = 1.0f,
         float size = 5.0f,
         float vision = 50.0f,
         QObject *parent = nullptr);
    Prey(const Prey &parent1, const Prey &parent2);

    void update() override;
    Organism* reproduce() override;
    void updateAI() override;
    bool canSwim() const { return m_canSwim; }

    void setHerd(Herd* herd) { m_herd = herd; }
    Herd* getHerd() const { return m_herd; }
    void followAlpha();
    bool isAlpha() const;

    using Organism::findNearestBush;
    using Organism::findNearestWater;

    void moveTowardsTarget(const QPointF& target);

private:
    void updateNeeds();
    void decideState();
    void executeState();

    Organism* m_mateTarget;
    bool m_canSwim;
    Herd* m_herd;
    float m_herdAttraction;

    Organism* findNearestMate();
    Organism* findNearestPredator();
    QPointF findFood();
    QPointF getHerdCenter();
    void avoidPredators();
};

#endif // PREY_H
