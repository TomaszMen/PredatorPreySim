#include "prey.h"
#include "water.h"
#include "bush.h"
#include <QtMath>
#include <QDebug>
#include <QLineF>
#include <QRandomGenerator>
#include <algorithm>

Prey::Prey(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
{
    m_position = position;
    m_type = PREY;
    m_color = QColor(0, 255, 0);
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_age = 0;
    m_ageStage = BABY;
    m_currentState = IDLE;

    // Inicjalizacja potrzeb
    m_hunger = 20.0f;
    m_thirst = 20.0f;
    m_fatigue = 0.0f;
    m_reproductionUrge = 0.0f;

    // Inicjalizacja genów
    m_speedGene = speed / 2.0f;
    m_sizeGene = size / 10.0f;
    m_visionGene = vision / 100.0f;
    m_reproductionRateGene = 1.0f;
    m_metabolismGene = 1.0f;
    m_staminaGene = 1.0f;

    // Reprodukcja
    m_pregnancyTimer = 0;
    m_pregnancyDuration = 100;
    m_partner = nullptr;
    m_timeSinceLastReproduction = 0;

    // Inicjalizacja zmiennych specyficznych dla Prey
    m_direction = QPointF(0, 0);
    m_sleepingSpot = QPointF(0, 0);
    m_isAsleep = false;
    m_sleepTimer = 0;
    m_groupMembers.clear();

    // Losowy kierunek początkowy
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

Prey::Prey(const Prey &parent1, const Prey &parent2)
    : Organism(nullptr)
{
    m_type = PREY;
    m_color = QColor(0, 255, 0);

    // Dziedziczenie cech
    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;
    m_metabolismGene = (parent1.m_metabolismGene + parent2.m_metabolismGene) / 2.0f;
    m_staminaGene = (parent1.m_staminaGene + parent2.m_staminaGene) / 2.0f;

    // Mutacja
    mutateGenes();

    // Oblicz cechy z genów
    m_speed = m_speedGene * 2.0f;
    m_size = m_sizeGene * 10.0f;
    m_visionRange = m_visionGene * 100.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_age = 0;
    m_ageStage = BABY;
    m_currentState = IDLE;

    // Potrzeby
    m_hunger = 20.0f;
    m_thirst = 20.0f;
    m_fatigue = 0.0f;
    m_reproductionUrge = 0.0f;

    // Reprodukcja
    m_pregnancyTimer = 0;
    m_pregnancyDuration = 100;
    m_partner = nullptr;
    m_timeSinceLastReproduction = 0;

    // Inicjalizacja zmiennych specyficznych dla Prey
    m_direction = QPointF(0, 0);
    m_sleepingSpot = QPointF(0, 0);
    m_isAsleep = false;
    m_sleepTimer = 0;
    m_groupMembers.clear();

    // Losowy kierunek
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Prey::update()
{
    if (m_energy <= 0) {
        emit organismDied(this);
        return;
    }

    // Aktualizacja wieku i potrzeb
    ageOneStep();
    updateNeeds();
    updateAgeStage();

    // Aktualizacja ciąży
    if (isPregnant()) {
        updatePregnancy();
    }

    // Aktualizacja snu
    if (m_isAsleep) {
        m_sleepTimer++;
        if (m_sleepTimer > 50) {
            m_isAsleep = false;
            m_sleepTimer = 0;
            m_currentState = IDLE;
        } else {
            m_fatigue -= 2.0f;
            if (m_fatigue < 0) m_fatigue = 0;
            return;
        }
    }

    // Decyzja o następnej akcji
    decideNextAction();

    // Wykonanie ruchu
    QPointF movement = calculateMovement();
    m_position += movement;

    // Sprawdzenie granic świata
    const int worldWidth = 2000;
    const int worldHeight = 2000;

    if (m_position.x() < 0) {
        m_position.setX(0);
        m_direction.setX(-m_direction.x());
    }
    if (m_position.x() > worldWidth) {
        m_position.setX(worldWidth);
        m_direction.setX(-m_direction.x());
    }
    if (m_position.y() < 0) {
        m_position.setY(0);
        m_direction.setY(-m_direction.y());
    }
    if (m_position.y() > worldHeight) {
        m_position.setY(worldHeight);
        m_direction.setY(-m_direction.y());
    }

    // Interakcja ze środowiskiem
    interactWithEnvironment();

    // Aktualizacja koloru na podstawie stanu
    updateColor();

    m_timeSinceLastReproduction++;
}

Organism* Prey::reproduce()
{
    // Reprodukcja przez ciążę
    if (isPregnant() && m_pregnancyTimer >= m_pregnancyDuration) {
        return giveBirth();
    }

    // Inicjowanie reprodukcji
    if (canReproduce() && m_partner && m_partner->canReproduce()) {
        startPregnancy();
        m_partner->startPregnancy();
        m_timeSinceLastReproduction = 0;
        m_partner->setTimeSinceLastReproduction(0);
    }

    return nullptr;
}

Organism* Prey::giveBirth()
{
    if (isPregnant() && m_pregnancyTimer >= m_pregnancyDuration) {
        m_pregnancyTimer = 0;
        m_currentState = CARING_FOR_YOUNG;

        if (m_partner) {
            Prey* offspring = new Prey(*this, *(static_cast<Prey*>(m_partner)));
            offspring->setPosition(m_position);
            addChild(offspring);
            m_timeSinceLastReproduction = 0;

            m_partner = nullptr;
            m_currentState = IDLE;

            return offspring;
        }
    }

    return nullptr;
}

void Prey::decideNextAction()
{
    // Prosta logika priorytetów
    if (!m_nearbyPredators.isEmpty()) {
        m_currentState = FLEEING;
    } else if (m_thirst > 70.0f) {
        m_currentState = SEEKING_WATER;
    } else if (m_hunger > 60.0f) {
        m_currentState = SEEKING_FOOD;
    } else if (m_fatigue > 80.0f) {
        m_currentState = SLEEPING;
        m_isAsleep = true;
        m_sleepTimer = 0;
    } else if (m_reproductionUrge > 50.0f && canReproduce()) {
        m_currentState = SEEKING_MATE;
    } else {
        m_currentState = IDLE;
    }
}

QPointF Prey::calculateMovement()
{
    QPointF direction(0, 0);

    switch (m_currentState) {
    case FLEEING:
        direction = fleeFromDanger();
        break;
    case SEEKING_WATER:
        direction = seekWater();
        break;
    case SEEKING_FOOD:
        direction = seekFood();
        break;
    case SEEKING_MATE:
        direction = seekMate();
        break;
    case SLEEPING:
        direction = QPointF(0, 0);
        break;
    case IDLE:
        if (QRandomGenerator::global()->bounded(100) < 10) {
            float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
            direction = QPointF(qCos(angle), qSin(angle));
        } else {
            direction = m_direction;
        }
        break;
    default:
        direction = QPointF(0, 0);
        break;
    }

    float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        direction /= length;
    }

    m_direction = direction;
    return direction * m_speed;
}

QPointF Prey::seekWater()
{
    if (m_availableWater.isEmpty()) {
        return QPointF(0, 0);
    }

    WaterSource* nearest = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (WaterSource* water : m_availableWater) {
        if (water->isAvailable()) {
            float dx = water->position().x() - m_position.x();
            float dy = water->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < minDistance && distance < m_visionRange) {
                minDistance = distance;
                nearest = water;
            }
        }
    }

    if (nearest) {
        QPointF direction = nearest->position() - m_position;
        float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0) {
            return direction / length;
        }
    }

    if (QRandomGenerator::global()->bounded(100) < 20) {
        float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
        return QPointF(qCos(angle), qSin(angle));
    }

    return QPointF(0, 0);
}

