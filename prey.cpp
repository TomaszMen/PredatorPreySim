#include "prey.h"
#include <QtMath>
#include <QDebug>

Prey::Prey(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
    , m_mateTarget(nullptr)
    , m_canSwim(false)  // Inicjalizacja DODANA
{
    m_position = position;
    m_type = PREY;
    m_color = QColor(0, 255, 0);
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_hydration = 100.0f;
    m_age = 0;

    // Inicjalizacja genów z normalizacją
    m_speedGene = speed / 2.0f;
    m_sizeGene = size / 10.0f;
    m_visionGene = vision / 100.0f;
    m_reproductionRateGene = 1.0f;
    m_hydrationRateGene = 1.0f;

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

Prey::Prey(const Prey &parent1, const Prey &parent2)
    : Organism(nullptr)
    , m_mateTarget(nullptr)
    , m_canSwim(false)  // Inicjalizacja DODANA
{
    m_type = PREY;
    m_color = QColor(0, 255, 0);

    // Dziedziczenie z mutacją
    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;
    m_hydrationRateGene = (parent1.m_hydrationRateGene + parent2.m_hydrationRateGene) / 2.0f;

    mutateGenes();

    // Ustawienie fenotypu
    m_speed = m_speedGene * 2.0f;
    m_size = m_sizeGene * 10.0f;
    m_visionRange = m_visionGene * 100.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_hydration = 100.0f;
    m_age = 0;

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Prey::update()
{
    if (m_energy <= 0 || m_hydration <= 0) return;

    m_age++;
    m_lastMealTime++;
    m_lastDrinkTime++;

    // Zmniejszone zużycie energii (wydłużenie życia)
    m_energy -= 0.02f * (1.0f + m_speed / 3.0f);
    m_hydration -= 0.04f * m_hydrationRateGene;

    // Mniejsza kara za bycie w wodzie
    if (isInWater() && !m_canSwim) {
        m_energy -= 0.05f;
        m_state = FLEEING;
    }

    // Aktualizuj potrzeby i AI
    updateNeeds();
    updateAI();

    // Wykonaj ruch zgodnie z aktualnym stanem
    executeState();

    // Ruch z możliwością przyśpieszenia podczas ucieczki
    float currentSpeed = m_speed;
    if (m_state == FLEEING && m_fearLevel > 0.5f) {
        currentSpeed *= 1.8f; // Przyśpieszenie podczas ucieczki
    }

    m_position += m_direction * currentSpeed;
    applyBoundaries();

    // Mniejsze ograniczenie prędkości w wodzie
    if (isInWater() && !m_canSwim) {
        m_position += m_direction * (currentSpeed * 0.5f);
    }

    // Aktualizacja koloru
    int green = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
    int blue = std::max(0, std::min(255, (int)(255 * (m_hydration / 100.0f))));
    m_color = QColor(0, green, blue);

    // Reset cooldownów
    if (m_reproductionCooldown > 0) m_reproductionCooldown--;
    if (m_reproductionCooldown == 0) m_justReproduced = false;
}

void Prey::updateAI()
{
    // Hierarchia potrzeb:
    // 1. Unikaj drapieżników (strach)
    // 2. Szukaj wody (pragnienie)
    // 3. Szukaj jedzenia (głód)
    // 4. Szukaj partnera (rozmnażanie)
    // 5. Wędruj losowo

    m_fearLevel = 0;
    m_thirstLevel = 0;
    m_hungerLevel = 0;
    m_matingUrge = 0;

    // Oblicz poziom strachu
    Organism* predator = findNearestPredator();
    if (predator) {
        float dx = predator->position().x() - m_position.x();
        float dy = predator->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);
        m_fearLevel = 1.0f - (distance / (m_visionRange * 1.5f));
        m_fearLevel = std::max(0.0f, std::min(1.0f, m_fearLevel));
    }

    // Oblicz poziom pragnienia
    m_thirstLevel = 1.0f - (m_hydration / 100.0f);
    if (m_lastDrinkTime > 200) m_thirstLevel = 1.0f;

    // Oblicz poziom głodu
    m_hungerLevel = 1.0f - (m_energy / 200.0f);
    if (m_lastMealTime > 150) m_hungerLevel = 1.0f;

    // Oblicz potrzebę rozmnażania
    if (canMate() && m_age > 100) {
        m_matingUrge = 0.7f;
    }
}

void Prey::updateNeeds()
{
    // Sprawdź czy jesteś przy krzaku i jedz
    for (Environment* env : m_environment) {
        if (env->type() == Environment::BUSH) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + env->size()) {
                float foodTaken = env->consumeFood(5.0f);
                if (foodTaken > 0) {
                    m_energy += foodTaken * 3.0f;
                    m_energy = std::min(200.0f, m_energy);
                    m_lastMealTime = 0;
                    m_state = EATING;
                    m_stateTimer = 5;
                }
            }
        }

        // Pij wodę
        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + env->size() * 0.5f) {
                m_hydration = std::min(100.0f, m_hydration + 20.0f);
                m_lastDrinkTime = 0;
                m_state = DRINKING;
                m_stateTimer = 3;
            }
        }
    }
}

