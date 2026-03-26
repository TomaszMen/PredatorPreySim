#include "predator.h"
#include <QtMath>
#include <QDebug>

Predator::Predator(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
    , m_mateTarget(nullptr)
    , m_canSwim(true) // Drapieżniki mogą pływać
{
    m_position = position;
    m_type = PREDATOR;
    m_color = QColor(255, 0, 0);
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_hydration = 100.0f;
    m_age = 0;

    // Inicjalizacja genów
    m_speedGene = speed / 3.0f;
    m_sizeGene = size / 15.0f;
    m_visionGene = vision / 150.0f;
    m_reproductionRateGene = 1.0f;
    m_hydrationRateGene = 0.8f; // Drapieżniki wolniej tracą wodę

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

Predator::Predator(const Predator &parent1, const Predator &parent2)
    : Organism(nullptr)
    , m_mateTarget(nullptr)
{
    m_type = PREDATOR;
    m_color = QColor(255, 0, 0);

    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;
    m_hydrationRateGene = (parent1.m_hydrationRateGene + parent2.m_hydrationRateGene) / 2.0f;

    m_canSwim = parent1.m_canSwim && parent2.m_canSwim;
    if (QRandomGenerator::global()->bounded(100) < 5) {
        m_canSwim = !m_canSwim;
    }

    mutateGenes();

    m_speed = m_speedGene * 3.0f;
    m_size = m_sizeGene * 15.0f;
    m_visionRange = m_visionGene * 150.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_hydration = 100.0f;
    m_age = 0;

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Predator::update()
{
    if (m_energy <= 0 || m_hydration <= 0) return;

    m_age++;
    m_lastKillTime++;

    m_energy -= 0.03f * (1.0f + m_speed / 5.0f) * s_energyConsumptionFactor;
    m_hydration -= 0.02f * m_hydrationRateGene * s_energyConsumptionFactor;

    if (isInWater() && !m_canSwim) {
        m_energy -= 0.01f;
        m_state = FLEEING;
        m_stateTimer = 30;
    }

    if (m_lastKillTime > 0) {
        m_unsuccessfulHuntTicks++;
    } else {
        m_unsuccessfulHuntTicks = 0;
    }

    updateNeeds();
    updateAI();
    executeState();

    float currentSpeed = m_speed;

    if (m_state == HUNTING && m_currentTarget != nullptr) {
        float dx = m_currentTarget->position().x() - m_position.x();
        float dy = m_currentTarget->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);
        if (distance < m_visionRange * 0.5f) {
            currentSpeed *= 1.5f;
        }
    }

    if (isInWater()) {
        if (m_canSwim) {
            currentSpeed = m_speed * 0.8f;
        } else {
            currentSpeed = m_speed * 0.3f;
        }
    }

    m_position += m_direction * currentSpeed;
    applyBoundaries();

    // Aktualizacja koloru
    int red = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
    int blue = std::max(0, std::min(255, (int)(255 * (m_hydration / 100.0f))));
    m_color = QColor(red, 0, blue);

    if (m_reproductionCooldown > 0) m_reproductionCooldown--;
    if (m_reproductionCooldown == 0) m_justReproduced = false;
}

void Predator::updateNeeds()
{
    for (Organism* prey : m_availablePrey) {
        if (prey->energy() > 0) {
            float dx = prey->position().x() - m_position.x();
            float dy = prey->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + prey->size()) {
                float energyGained = prey->energy();   // ← 100% zamiast 0.8
                m_energy += energyGained;
                m_energy = std::min(200.0f, m_energy);
                m_lastKillTime = 0;
                m_currentTarget = nullptr;

                prey->setEnergy(0);   // zabicie ofiary
                m_state = EATING;
                m_stateTimer = 8;
                break;
            }
        }
    }

    // Sprawdź wodę
    for (Environment* env : m_environment) {
        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + env->size() * 0.5f) {
                m_hydration = std::min(100.0f, m_hydration + 15.0f); // Stopniowe picie
                m_state = DRINKING;
                m_stateTimer = 4;
                break;
            }
        }
    }
}