QPointF Prey::seekFood()
{
    if (m_availableBushes.isEmpty()) {
        return QPointF(0, 0);
    }

    Bush* nearest = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Bush* bush : m_availableBushes) {
        if (bush->hasFood()) {
            float dx = bush->position().x() - m_position.x();
            float dy = bush->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < minDistance && distance < m_visionRange) {
                minDistance = distance;
                nearest = bush;
            }
        }
    }

    if (nearest) {
        QPointF direction = nearest->position() - m_position;
        float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0) {
            return direction / length;
        }
    }

    if (QRandomGenerator::global()->bounded(100) < 20) {
        float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
        return QPointF(qCos(angle), qSin(angle));
    }

    return QPointF(0, 0);
}

QPointF Prey::seekMate()
{
    if (m_availableMates.isEmpty()) {
        return QPointF(0, 0);
    }

    Organism* nearestMate = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Organism* mate : m_availableMates) {
        if (mate != this &&
            mate->type() == PREY &&
            mate->canReproduce() &&
            !mate->isPregnant() &&
            mate->ageStage() == ADULT) {

            float dx = mate->position().x() - m_position.x();
            float dy = mate->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < minDistance && distance < m_visionRange) {
                minDistance = distance;
                nearestMate = mate;
            }
        }
    }

    if (nearestMate) {
        if (minDistance < m_size + nearestMate->size() + 10) {
            m_partner = nearestMate;
            return QPointF(0, 0);
        }

        QPointF direction = nearestMate->position() - m_position;
        float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0) {
            return direction / length;
        }
    }

    if (QRandomGenerator::global()->bounded(100) < 20) {
        float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
        return QPointF(qCos(angle), qSin(angle));
    }

    return QPointF(0, 0);
}

