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

private:
    QPointF findFood();
    void avoidPredators();
    QPointF m_direction;
};

#endif // PREY_H