void Predator::updateAI()
{
    // Hierarchia potrzeb drapieżnika:
    // 1. Poluj (głód)
    // 2. Szukaj wody (pragnienie)
    // 3. Szukaj partnera
    // 4. Patroluj/Wędruj

    m_hungerLevel = 0;
    m_thirstLevel = 0;
    m_matingUrge = 0;

    // Oblicz poziom głodu
    m_hungerLevel = 1.0f - (m_energy / 200.0f);
    if (m_lastKillTime > 100) m_hungerLevel = 1.0f;

    // Oblicz poziom pragnienia
    m_thirstLevel = 1.0f - (m_hydration / 100.0f);

    // Oblicz potrzebę rozmnażania
    if (canMate() && m_age > 150) {
        m_matingUrge = 0.6f;
    }

    // Aktualizuj odwiedzone krzaki
    updateVisitedBushes();

    // Sprawdź czy jesteśmy przy krzaku
    Environment* currentBush = findNearestBush();
    if (currentBush && !m_currentTarget) {
        float dx = currentBush->position().x() - m_position.x();
        float dy = currentBush->position().y() - m_position.y();
        float distance = qSqrt(dx*dx + dy*dy);

        if (distance < m_visionRange * 0.5f) {
            m_ticksAtCurrentBush++;
            m_lastVisitedBush = currentBush;

            // Jeśli kręcimy się przy krzaku zbyt długo (400 ticków)
            if (m_ticksAtCurrentBush > 400) {
                // Zapamiętaj ten krzak jako do unikania
                markBushAsVisited(currentBush);
                m_ticksAtCurrentBush = 0;
                m_lastVisitedBush = nullptr;
            }
        } else {
            m_ticksAtCurrentBush = 0;
        }
    } else {
        m_ticksAtCurrentBush = 0;
    }
}

void Predator::decideState()
{
    m_stateTimer--;

    if (m_stateTimer > 0 && (m_state == EATING || m_state == DRINKING || m_state == MATING)) {
        return;
    }

    // Hierarchia decyzji
    if (m_hungerLevel > 0.4f) {
        m_state = HUNTING;
        m_stateTimer = 60 + QRandomGenerator::global()->bounded(60);
    }
    else if (m_thirstLevel > 0.6f) {
        m_state = HUNTING; // Szukanie wody
        m_stateTimer = 40 + QRandomGenerator::global()->bounded(40);
    }
    else if (m_matingUrge > 0.4f && canMate()) {
        m_state = MATING;
        m_stateTimer = 80;
    }
    else {
        m_state = WANDERING;
        m_stateTimer = 30 + QRandomGenerator::global()->bounded(40);
    }
}