QPointF Prey::fleeFromDanger()
{
    if (m_nearbyPredators.isEmpty()) {
        return QPointF(0, 0);
    }

    QPointF fleeDirection(0, 0);
    float closestDistance = std::numeric_limits<float>::max();
    Organism* closestPredator = nullptr;

    for (Organism* predator : m_nearbyPredators) {
        float dx = predator->position().x() - m_position.x();
        float dy = predator->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestPredator = predator;
        }
    }

    if (closestPredator) {
        fleeDirection = m_position - closestPredator->position();
        float length = qSqrt(fleeDirection.x() * fleeDirection.x() +
                             fleeDirection.y() * fleeDirection.y());

        if (length > 0) {
            fleeDirection /= length;
        }

        if (QRandomGenerator::global()->bounded(100) < 30) {
            float angle = QRandomGenerator::global()->bounded(60) * M_PI / 180.0f - 30.0f;
            float cosA = qCos(angle);
            float sinA = qSin(angle);
            fleeDirection = QPointF(
                fleeDirection.x() * cosA - fleeDirection.y() * sinA,
                fleeDirection.x() * sinA + fleeDirection.y() * cosA
                );
        }
    }

    return fleeDirection;
}

void Prey::interactWithEnvironment()
{
    // Interakcja z wodą
    for (WaterSource* water : m_availableWater) {
        float dx = water->position().x() - m_position.x();
        float dy = water->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < water->radius() + m_size) {
            if (water->isAvailable() && m_thirst > 0) {
                float amount = qMin(5.0f, m_thirst);
                water->consume(amount);
                drink(amount);
            }
        }
    }

    // Interakcja z krzakami
    for (Bush* bush : m_availableBushes) {
        float dx = bush->position().x() - m_position.x();
        float dy = bush->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < bush->radius() + m_size) {
            if (bush->hasFood() && m_hunger > 0) {
                float amount = qMin(3.0f, m_hunger);
                float foodEaten = bush->consume(amount);
                if (foodEaten > 0) {
                    eat(foodEaten);
                }
            }
        }
    }

    // Interakcja z partnerem
    if (m_currentState == SEEKING_MATE && m_partner) {
        float dx = m_partner->position().x() - m_position.x();
        float dy = m_partner->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < m_size + m_partner->size() + 5) {
            if (canReproduce() && m_partner->canReproduce()) {
                m_partner->setPartner(this);
                startPregnancy();
            }
        }
    }
}

void Prey::updateColor()
{
    int green = 255;
    int red = 0;
    int blue = 0;

    switch (m_currentState) {
    case FLEEING:
        red = 255;
        green = 100;
        blue = 0;
        break;
    case SEEKING_WATER:
        red = 100;
        green = 150;
        blue = 255;
        break;
    case SEEKING_FOOD:
        red = 150;
        green = 200;
        blue = 0;
        break;
    case SEEKING_MATE:
        red = 255;
        green = 200;
        blue = 100;
        break;
    case PREGNANT:
        red = 200;
        green = 100;
        blue = 200;
        break;
    case SLEEPING:
        red = 100;
        green = 100;
        blue = 100;
        break;
    case CARING_FOR_YOUNG:
        red = 150;
        green = 150;
        blue = 255;
        break;
    default:
        green = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
        red = 255 - green;
        break;
    }

    m_color = QColor(red, green, blue);
}

void Prey::followGroup()
{
    if (m_groupMembers.isEmpty()) {
        findNearbyGroupMembers();
    }

    if (!m_groupMembers.isEmpty()) {
        QPointF groupCenter = calculateGroupCenter();
        QPointF toCenter = groupCenter - m_position;
        float distance = qSqrt(toCenter.x() * toCenter.x() + toCenter.y() * toCenter.y());

        if (distance > 100) {
            m_direction = toCenter / distance;
        }
    }
}

QPointF Prey::calculateGroupCenter()
{
    if (m_groupMembers.isEmpty()) {
        return m_position;
    }

    QPointF center(0, 0);
    for (Prey* member : m_groupMembers) {
        center += member->position();
    }
    center /= m_groupMembers.size();

    return center;
}

bool Prey::isTooFarFromGroup()
{
    if (m_groupMembers.isEmpty()) {
        return false;
    }

    QPointF center = calculateGroupCenter();
    float dx = center.x() - m_position.x();
    float dy = center.y() - m_position.y();
    float distance = qSqrt(dx * dx + dy * dy);

    return distance > 150;
}

void Prey::findNearbyGroupMembers()
{
    m_groupMembers.clear();
}

QPointF Prey::findSafePlace()
{
    return QPointF(100, 100);
}

void Prey::updateSocialBehavior()
{
    findNearbyGroupMembers();
}

void Prey::updateReproductionCycle()
{
    if (m_ageStage == ADULT && m_age % 100 == 0) {
        m_reproductionUrge += 10.0f;
        if (m_reproductionUrge > 100.0f) {
            m_reproductionUrge = 100.0f;
        }
    }
}

void Prey::updateNeedsInternal()
{
    m_hunger += 0.1f * m_metabolismGene;
    m_thirst += 0.2f * m_metabolismGene;
    m_fatigue += 0.05f * (2.0f - m_staminaGene);

    if (m_currentState == FLEEING) {
        m_hunger += 0.3f;
        m_thirst += 0.4f;
        m_fatigue += 0.2f;
    }

    m_hunger = qMax(0.0f, qMin(100.0f, m_hunger));
    m_thirst = qMax(0.0f, qMin(100.0f, m_thirst));
    m_fatigue = qMax(0.0f, qMin(100.0f, m_fatigue));
}
