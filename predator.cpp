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

    // Dziedziczenie cech
    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;
    m_hydrationRateGene = (parent1.m_hydrationRateGene + parent2.m_hydrationRateGene) / 2.0f;

    // Dziedziczenie umiejętności pływania
    m_canSwim = parent1.m_canSwim && parent2.m_canSwim;
    if (QRandomGenerator::global()->bounded(100) < 5) {
        m_canSwim = !m_canSwim; // Mutacja umiejętności pływania
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

    // Zmniejszone zużycie
    m_energy -= 0.05f * (1.0f + m_speed / 4.0f); // Zmniejszone z 0.1f
    m_hydration -= 0.03f * m_hydrationRateGene; // Zmniejszone z 0.06f

    // Mniejsza kara za wodę
    if (isInWater() && !m_canSwim) {
        m_energy -= 0.1f; // Zmniejszone z 0.3f
        m_state = FLEEING;
    }

    updateNeeds();
    updateAI();
    executeState();

    // Ruch z możliwością przyśpieszenia podczas polowania
    float currentSpeed = m_speed;
    if (m_state == HUNTING && m_currentTarget != nullptr) {
        float dx = m_currentTarget->position().x() - m_position.x();
        float dy = m_currentTarget->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);

        if (distance < m_visionRange * 0.5f) {
            currentSpeed *= 1.5f; // Przyśpieszenie podczas zbliżania do ofiary
        }
    }

    if (isInWater() && m_canSwim) {
        m_position += m_direction * (currentSpeed * 0.8f);
    } else {
        m_position += m_direction * currentSpeed;
    }

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
    // Sprawdź ofiary
    for (Organism* prey : m_availablePrey) {
        if (prey->energy() > 0) {
            float dx = prey->position().x() - m_position.x();
            float dy = prey->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + prey->size()) {
                float energyGained = prey->energy() * 0.8f;
                m_energy += energyGained;
                m_energy = std::min(200.0f, m_energy);
                m_lastKillTime = 0;
                m_currentTarget = nullptr; // Resetuj cel po zjedzeniu

                prey->setEnergy(0);
                m_state = EATING;
                m_stateTimer = 8; // Krótszy czas jedzenia
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
            // Szukaj ofiary
            m_currentTarget = findNearestPrey();
            if (m_currentTarget) {
                moveTowards(m_currentTarget->position(), 0.9f);

                // Jeśli ofiara jest daleko, patroluj
                float dx = m_currentTarget->position().x() - m_position.x();
                float dy = m_currentTarget->position().y() - m_position.y();
                float distance = qSqrt(dx * dx + dy * dy);

                if (distance > m_visionRange * 1.5f) {
                    m_currentTarget = nullptr;
                    wander();
                }
            } else {
                // Jeśli nie ma ofiar, unikaj bezcelowego krążenia w krzakach
                Environment* bush = findNearestBush();
                if (bush && QRandomGenerator::global()->bounded(100) < 30) {
                    // Czasami idź do krzaka, ale nie zawsze
                    moveTowards(bush->position(), 0.3f);
                    wander();
                } else {
                    // Więcej wędrówki, mniej skupiania się na krzakach
                    wander();
                    if (QRandomGenerator::global()->bounded(100) < 20) {
                        // Czasami celowo szukaj wody
                        Environment* water = findNearestWater();
                        if (water) {
                            moveTowards(water->position(), 0.2f);
                        }
                    }
                }
            }
        } else {
            // Szukaj wody
            Environment* water = findNearestWater();
            if (water) {
                moveTowards(water->position(), 0.7f);
                m_currentTarget = nullptr;
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
        // Krótkie zatrzymanie, potem kontynuuj
        m_direction *= 0.5f;
        if (m_stateTimer < 3) {
            // Pod koniec jedzenia/picia zacznij się rozglądać
            wander();
        }
        break;
    case FLEEING:
        // Jeśli uciekasz z wody, znajdź suchy ląd
        if (isInWater() && !m_canSwim) {
            Environment* nearestLand = nullptr;
            float nearestDistance = std::numeric_limits<float>::max();

            // Szukaj obszaru bez wody
            for (Environment* env : m_environment) {
                if (env->type() != Environment::WATER) {
                    float dx = env->position().x() - m_position.x();
                    float dy = env->position().y() - m_position.y();
                    float distance = qSqrt(dx * dx + dy * dy);
                    if (distance < nearestDistance) {
                        nearestDistance = distance;
                        nearestLand = env;
                    }
                }
            }

            if (nearestLand) {
                moveTowards(nearestLand->position(), 0.9f);
            } else {
                wander();
            }
        } else {
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

    // Szukaj partnera w pobliżu
    Organism* mate = findNearestMate();
    if (!mate || mate == this) return nullptr;

    float dx = mate->position().x() - m_position.x();
    float dy = mate->position().y() - m_position.y();
    float distance = qSqrt(dx * dx + dy * dy);

    // Muszą być blisko siebie i mieć terytorium
    if (distance < 40 && QRandomGenerator::global()->bounded(100) < m_reproductionRateGene) {
        // Sprawdź czy w pobliżu jest wystarczająco jedzenia (ofiar)
        int nearbyPrey = 0;
        for (Organism* prey : m_availablePrey) {
            if (prey->energy() > 0) {
                float pdx = prey->position().x() - m_position.x();
                float pdy = prey->position().y() - m_position.y();
                float pDistance = qSqrt(pdx * pdx + pdy * pdy);
                if (pDistance < 100) nearbyPrey++;
            }
        }

        if (nearbyPrey < 2) return nullptr; // Za mało jedzenia dla młodych

        m_energy -= 60;
        m_hydration -= 40;
        m_justReproduced = true;
        m_reproductionCooldown = 150;

        // Partner też płaci koszt
        mate->setEnergy(mate->energy() - 60);

        return new Predator(*static_cast<Predator*>(this), *static_cast<Predator*>(mate));
    }

    return nullptr;
}
