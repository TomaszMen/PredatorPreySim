#ifndef PREY_H
#define PREY_H

#include "organism.h"
#include <QRandomGenerator>
#include <QVector>

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

    // Wirtualne metody z Organism
    void update() override;
    Organism* reproduce() override;

    // Metody AI
    void decideNextAction() override;
    QPointF calculateMovement() override;
    QPointF seekWater() override;
    QPointF seekFood() override;
    QPointF seekMate() override;
    QPointF fleeFromDanger() override;
    void interactWithEnvironment() override;
    void updateColor() override;

    // Zachowania stadne
    void followGroup();
    QPointF calculateGroupCenter();
    bool isTooFarFromGroup();

private:
    // Zmienne specyficzne dla Prey
    QPointF m_direction;
    QVector<Prey*> m_groupMembers;
    QPointF m_sleepingSpot;
    bool m_isAsleep;
    int m_sleepTimer;

    // Metody pomocnicze
    QPointF findSafePlace();
    void updateSocialBehavior();
    void updateReproductionCycle();
    void updateNeedsInternal();
    void findNearbyGroupMembers();
    Organism* giveBirth();
};

#endif // PREY_H
