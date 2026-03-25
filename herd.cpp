#include "herd.h"
#include "prey.h"
#include "environment.h"
#include <cmath>
#include <QRandomGenerator>
#include <qdebug.h>

Herd::Herd(QObject *parent)
    : QObject(parent)
    , m_alpha(nullptr)
    , m_targetEnvironment(nullptr)
    , m_updateCounter(0)
    , m_targetPersistenceCounter(0)
    , m_targetPosition(-1, -1)
{
}

void Herd::addMember(Prey* prey)
{
    if (!prey || m_members.contains(prey))
        return;

    m_members.append(prey);

    if (m_members.size() == 1) {
        m_alpha = prey;
        emit alphaChanged(m_alpha);
        chooseNewTarget();
    }
    else if (calculateFitness(prey) > calculateFitness(m_alpha)) {
        m_alpha = prey;
        emit alphaChanged(m_alpha);
        chooseNewTarget();
    }
}

void Herd::removeMember(Prey* prey)
{
    int index = m_members.indexOf(prey);
    if (index == -1)
        return;

    m_members.removeAt(index);

    if (prey == m_alpha && !m_members.isEmpty()) {
        selectNewAlpha();
    }
}

void Herd::update()
{
    // Sprawdź czy stado nie jest w trakcie czyszczenia
    if (!m_alpha || m_alpha->energy() <= 0) {
        return;
    }

    // Sprawdź czy alfa nadal żyje
    if (m_alpha->energy() <= 0) {
        selectNewAlpha();
        if (!m_alpha) return;
    }

    m_updateCounter++;
    m_targetPersistenceCounter++;

    if (isTargetReached()) {
        emit targetReached();
        chooseNewTarget();
    }
    else if (m_targetPersistenceCounter >= TARGET_PERSISTENCE) {
        chooseNewTarget();
    }

    if (hasTarget() && m_alpha && m_alpha->energy() > 0) {
        if (m_alpha->canSwim() || !m_alpha->isInWater()) {
            m_alpha->moveTowardsTarget(m_targetPosition);
        }
    }

    emit herdMoved(getCenter());
}

void Herd::chooseNewTarget()
{
    if (!m_alpha || m_alpha->energy() <= 0) {
        return;
    }

    QRandomGenerator* rand = QRandomGenerator::global();

    // Hierarchia potrzeb alfy - z zabezpieczeniem przed nullptr
    if (m_alpha->energy() < 80.0f) {
        Environment* bush = m_alpha->findNearestBush();
        if (bush) {
            m_targetEnvironment = bush;
            m_targetPosition = bush->position();
            m_targetPersistenceCounter = 0;
            return;
        }
    }
    else if (m_alpha->hydration() < 70.0f) {
        Environment* water = m_alpha->findNearestWater();
        if (water) {
            m_targetEnvironment = water;
            m_targetPosition = water->position();
            m_targetPersistenceCounter = 0;
            return;
        }
    }

    // Losowa eksploracja
    m_targetPosition = QPointF(
        rand->bounded(500, 2500),
        rand->bounded(300, 1700)
        );
    m_targetEnvironment = nullptr;
    m_targetPersistenceCounter = 0;
}

bool Herd::isTargetReached() const
{
    if (!hasTarget() || !m_alpha)
        return false;

    float dx = m_targetPosition.x() - m_alpha->position().x();
    float dy = m_targetPosition.y() - m_alpha->position().y();
    float distance = qSqrt(dx * dx + dy * dy);

    if (m_targetEnvironment) {
        return distance < m_alpha->size() + m_targetEnvironment->size() + 20.0f;
    }

    return distance < 30.0f;
}

void Herd::setTargetPosition(const QPointF& target)
{
    m_targetPosition = target;
    m_targetEnvironment = nullptr;
    m_targetPersistenceCounter = 0;
}

void Herd::setTargetEnvironment(Environment* target)
{
    m_targetEnvironment = target;
    if (target) {
        m_targetPosition = target->position();
    }
    m_targetPersistenceCounter = 0;
}

void Herd::clearTarget()
{
    m_targetPosition = QPointF(-1, -1);
    m_targetEnvironment = nullptr;
}

QPointF Herd::getCenter() const
{
    if (m_members.isEmpty())
        return QPointF(0, 0);

    QPointF center(0, 0);
    for (Prey* prey : m_members) {
        if (prey->energy() > 0) {
            center += prey->position();
        }
    }
    return center / m_members.size();
}

void Herd::selectNewAlpha()
{
    if (m_members.isEmpty()) {
        m_alpha = nullptr;
        return;
    }

    // Usuń martwe ofiary z listy członków
    for (int i = m_members.size() - 1; i >= 0; --i) {
        if (!m_members[i] || m_members[i]->energy() <= 0) {
            m_members.removeAt(i);
        }
    }

    if (m_members.isEmpty()) {
        m_alpha = nullptr;
        return;
    }

    Prey* best = m_members.first();
    float bestFitness = calculateFitness(best);

    for (Prey* prey : m_members) {
        if (prey) {
            float fitness = calculateFitness(prey);
            if (fitness > bestFitness) {
                bestFitness = fitness;
                best = prey;
            }
        }
    }

    m_alpha = best;
    emit alphaChanged(m_alpha);
    chooseNewTarget();
}

float Herd::calculateFitness(Prey* prey) const
{
    if (!prey || prey->energy() <= 0)
        return -1.0f;

    float fitness = 0.0f;
    fitness += prey->speed() * 0.3f;
    fitness += prey->size() * 0.2f;
    fitness += prey->visionRange() * 0.2f;
    fitness += prey->energy() * 0.3f;

    return fitness;
}
