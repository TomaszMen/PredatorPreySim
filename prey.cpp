#include "prey.h"
#include <QtMath>
#include <QDebug>
#include "herd.h"

Prey::Prey(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
    , m_mateTarget(nullptr)
    , m_canSwim(false)
    , m_herd(nullptr)
    , m_herdAttraction(0.5f)
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
    , m_canSwim(false)
    , m_herd(nullptr)
    , m_herdAttraction(0.5f)
{
    m_type = PREY;
    m_color = QColor(0, 255, 0);

    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;
    m_hydrationRateGene = (parent1.m_hydrationRateGene + parent2.m_hydrationRateGene) / 2.0f;

    mutateGenes();

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

    m_energy -= 0.03f * (1.0f + m_speed / 5.0f) * s_energyConsumptionFactor;
    m_hydration -= 0.03f * m_hydrationRateGene * s_energyConsumptionFactor;

    if (isInWater() && !m_canSwim) {
        m_energy -= 0.05f;
        m_state = FLEEING;
    }

    updateNeeds();
    updateAI();

    executeState();

    float currentSpeed = m_speed;
    if (m_state == RESTING) {
        currentSpeed *= 0.7f;
    } else if (m_state == FLEEING && m_fearLevel > 0.5f) {
        currentSpeed *= 1.8f;
    }

    m_position += m_direction * currentSpeed;
    applyBoundaries();

    if (isInWater() && !m_canSwim) {
        m_position += m_direction * (currentSpeed * 0.5f);
    }

    int green = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
    int blue = std::max(0, std::min(255, (int)(255 * (m_hydration / 100.0f))));
    m_color = QColor(0, green, blue);

    if (m_reproductionCooldown > 0) m_reproductionCooldown--;
    if (m_reproductionCooldown == 0) m_justReproduced = false;
}

void Prey::updateAI()
{
    m_fearLevel = 0;
    m_thirstLevel = 0;
    m_hungerLevel = 0;
    m_matingUrge = 0;

    Organism* predator = findNearestPredator();
    if (predator) {
        float dx = predator->position().x() - m_position.x();
        float dy = predator->position().y() - m_position.y();
        float distance = qSqrt(dx * dx + dy * dy);
        m_fearLevel = 1.0f - (distance / (m_visionRange * 1.5f));
        m_fearLevel = std::max(0.0f, std::min(1.0f, m_fearLevel));
    }

    m_thirstLevel = 1.0f - (m_hydration / 100.0f);
    if (m_lastDrinkTime > 200) m_thirstLevel = 1.0f;

    m_hungerLevel = 1.0f - (m_energy / 200.0f);
    if (m_lastMealTime > 150) m_hungerLevel = 1.0f;

    if (canMate() && m_age > 100) {
        m_matingUrge = 0.7f;
    }
}

void Prey::updateNeeds()
{
    for (Environment* env : m_environment) {
        if (env->type() == Environment::BUSH) {
            float dx = env->position().x() - m_position.x();
            float dy = env->position().y() - m_position.y();
            float distance = qSqrt(dx * dx + dy * dy);

            if (distance < m_size + env->size()) {
                float foodTaken = env->consumeFood(10.0f);
                if (foodTaken > 0) {
                    m_energy += foodTaken * 3.0f;
                    m_energy = std::min(200.0f, m_energy);
                    m_lastMealTime = 0;
                    m_state = EATING;
                    m_stateTimer = 5;
                }
            }
        }

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

    if (m_state == RESTING && m_stateTimer > 0) {
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
    else if (m_energy > 150.0f && m_hydration > 80.0f && m_fearLevel < 0.1f) {
        m_state = RESTING;
        m_stateTimer = 30 + QRandomGenerator::global()->bounded(50);
    }
    else {
        m_state = WANDERING;
        m_stateTimer = 15 + QRandomGenerator::global()->bounded(20);
    }
}

void Prey::executeState()
{
    decideState();

    if (m_herd && m_herd->getAlpha() && m_state != FLEEING && m_state != MATING) {
        if (isAlpha()) {
            if (m_herd->hasTarget()) {
                QPointF target = m_herd->getTargetPosition();
                float dx = target.x() - m_position.x();
                float dy = target.y() - m_position.y();
                float distance = qSqrt(dx * dx + dy * dy);

                if (distance > 30.0f) {
                    moveTowards(target, 0.3f);
                } else {
                    wander();
                }
            } else {
                wander();
            }
        } else {
            followAlpha();
        }
    }

    switch(m_state) {
    case FLEEING: {
        Organism* predator = findNearestPredator();
        if (predator) {
            moveAwayFrom(predator->position(), 0.8f);
        } else {
            // Jeśli nie ma drapieżnika, ale uciekaliśmy, wróć do stada
            if (m_herd && m_herd->getAlpha() && !isAlpha()) {
                followAlpha();
            } else if (m_herd && m_herd->getAlpha() && isAlpha()) {
                // Alfa wraca do celu
                if (m_herd->hasTarget()) {
                    moveTowards(m_herd->getTargetPosition(), 0.5f);
                } else {
                    wander();
                }
            } else {
                wander();
            }
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
            if (water) moveTowards(water->position(), 0.6f);
            else if (m_herd && m_herd->getAlpha()) followAlpha();
            else wander();
        } else {
            Environment* bush = findNearestBush();
            if (bush) moveTowards(bush->position(), 0.6f);
            else if (m_herd && m_herd->getAlpha()) followAlpha();
            else wander();
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
            if (m_herd && m_herd->getAlpha()) followAlpha();
            else wander();
        }
        break;
    }
    case EATING:
    case DRINKING:
        m_direction *= 0.3f;
        break;
    case RESTING:
        m_direction *= 0.5f;
        break;
    case WANDERING:
    default:
        if (m_herd && m_herd->getAlpha() && !isAlpha()) {
            followAlpha();
        } else {
            wander();
            if (m_herd && QRandomGenerator::global()->bounded(500) < 1) {
                m_herd->removeMember(this);
                m_herd = nullptr;
            }
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
    float distance = qSqrt(dx*dx + dy*dy);

    if (distance < 25 && QRandomGenerator::global()->bounded(100) <
                             m_reproductionRateGene * 5 * s_preyReproductionFactor) {
        m_energy -= 20;
        m_hydration -= 15;
        m_justReproduced = true;
        m_reproductionCooldown = 40;

        mate->setEnergy(mate->energy() - 20);
        return new Prey(*static_cast<Prey*>(this), *static_cast<Prey*>(mate));
    }
    return nullptr;
}

QPointF Prey::getHerdCenter                                 ()
{
    QPointF center(0,0);
    int count = 0;
    for (Organism* org : m_availablePrey) {
        if (org != this && org->energy() > 0) {
            float dx = org->position().x() - m_position.x();
            float dy = org->position().y() - m_position.y();
            float distance = qSqrt(dx*dx + dy*dy);
            if (distance < m_visionRange) {
                center += org->position();
                count++;
            }
        }
    }
    if (count > 0) return center / count;
    return m_position;
}

void Prey::followAlpha()
{
    if (!m_herd || !m_herd->getAlpha() || m_herd->getAlpha() == this)
        return;

    Prey* alpha = m_herd->getAlpha();
    if (alpha->energy() <= 0)
        return;

    float dx = alpha->position().x() - m_position.x();
    float dy = alpha->position().y() - m_position.y();
    float distance = qSqrt(dx * dx + dy * dy);

    // Optymalna odległość - większa dla lepszego rozproszenia
    float optimalDistance = m_size + alpha->size() + 40.0f;

    if (distance > optimalDistance * 1.3f) {
        // Za daleko - idź w kierunku alfy
        moveTowards(alpha->position(), 0.35f);
    }
    else if (distance < optimalDistance * 0.7f) {
        // Za blisko - odejdź, ale nie wokół koła
        // Odchodź w kierunku prostopadłym do kierunku do alfy
        QPointF awayDir(-dy, dx);  // Wektor prostopadły
        float len = qSqrt(awayDir.x() * awayDir.x() + awayDir.y() * awayDir.y());
        if (len > 0) {
            awayDir /= len;
            // Połącz z kierunkiem ucieczki od alfy
            QPointF fleeFromAlpha = m_position - alpha->position();
            len = qSqrt(fleeFromAlpha.x() * fleeFromAlpha.x() + fleeFromAlpha.y() * fleeFromAlpha.y());
            if (len > 0) {
                fleeFromAlpha /= len;
            }
            // Średnia ważona: 70% od alfy, 30% prostopadle
            QPointF finalDir = fleeFromAlpha * 0.7f + awayDir * 0.3f;
            len = qSqrt(finalDir.x() * finalDir.x() + finalDir.y() * finalDir.y());
            if (len > 0) {
                finalDir /= len;
                m_direction = m_direction * 0.6f + finalDir * 0.4f;
                len = qSqrt(m_direction.x() * m_direction.x() + m_direction.y() * m_direction.y());
                if (len > 0) {
                    m_direction /= len;
                }
            }
        }
    }
    else {
        // W optymalnej odległości - utrzymuj kierunek podobny do alfy,
        // ale z pewną losowością
        float alphaAngle = qAtan2(alpha->direction().y(), alpha->direction().x());
        float myAngle = qAtan2(m_direction.y(), m_direction.x());

        // Nie kopiuj dokładnie, tylko zbliżaj się do kierunku alfy
        float angleDiff = alphaAngle - myAngle;
        // Ogranicz zmianę kierunku
        float maxTurn = 0.05f;
        if (angleDiff > maxTurn) angleDiff = maxTurn;
        if (angleDiff < -maxTurn) angleDiff = -maxTurn;

        float newAngle = myAngle + angleDiff * 0.3f;
        m_direction = QPointF(qCos(newAngle), qSin(newAngle));

        // Dodaj losowe fluktuacje dla naturalnego wyglądu
        QRandomGenerator* rand = QRandomGenerator::global();
        if (rand->bounded(100) < 15) {
            float randomAngle = (rand->bounded(40) - 20) * M_PI / 180.0f;
            newAngle = qAtan2(m_direction.y(), m_direction.x()) + randomAngle;
            m_direction = QPointF(qCos(newAngle), qSin(newAngle));
        }
    }
}

bool Prey::isAlpha() const
{
    return m_herd && m_herd->getAlpha() == this;
}

void Prey::moveTowardsTarget(const QPointF& target)
{
    moveTowards(target, 0.4f);
}