void Predator::executeState()
{
    decideState();

    switch(m_state) {
    case HUNTING: {
        if (m_hungerLevel > m_thirstLevel) {
            m_currentTarget = findNearestPrey();

            if (m_currentTarget) {
                moveTowards(m_currentTarget->position(), 0.9f);
            } else {
                // Brak ofiar w zasięgu
                if (m_unsuccessfulHuntTicks > EXPLORE_THRESHOLD) {
                    // Eksploracja – wybierz daleki losowy kierunek
                    QRandomGenerator* rand = QRandomGenerator::global();
                    float angle = rand->bounded(360) * M_PI / 180.0f;
                    float distance = 400 + rand->bounded(400);
                    QPointF target = m_position + QPointF(cos(angle) * distance, sin(angle) * distance);
                    moveTowards(target, 0.5f);
                    // Opcjonalnie: zresetuj licznik, aby nie eksplorować w każdej klatce
                    if (rand->bounded(100) < 20) {
                        m_unsuccessfulHuntTicks = EXPLORE_THRESHOLD / 2;
                    }
                } else {
                    // Zwykłe wędrowanie
                    wander();
                }
            }
        } else {
            // Szukanie wody – istniejąca logika
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

            if (distance < 30) {
                // Terytorialne zachowanie - nie kręć się w kółko
                if (QRandomGenerator::global()->bounded(100) < 50) {
                    // Czasami oddal się trochę
                    moveAwayFrom(m_mateTarget->position(), 0.3f);
                } else {
                    wander();
                }
            } else {
                moveTowards(m_mateTarget->position(), 0.8f);
            }
        } else {
            m_mateTarget = nullptr;
            wander();
        }
        break;
    }
    case EATING:
    case DRINKING:
        // Tylko raz na początku stanu zmniejsz kierunek
        if (m_stateTimer == 4) { // początek stanu
            m_direction *= 0.8f;
        }
        if (m_stateTimer < 3) {
            wander();
        }
        break;
    case FLEEING:
        if (isInWater()) {
            // Znajdź najbliższy ląd
            Environment* nearestLand = nullptr;
            float nearestDistance = std::numeric_limits<float>::max();
            for (Environment* env : m_environment) {
                if (env->type() != Environment::WATER) {
                    float dx = env->position().x() - m_position.x();
                    float dy = env->position().y() - m_position.y();
                    float distance = qSqrt(dx*dx + dy*dy);
                    if (distance < nearestDistance) {
                        nearestDistance = distance;
                        nearestLand = env;
                    }
                }
            }
            if (nearestLand) {
                // Mocniejszy zwrot w kierunku lądu i zwiększona prędkość ucieczki
                moveTowards(nearestLand->position(), 0.95f);
                // Dodatkowe przyspieszenie przy ucieczce z wody
                m_direction = m_direction * 1.5f;
            } else {
                // Jeśli nie ma lądu w zasięgu, uciekaj w losowym kierunku od środka wody
                QPointF awayFromWater(0, 0);
                for (Environment* env : m_environment) {
                    if (env->type() == Environment::WATER) {
                        QPointF dir = m_position - env->position();
                        float len = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
                        if (len > 0) {
                            awayFromWater += dir / len;
                        }
                    }
                }
                if (awayFromWater.manhattanLength() > 0) {
                    float len = qSqrt(awayFromWater.x()*awayFromWater.x() + awayFromWater.y()*awayFromWater.y());
                    if (len > 0) {
                        awayFromWater /= len;
                        m_direction = awayFromWater;
                    }
                }
            }
        } else {
            // normalne zachowanie ucieczki
            wander();
        }
        break;
    case WANDERING:
    default:
        wander();
        // Naturalne zachowanie - czasami idź do wody
        if (m_canSwim && QRandomGenerator::global()->bounded(100) < 5) {
            Environment* water = findNearestWater();
            if (water) {
                float dx = water->position().x() - m_position.x();
                float dy = water->position().y() - m_position.y();
                float distance = qSqrt(dx * dx + dy * dy);
                if (distance < m_visionRange * 0.5f) {
                    moveTowards(water->position(), 0.2f);
                }
            }
        }
        // Czasami sprawdź czy nie ma ofiar w pobliżu
        if (QRandomGenerator::global()->bounded(100) < 10) {
            Organism* nearbyPrey = findNearestPrey();
            if (nearbyPrey && nearbyPrey->energy() > 0) {
                float dx = nearbyPrey->position().x() - m_position.x();
                float dy = nearbyPrey->position().y() - m_position.y();
                float distance = qSqrt(dx * dx + dy * dy);
                if (distance < m_visionRange) {
                    m_state = HUNTING;
                    m_stateTimer = 30;
                    m_currentTarget = nearbyPrey;
                }
            }
        }
        break;
    }
}
Organism* Predator::findNearestMate()
{
    Organism* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Organism* org : m_availablePredators) {
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

Organism* Predator::findNearestPrey()
{
    Organism* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Organism* prey : m_availablePrey) {
        if (prey->energy() > 0) {
            float dx = prey->position().x() - m_position.x();
            float dy = prey->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            // Drapieżniki mogą polować przez płytką wodę
            bool canReach = true;
            if (isInWater() && !m_canSwim) {
                canReach = false;
            }

            if (distance < nearestDistance && distance < m_visionRange && canReach) {
                nearestDistance = distance;
                nearest = prey;
            }
        }
    }

    return nearest;
}

