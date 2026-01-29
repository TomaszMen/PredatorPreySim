#ifndef PREY_H
#define PREY_H

#include "organism.h"
#include <QRandomGenerator>

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

private:
    void updateNeeds();
    void decideState();
    void executeState();

    // Specyficzne dla Prey
    Organism* m_mateTarget;

    // Umiejętności - DODANE TUTAJ
    bool m_canSwim;

    // Pomocnicze metody
    Organism* findNearestMate();
    Organism* findNearestPredator();
    QPointF findFood();
    void avoidPredators();
};

#endif // PREY_H
