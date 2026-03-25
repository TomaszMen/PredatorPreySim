#include "organism.h"
#include <QRandomGenerator>
#include <QDebug>

float Organism::s_energyConsumptionFactor = 1.0f;
float Organism::s_mutationRatePercent = 5.0f;
float Organism::s_preyReproductionFactor = 1.0f;
float Organism::s_predatorReproductionFactor = 1.0f;

Organism::Organism(QObject *parent)
    : QObject(parent)
    , m_position(0, 0)
    , m_energy(100.0f)
    , m_speed(1.0f)
    , m_size(5.0f)
    , m_visionRange(50.0f)
    , m_hydration(100.0f)
    , m_age(0)
    , m_state(WANDERING)
    , m_justReproduced(false)
    , m_reproductionCooldown(0)
    , m_speedGene(1.0f)
    , m_sizeGene(1.0f)
    , m_visionGene(1.0f)
    , m_reproductionRateGene(1.0f)
    , m_hydrationRateGene(1.0f)
    , m_currentTarget(nullptr)
    , m_currentEnvironmentTarget(nullptr)
    , m_stateTimer(0)
    , m_fearLevel(0.0f)
    , m_hungerLevel(0.0f)
    , m_thirstLevel(0.0f)
    , m_matingUrge(0.0f)
    , m_lastMealTime(0)
    , m_lastDrinkTime(0)
    , m_lastKillTime(0)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

Organism::~Organism()
{
}

void Organism::mutateGenes()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    float mutationChance = s_mutationRatePercent / 100.0f;

    if (rand->bounded(1.0f) < mutationChance) {
        float mutation = (rand->bounded(200) - 100) / 500.0f;
        m_speedGene += mutation;
    }

    if (rand->bounded(1.0f) < mutationChance) {
        float mutation = (rand->bounded(200) - 100) / 500.0f;
        m_sizeGene += mutation;
    }

    if (rand->bounded(1.0f) < mutationChance) {
        float mutation = (rand->bounded(200) - 100) / 500.0f;
        m_visionGene += mutation;
    }

    if (rand->bounded(1.0f) < mutationChance) {
        float mutation = (rand->bounded(200) - 100) / 500.0f;
        m_reproductionRateGene += mutation;
    }

    if (rand->bounded(1.0f) < mutationChance) {
        float mutation = (rand->bounded(200) - 100) / 500.0f;
        m_hydrationRateGene += mutation;
    }

    m_speedGene = std::max(0.3f, std::min(3.0f, m_speedGene));
    m_sizeGene = std::max(0.3f, std::min(3.0f, m_sizeGene));
    m_visionGene = std::max(0.3f, std::min(3.0f, m_visionGene));
    m_reproductionRateGene = std::max(0.1f, std::min(5.0f, m_reproductionRateGene));
    m_hydrationRateGene = std::max(0.5f, std::min(2.0f, m_hydrationRateGene));
}

bool Organism::isInWater() const
{
    for (Environment* env : m_environment) {
        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);
            if (distance < env->size() + m_size) {
                return true;
            }
        }
    }
    return false;
}

QPointF Organism::avoidWater()
{
    QPointF avoidance(0, 0);
    int waterCount = 0;

    for (Environment* env : m_environment) {
        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_visionRange * 1.5f) {
                // Im bliżej wody, tym silniejsze unikanie
                float weight = 1.0f - (distance / (m_visionRange * 1.5f));
                QPointF dir(-dx, -dy);
                float len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
                if (len > 0) {
                    dir /= len;
                }
                avoidance += dir * weight;
                waterCount++;
            }
        }
    }

    if (waterCount > 0) {
        avoidance /= waterCount;
    }

    return avoidance;
}

QPointF Organism::seekWater()
{
    Environment* water = findNearestWater();
    if (water) {
        QPointF direction = water->position() - m_position;
        float distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (distance > 0) {
            direction /= distance;
            return direction;
        }
    }
    return QPointF(0, 0);
}

Environment* Organism::findNearestWater()
{
    if (m_environment.isEmpty()) {
        return nullptr;
    }

    Environment* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    int size = m_environment.size();
    for (int i = 0; i < size; ++i) {
        Environment* env = m_environment[i];

        if (!env) {
            continue;
        }

        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < nearestDistance && distance < m_visionRange * 2) {
                nearestDistance = distance;
                nearest = env;
            }
        }
    }

    return nearest;
}

