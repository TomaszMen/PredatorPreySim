#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"
#include <QVector>

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
    void updateAI() override;

private:
    void updateNeeds();
    void decideState();
    void executeState();

    // Specyficzne dla Predator
    Organism* m_mateTarget;

    // Umiejętności
    bool m_canSwim;

    // Pomocnicze metody
    Organism* findNearestMate();
    Organism* findNearestPrey();
};

#endif // PREDATOR_H
