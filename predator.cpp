#include "predator.h"
#include "water.h"
#include "bush.h"
#include <QtMath>
#include <QDebug>
#include <QLineF>
#include <QRandomGenerator>
#include <algorithm>

Predator::Predator(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
{
    m_position = position;
    m_type = PREDATOR;
    m_color = QColor(255, 0, 0);
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_age = 0;
    m_ageStage = BABY;
    m_currentState = IDLE;

    // Inicjalizacja potrzeb
    m_hunger = 30.0f;
    m_thirst = 30.0f;
    m_fatigue = 0.0f;
    m_reproductionUrge = 0.0f;

    // Inicjalizacja genów
    m_speedGene = speed / 3.0f;
    m_sizeGene = size / 15.0f;
    m_visionGene = vision / 150.0f;
    m_reproductionRateGene = 1.0f;
    m_metabolismGene = 1.0f;
    m_staminaGene = 1.0f;

    // Reprodukcja
    m_pregnancyTimer = 0;
    m_pregnancyDuration = 120;
    m_partner = nullptr;
    m_timeSinceLastReproduction = 0;

    // Inicjalizacja zmiennych specyficznych dla Predator
    m_direction = QPointF(0, 0);
    m_huntCooldown = 0;
    m_isHunting = false;
    m_currentTarget = nullptr;

    // Losowy kierunek początkowy
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

Predator::Predator(const Predator &parent1, const Predator &parent2)
    : Organism(nullptr)
{
    m_type = PREDATOR;
    m_color = QColor(255, 0, 0);

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
    m_speed = m_speedGene * 3.0f;
    m_size = m_sizeGene * 15.0f;
    m_visionRange = m_visionGene * 150.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_age = 0;
    m_ageStage = BABY;
    m_currentState = IDLE;

    // Potrzeby
    m_hunger = 30.0f;
    m_thirst = 30.0f;
    m_fatigue = 0.0f;
    m_reproductionUrge = 0.0f;

    // Reprodukcja
    m_pregnancyTimer = 0;
    m_pregnancyDuration = 120;
    m_partner = nullptr;
    m_timeSinceLastReproduction = 0;

    // Inicjalizacja zmiennych specyficznych dla Predator
    m_direction = QPointF(0, 0);
    m_huntCooldown = 0;
    m_isHunting = false;
    m_currentTarget = nullptr;

    // Losowy kierunek
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Predator::update()
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

    // Aktualizacja czasu polowania
    if (m_huntCooldown > 0) {
        m_huntCooldown--;
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

Organism* Predator::reproduce()
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
        m_energy -= 75.0f;
    }

    return nullptr;
}

Organism* Predator::giveBirth()
{
    if (isPregnant() && m_pregnancyTimer >= m_pregnancyDuration) {
        m_pregnancyTimer = 0;
        m_currentState = CARING_FOR_YOUNG;

        if (m_partner) {
            Predator* offspring = new Predator(*this, *(static_cast<Predator*>(m_partner)));
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

Prey* Predator::findNearestPrey()
{
    Prey* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Prey* prey : m_availablePrey) {
        if (prey->energy() > 0) {
            float dx = prey->position().x() - m_position.x();
            float dy = prey->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < nearestDistance && distance < m_visionRange) {
                nearestDistance = distance;
                nearest = prey;
            }
        }
    }

    return nearest;
}

void Predator::decideNextAction()
{
    // Aktualizuj cel polowania
    if (m_currentTarget && (m_currentTarget->energy() <= 0 ||
                            qSqrt(qPow(m_currentTarget->position().x() - m_position.x(), 2) +
                                  qPow(m_currentTarget->position().y() - m_position.y(), 2)) > m_visionRange * 1.5f)) {
        m_currentTarget = nullptr;
        m_isHunting = false;
    }

    // Logika priorytetów
    if (m_thirst > 80.0f) {
        m_currentState = SEEKING_WATER;
        m_isHunting = false;
    } else if (m_hunger > 60.0f || (m_currentTarget && m_hunger > 30.0f)) {
        m_currentState = SEEKING_FOOD;
        m_isHunting = true;
    } else if (m_fatigue > 70.0f) {
        m_currentState = SLEEPING;
        m_isHunting = false;
    } else if (m_reproductionUrge > 40.0f && canReproduce()) {
        m_currentState = SEEKING_MATE;
        m_isHunting = false;
    } else {
        m_currentState = IDLE;
        if (m_hunger > 30.0f) {
            m_isHunting = true;
        }
    }
}

QPointF Predator::calculateMovement()
{
    QPointF direction(0, 0);

    switch (m_currentState) {
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
        if (m_isHunting && m_huntCooldown == 0) {
            direction = seekFood();
        } else if (QRandomGenerator::global()->bounded(100) < 10) {
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

QPointF Predator::seekWater()
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

QPointF Predator::seekFood()
{
    Prey* target = findNearestPrey();

    if (target) {
        m_currentTarget = target;
        m_isHunting = true;

        QPointF direction = target->position() - m_position;
        float distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if (distance > 0) {
            return direction / distance;
        }
    } else {
        m_currentTarget = nullptr;
        m_isHunting = false;

        if (QRandomGenerator::global()->bounded(100) < 30) {
            float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
            return QPointF(qCos(angle), qSin(angle));
        }
    }

    return m_direction;
}

QPointF Predator::seekMate()
{
    if (m_availableMates.isEmpty()) {
        return QPointF(0, 0);
    }

    Organism* nearestMate = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Organism* mate : m_availableMates) {
        if (mate != this &&
            mate->type() == PREDATOR &&
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

QPointF Predator::fleeFromDanger()
{
    return QPointF(0, 0);
}

void Predator::interactWithEnvironment()
{
    // Interakcja z wodą
    for (WaterSource* water : m_availableWater) {
        float dx = water->position().x() - m_position.x();
        float dy = water->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < water->radius() + m_size) {
            if (water->isAvailable() && m_thirst > 0) {
                float amount = qMin(8.0f, m_thirst);
                water->consume(amount);
                drink(amount);
            }
        }
    }

    // Interakcja z ofiarami
    if (m_currentTarget && m_currentTarget->energy() > 0) {
        float dx = m_currentTarget->position().x() - m_position.x();
        float dy = m_currentTarget->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < m_size + m_currentTarget->size()) {
            if (m_huntCooldown == 0) {
                float damage = 20.0f + m_size * 2.0f;
                m_currentTarget->setEnergy(m_currentTarget->energy() - damage);

                if (m_currentTarget->energy() <= 0) {
                    float foodValue = 50.0f + m_currentTarget->size() * 5.0f;
                    eat(foodValue);
                    m_hunger = qMax(0.0f, m_hunger - 40.0f);
                    m_huntCooldown = 30;
                    m_currentTarget = nullptr;
                    m_isHunting = false;
                } else {
                    m_huntCooldown = 10;
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

void Predator::updateColor()
{
    int red = 255;
    int green = 0;
    int blue = 0;

    switch (m_currentState) {
    case SEEKING_FOOD:
    case IDLE:
        if (m_isHunting) {
            red = 255;
            green = 100;
            blue = 0;
        } else {
            red = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
            green = red / 3;
        }
        break;
    case SEEKING_WATER:
        red = 100;
        green = 100;
        blue = 255;
        break;
    case SEEKING_MATE:
        red = 255;
        green = 150;
        blue = 50;
        break;
    case PREGNANT:
        red = 200;
        green = 50;
        blue = 200;
        break;
    case SLEEPING:
        red = 100;
        green = 0;
        blue = 100;
        break;
    case CARING_FOR_YOUNG:
        red = 255;
        green = 100;
        blue = 100;
        break;
    default:
        red = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
        green = red / 4;
        break;
    }

    m_color = QColor(red, green, blue);
}