Environment* Organism::findNearestBush()
{
    // Sprawdź czy środowisko jest puste
    if (m_environment.isEmpty()) {
        return nullptr;
    }

    Environment* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    int envSize = m_environment.size();
    for (int i = 0; i < envSize; ++i) {
        Environment* env = m_environment[i];

        // Kluczowe: sprawdź czy wskaźnik jest poprawny
        if (!env) {
            continue;
        }

        // Dodaj sprawdzenie, czy env nie został usunięty
        // Możemy sprawdzić poprzez typ (jeśli env jest nullptr, to już sprawdziliśmy)

        if (env->type() == Environment::BUSH && env->foodLevel() > 10) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < nearestDistance && distance < m_visionRange * 1.5f) {
                nearestDistance = distance;
                nearest = env;
            }
        }
    }

    return nearest;
}

Organism* Organism::findNearestPrey()
{
    Organism* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Organism* prey : m_availablePrey) {
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

Organism* Organism::findNearestMate()
{
    Organism* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Organism* org : m_availablePrey) {
        if (org != this && org->canMate() && org->energy() > 0) {
            float dx = org->position().x() - m_position.x();
            float dy = org->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);
            if (distance < nearestDistance && distance < m_visionRange) {
                nearestDistance = distance;
                nearest = org;
            }
        }
    }

    return nearest;
}

Organism* Organism::findNearestPredator()
{
    Organism* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Organism* predator : m_availablePredators) {
        if (predator->energy() > 0) {
            float dx = predator->position().x() - m_position.x();
            float dy = predator->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);
            if (distance < nearestDistance && distance < m_visionRange * 1.2f) {
                nearestDistance = distance;
                nearest = predator;
            }
        }
    }

    return nearest;
}

void Organism::moveTowards(const QPointF &target, float weight)
{
    QPointF direction = target - m_position;
    float distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (distance > 0) {
        direction /= distance;
        m_direction = m_direction * (1.0f - weight) + direction * weight;
        // Normalizuj
        float length = qSqrt(m_direction.x() * m_direction.x() + m_direction.y() * m_direction.y());
        if (length > 0) {
            m_direction /= length;
        }
    }
}

void Organism::moveAwayFrom(const QPointF &target, float weight)
{
    QPointF direction = m_position - target;
    float distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (distance > 0) {
        direction /= distance;
        m_direction = m_direction * (1.0f - weight) + direction * weight;
        float length = qSqrt(m_direction.x() * m_direction.x() + m_direction.y() * m_direction.y());
        if (length > 0) {
            m_direction /= length;
        }
    }
}

void Organism::wander()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    if (rand->bounded(100) < 5) {
        float angle = (rand->bounded(60) - 30) * M_PI / 180.0f;
        float currentAngle = qAtan2(m_direction.y(), m_direction.x());
        float newAngle = currentAngle + angle;
        m_direction = QPointF(qCos(newAngle), qSin(newAngle));
    }
}

void Organism::applyBoundaries()
{
    // Granice świata (powiększony świat)
    float worldWidth = 3000;
    float worldHeight = 2000;
    if (m_position.x() < 0) {
        m_direction.setX(qAbs(m_direction.x()));
        m_position.setX(1);
    }
    if (m_position.x() > worldWidth) {
        m_direction.setX(-qAbs(m_direction.x()));
        m_position.setX(worldWidth - 1);
    }
    if (m_position.y() < 0) {
        m_direction.setY(qAbs(m_direction.y()));
        m_position.setY(1);
    }
    if (m_position.y() > worldHeight) {
        m_direction.setY(-qAbs(m_direction.y()));
        m_position.setY(worldHeight - 1);
    }
}

float Organism::calculateFitness() const
{
    // Fitness do selekcji naturalnej
    float fitness = 0;
    fitness += m_speed * 0.3f;
    fitness += m_visionRange * 0.2f;
    fitness += (200.0f - m_size) * 0.1f; // Mniejsze organizmy mają przewagę
    fitness += m_energy * 0.4f;
    return fitness;
}

void Organism::setGlobalParameters(float energyFactor, float mutation,
                                   float preyRepro, float predatorRepro)
{
    s_energyConsumptionFactor = energyFactor;
    s_mutationRatePercent = mutation;
    s_preyReproductionFactor = preyRepro;
    s_predatorReproductionFactor = predatorRepro;
}
