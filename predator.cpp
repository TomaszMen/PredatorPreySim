#include "predator.h"
#include <QtMath>
#include <QDebug>
#include <QLineF>

Predator::Predator(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
{
    m_position = position;
    m_type = PREDATOR;
    m_color = QColor(255, 0, 0); // Czerwony dla drapieżników
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_age = 0;

    // Inicjalizacja genów
    m_speedGene = speed / 3.0f;
    m_sizeGene = size / 15.0f;
    m_visionGene = vision / 150.0f;
    m_reproductionRateGene = 1.0f;

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

    mutateGenes();

    m_speed = m_speedGene * 3.0f;
    m_size = m_sizeGene * 15.0f;
    m_visionRange = m_visionGene * 150.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_age = 0;

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Predator::update()
{
    if (m_energy <= 0) return;

    m_age++;
    m_energy -= 0.2f; // Drapieżniki zużywają więcej energii

    // Szukanie ofiary
    Prey* target = findNearestPrey();
    if (target) {
        QPointF directionToPrey = target->position() - m_position;
        float distance = qSqrt(directionToPrey.x() * directionToPrey.x() + directionToPrey.y() * directionToPrey.y());

        if (distance < m_visionRange && distance > 0) {
            directionToPrey /= distance;
            m_direction = directionToPrey;

            // Jeśli blisko, zjedz ofiarę
            if (distance < m_size + target->size()) {
                m_energy += 50.0f;
                if (m_energy > 200.0f) m_energy = 200.0f;
                target->setEnergy(0); // Zabij ofiarę
            }
        }
    } else {
        // Losowe przemieszczanie się
        QRandomGenerator *rand = QRandomGenerator::global();
        if (rand->bounded(100) < 10) {
            float angle = rand->bounded(360) * M_PI / 180.0f;
            m_direction = QPointF(qCos(angle), qSin(angle));
        }
    }

    // Ruch
    m_position += m_direction * m_speed;

    // Odbijanie od krawędzi
    if (m_position.x() < 0 || m_position.x() > 800) {
        m_direction.setX(-m_direction.x());
        m_position.setX(std::max(0.0f, std::min((float)m_position.x(), 800.0f)));
    }
    if (m_position.y() < 0 || m_position.y() > 600) {
        m_direction.setY(-m_direction.y());
        m_position.setY(std::max(0.0f, std::min((float)m_position.y(), 600.0f)));
    }

    // Aktualizacja koloru
    int red = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
    m_color = QColor(red, 0, 0);
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

Organism* Predator::reproduce()
{
    if (m_energy > 150 && QRandomGenerator::global()->bounded(100) < m_reproductionRateGene * 3) {
        m_energy -= 75; // Wyższy koszt reprodukcji
        return new Predator(*this, *this);
    }
    return nullptr;
}