Organism* Predator::reproduce()
{
    if (!canMate() || m_justReproduced) return nullptr;

    Organism* mate = findNearestMate();
    if (!mate || mate == this) return nullptr;

    float dx = mate->position().x() - m_position.x();
    float dy = mate->position().y() - m_position.y();
    float distance = qSqrt(dx * dx + dy * dy);

    if (distance < 40 && QRandomGenerator::global()->bounded(100) < m_reproductionRateGene * 3 * s_predatorReproductionFactor) {
        m_energy -= 30;
        m_hydration -= 20;
        m_justReproduced = true;
        m_reproductionCooldown = 60;

        mate->setEnergy(mate->energy() - 30);

        return new Predator(*static_cast<Predator*>(this), *static_cast<Predator*>(mate));
    }

    return nullptr;
}

QPointF Predator::findNearestLandPoint()
{
    float step = 15.0f;                     // krok przeszukiwania
    float maxRadius = m_visionRange * 1.5f; // maksymalny promień szukania
    QPointF bestPoint = m_position;
    float bestDistSq = std::numeric_limits<float>::max();

    // Sprawdzamy 16 kierunków
    for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 8) {
        QPointF dir(std::cos(angle), std::sin(angle));
        for (float r = step; r <= maxRadius; r += step) {
            QPointF testPoint = m_position + dir * r;
            bool inWater = false;

            // Sprawdzamy, czy testPoint znajduje się w jakimś zbiorniku wodnym
            for (Environment* env : m_environment) {
                if (env->type() == Environment::WATER) {
                    float dx = env->position().x() - testPoint.x();
                    float dy = env->position().y() - testPoint.y();
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < env->size()) {
                        inWater = true;
                        break;
                    }
                }
            }

            if (!inWater) {
                // Znaleziono ląd – zapamiętujemy najbliższy punkt
                float distSq = r * r;
                if (distSq < bestDistSq) {
                    bestDistSq = distSq;
                    bestPoint = testPoint;
                }
                break; // dla tego kierunku już mamy pierwszy punkt na lądzie
            }
        }
    }
    return bestPoint;
}

void Predator::updateVisitedBushes()
{
    // Zwiększ liczniki i usuń stare wpisy (starsze niż 1200 ticków)
    for (int i = m_visitedBushes.size() - 1; i >= 0; --i) {
        m_visitedBushes[i].ticksSinceLastVisit++;
        if (m_visitedBushes[i].ticksSinceLastVisit > 1200) {
            m_visitedBushes.removeAt(i);
        }
    }
}

void Predator::markBushAsVisited(Environment* bush)
{
    // Sprawdź czy już nie ma tego krzaka w liście
    for (const VisitedBush& vb : m_visitedBushes) {
        if (vb.bush == bush) {
            return;
        }
    }

    VisitedBush newVB;
    newVB.bush = bush;
    newVB.ticksSinceLastVisit = 0;
    newVB.ticksSpentNearby = m_ticksAtCurrentBush;
    m_visitedBushes.append(newVB);
}

bool Predator::shouldAvoidBush(Environment* bush) const
{
    if (!bush) return false;

    for (const VisitedBush& vb : m_visitedBushes) {
        if (vb.bush == bush) {
            return true;
        }
    }
    return false;
}

Environment* Predator::findNearestBush()
{
    if (m_environment.isEmpty()) {
        return nullptr;
    }

    Environment* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Environment* env : m_environment) {
        if (!env) continue;

        if (env->type() == Environment::BUSH && env->foodLevel() > 10) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < nearestDistance && distance < m_visionRange * 1.2f) {
                nearestDistance = distance;
                nearest = env;
            }
        }
    }

    return nearest;
}
