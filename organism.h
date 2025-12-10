#ifndef ORGANISM_H
#define ORGANISM_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QtMath>
#include <algorithm>
#include <QVector>

class WaterSource;
class Bush;
class Prey;

class Organism : public QObject
{
    Q_OBJECT

public:
    enum OrganismType { PREY, PREDATOR };
    enum OrganismState {
        IDLE,
        SEEKING_FOOD,
        SEEKING_WATER,
        SEEKING_MATE,
        SLEEPING,
        FLEEING,
        RESTING,
        PREGNANT,
        CARING_FOR_YOUNG
    };
    enum AgeStage { BABY, YOUNG, ADULT, ELDER };

    Organism(QObject *parent = nullptr);
    virtual ~Organism();

    // Gettery
    QPointF position() const { return m_position; }
    QColor color() const { return m_color; }
    OrganismType type() const { return m_type; }
    float energy() const { return m_energy; }
    float speed() const { return m_speed; }
    float size() const { return m_size; }
    float visionRange() const { return m_visionRange; }
    int age() const { return m_age; }
    AgeStage ageStage() const { return m_ageStage; }
    OrganismState state() const { return m_currentState; }
    float thirst() const { return m_thirst; }
    float hunger() const { return m_hunger; }
    float fatigue() const { return m_fatigue; }
    float reproductionUrge() const { return m_reproductionUrge; }
    bool isPregnant() const { return m_pregnancyTimer > 0; }
    int pregnancyProgress() const { return m_pregnancyTimer; }
    bool hasPartner() const { return m_partner != nullptr; }
    QVector<Organism*> children() const { return m_children; }

    // Settery
    void setPosition(const QPointF &pos) { m_position = pos; }
    void setEnergy(float energy) { m_energy = energy; }
    void setThirst(float thirst) { m_thirst = thirst; }
    void setHunger(float hunger) { m_hunger = hunger; }
    void setFatigue(float fatigue) { m_fatigue = fatigue; }
    void setReproductionUrge(float urge) { m_reproductionUrge = urge; }
    void setPartner(Organism* partner) { m_partner = partner; }
    void setTimeSinceLastReproduction(int time) { m_timeSinceLastReproduction = time; }
    void addChild(Organism* child) { m_children.append(child); }

    // Środowisko
    void setAvailableWater(const QVector<WaterSource*> &water) { m_availableWater = water; }
    void setAvailableBushes(const QVector<Bush*> &bushes) { m_availableBushes = bushes; }
    void setAvailableMates(const QVector<Organism*> &mates) { m_availableMates = mates; }
    void setNearbyPredators(const QVector<Organism*> &predators) { m_nearbyPredators = predators; }

    // Podstawowe metody
    virtual void update() = 0;
    virtual Organism* reproduce() = 0;

    // Metody behawioralne
    void drink(float amount);
    void eat(float amount);
    void rest(float amount);
    void ageOneStep();
    void updateNeeds();
    bool canReproduce() const;
    void startPregnancy();
    void updatePregnancy();
    void updateAgeStage();

    // Metody wirtualne do zaimplementowania
    virtual void decideNextAction() = 0;
    virtual QPointF calculateMovement() = 0;
    virtual QPointF seekWater() = 0;
    virtual QPointF seekFood() = 0;
    virtual QPointF seekMate() = 0;
    virtual QPointF fleeFromDanger() = 0;
    virtual void interactWithEnvironment() = 0;
    virtual void updateColor() = 0;

signals:
    void organismDied(Organism* organism);
    void organismReproduced(Organism* parent, Organism* offspring);
    void organismStateChanged(Organism* organism, OrganismState newState);

protected:
    QPointF m_position;
    QColor m_color;
    OrganismType m_type;
    OrganismState m_currentState;
    AgeStage m_ageStage;

    // Podstawowe atrybuty
    float m_energy;
    float m_speed;
    float m_size;
    float m_visionRange;
    int m_age;

    // Potrzeby
    float m_hunger;
    float m_thirst;
    float m_fatigue;
    float m_reproductionUrge;

    // Reprodukcja i rodzina
    int m_pregnancyTimer;
    int m_pregnancyDuration;
    Organism* m_partner;
    QVector<Organism*> m_children;
    int m_timeSinceLastReproduction;

    // Genetyczne cechy
    float m_speedGene;
    float m_sizeGene;
    float m_visionGene;
    float m_reproductionRateGene;
    float m_metabolismGene;
    float m_staminaGene;

    // Środowisko
    QVector<WaterSource*> m_availableWater;
    QVector<Bush*> m_availableBushes;
    QVector<Organism*> m_availableMates;
    QVector<Organism*> m_nearbyPredators;

    // Metody pomocnicze
    void mutateGenes();
};

#endif // ORGANISM_H
