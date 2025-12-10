#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"
#include <QVector>
#include "prey.h"

class WaterSource;
class Bush;

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

    // Środowisko
    void setAvailablePrey(const QVector<Prey*> &prey) { m_availablePrey = prey; }

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

    // Pomocnicze
    Prey* findNearestPrey();
    Organism* giveBirth();

private:
    QPointF m_direction;
    QVector<Prey*> m_availablePrey;

    // Dodatkowe dla drapieżników
    int m_huntCooldown;
    bool m_isHunting;
    Prey* m_currentTarget;  // Tylko w Predator!
};

#endif // PREDATOR_H
