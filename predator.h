#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"
#include <QVector>
#include "prey.h"

class Predator : public Organism
{
    Q_OBJECT

public:
    Predator(QPointF position = QPointF(0, 0),
             float speed = 1.5f,
             float size = 8.0f,
             float vision = 100.0f,
             QObject *parent = nullptr);
    Predator(const Predator &parent1, const Predator &parent2);

    void update() override;
    Organism* reproduce() override;

    void setAvailablePrey(const QVector<Prey*> &prey) { m_availablePrey = prey; }

private:
    Prey* findNearestPrey();
    QPointF m_direction;
    QVector<Prey*> m_availablePrey;
};

#endif // PREDATOR_H