void Prey::decideState()
{
    m_stateTimer--;

    if (m_stateTimer > 0 && (m_state == EATING || m_state == DRINKING || m_state == MATING)) {
        if (m_stateTimer < 15) {
            m_state = WANDERING;
        }
        return;
    }

    if (m_fearLevel > 0.2f) {
        m_state = FLEEING;
        m_stateTimer = 20 + QRandomGenerator::global()->bounded(30);
    }
    else if (m_thirstLevel > 0.8f && m_hydration < 30.0f) {
        m_state = HUNTING;
        m_stateTimer = 30 + QRandomGenerator::global()->bounded(30);
    }
    else if (m_hungerLevel > 0.7f && m_energy < 60.0f) {
        m_state = HUNTING;
        m_stateTimer = 25 + QRandomGenerator::global()->bounded(25);
    }
    else if (m_matingUrge > 0.6f && canMate()) {
        m_state = MATING;
        m_stateTimer = 40;
    }
    else {
        m_state = WANDERING;
        m_stateTimer = 15 + QRandomGenerator::global()->bounded(20);
    }
}

void Prey::executeState()
{
    decideState();

    switch(m_state) {
    case FLEEING: {
        Organism* predator = findNearestPredator();
        if (predator) {
            moveAwayFrom(predator->position(), 0.8f);
        } else {
            wander();
        }
        QPointF waterAvoidance = avoidWater();
        if (waterAvoidance.manhattanLength() > 0) {
            moveTowards(m_position + waterAvoidance, 0.3f);
        }
        break;
    }
    case HUNTING: {
        if (m_thirstLevel > m_hungerLevel) {
            Environment* water = findNearestWater();
            if (water) {
                moveTowards(water->position(), 0.6f);
            } else {
                wander();
            }
        } else {
            Environment* bush = findNearestBush();
            if (bush) {
                moveTowards(bush->position(), 0.6f);
            } else {
                wander();
            }
        }
        break;
    }
    case MATING: {
        if (!m_mateTarget) {
            m_mateTarget = findNearestMate();
        }

        if (m_mateTarget && m_mateTarget->canMate()) {
            float dx = m_mateTarget->position().x() - m_position.x();
            float dy = m_mateTarget->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < 20) {
                float angle = qAtan2(m_direction.y(), m_direction.x()) + 0.1f;
                m_direction = QPointF(qCos(angle), qSin(angle));
            } else {
                moveTowards(m_mateTarget->position(), 0.7f);
            }
        } else {
            wander();
        }
        break;
    }
    case EATING:
    case DRINKING:
        m_direction *= 0.3f;
        break;
    case WANDERING:
    default:
        wander();
        QPointF waterAvoidance = avoidWater();
        if (waterAvoidance.manhattanLength() > 0) {
            moveTowards(m_position + waterAvoidance, 0.1f);
        }
        break;
    }
}

Organism* Prey::findNearestMate()
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

Organism* Prey::findNearestPredator()
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

QPointF Prey::findFood()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    return QPointF(qCos(angle), qSin(angle));
}

void Prey::avoidPredators()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    if (rand->bounded(100) < 10) {
        float angle = rand->bounded(360) * M_PI / 180.0f;
        m_direction = QPointF(qCos(angle), qSin(angle));
    }
}

Organism* Prey::reproduce()
{
    if (!canMate() || m_justReproduced) return nullptr;

    Organism* mate = findNearestMate();
    if (!mate || mate == this) return nullptr;

    float dx = mate->position().x() - m_position.x();
    float dy = mate->position().y() - m_position.y();
    float distance = qSqrt(dx * dx + dy * dy);

    if (distance < 25 && QRandomGenerator::global()->bounded(100) < m_reproductionRateGene * 2) {
        m_energy -= 40;
        m_hydration -= 30;
        m_justReproduced = true;
        m_reproductionCooldown = 100;

        mate->setEnergy(mate->energy() - 40);

        return new Prey(*static_cast<Prey*>(this), *static_cast<Prey*>(mate));
    }

    return nullptr;
}
