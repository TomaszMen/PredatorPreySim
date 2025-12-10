#include "prey.h"
#include <QtMath>
#include <QDebug>
#include <QLineF>

Prey::Prey(QPointF position, float speed, float size, float vision, QObject *parent)
    : Organism(parent)
{
    m_position = position;
    m_type = PREY;
    m_color = QColor(0, 255, 0); // Zielony dla ofiar
    m_speed = speed;
    m_size = size;
    m_visionRange = vision;
    m_energy = 100.0f;
    m_age = 0;

    // Inicjalizacja genów
    m_speedGene = speed / 2.0f; // Normalizacja
    m_sizeGene = size / 10.0f;
    m_visionGene = vision / 100.0f;
    m_reproductionRateGene = 1.0f;

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

    // Dziedziczenie cech - średnia z rodziców
    m_speedGene = (parent1.m_speedGene + parent2.m_speedGene) / 2.0f;
    m_sizeGene = (parent1.m_sizeGene + parent2.m_sizeGene) / 2.0f;
    m_visionGene = (parent1.m_visionGene + parent2.m_visionGene) / 2.0f;
    m_reproductionRateGene = (parent1.m_reproductionRateGene + parent2.m_reproductionRateGene) / 2.0f;

    // Mutacja
    mutateGenes();

    // Ustawienie cech fenotypowych na podstawie genów
    m_speed = m_speedGene * 2.0f;
    m_size = m_sizeGene * 10.0f;
    m_visionRange = m_visionGene * 100.0f;

    m_position = parent1.m_position;
    m_energy = 100.0f;
    m_age = 0;

    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    m_direction = QPointF(qCos(angle), qSin(angle));
}

void Prey::update()
{
    if (m_energy <= 0) return;

    m_age++;
    m_energy -= 0.1f; // Podstawowe zużycie energii

    // Szukanie jedzenia i unikanie drapieżników
    QPointF foodDirection = findFood();
    avoidPredators();

    // Łączenie kierunków
    QPointF desiredDirection = m_direction + foodDirection * 0.5;
    float length = qSqrt(desiredDirection.x() * desiredDirection.x() + desiredDirection.y() * desiredDirection.y());
    if (length > 0) {
        desiredDirection /= length;
        m_direction = desiredDirection;
    }

    // Ruch
    m_position += m_direction * m_speed;

    // Odbijanie od krawędzi
    QRandomGenerator *rand = QRandomGenerator::global();
    if (m_position.x() < 0 || m_position.x() > 800) {
        m_direction.setX(-m_direction.x());
        m_position.setX(std::max(0.0f, std::min((float)m_position.x(), 800.0f)));
    }
    if (m_position.y() < 0 || m_position.y() > 600) {
        m_direction.setY(-m_direction.y());
        m_position.setY(std::max(0.0f, std::min((float)m_position.y(), 600.0f)));
    }

    // Spożywanie jedzenia (symulowane)
    if (rand->bounded(100) < 5) { // 5% szansy na znalezienie jedzenia
        m_energy += 20.0f;
        if (m_energy > 200.0f) m_energy = 200.0f;
    }

    // Aktualizacja koloru na podstawie energii
    int green = std::max(0, std::min(255, (int)(255 * (m_energy / 200.0f))));
    m_color = QColor(0, green, 0);
}

QPointF Prey::findFood()
{
    // Symulowane szukanie jedzenia - losowy kierunek
    QRandomGenerator *rand = QRandomGenerator::global();
    float angle = rand->bounded(360) * M_PI / 180.0f;
    return QPointF(qCos(angle), qSin(angle));
}

void Prey::avoidPredators()
{
    // W rzeczywistej implementacji, to sprawdzałoby pobliskie drapieżniki
    QRandomGenerator *rand = QRandomGenerator::global();
    if (rand->bounded(100) < 10) { // 10% szansy na zmianę kierunku
        float angle = rand->bounded(360) * M_PI / 180.0f;
        m_direction = QPointF(qCos(angle), qSin(angle));
    }
}

Organism* Prey::reproduce()
{
    if (m_energy > 150 && QRandomGenerator::global()->bounded(100) < m_reproductionRateGene * 5) {
        m_energy -= 50; // Koszt reprodukcji
        return new Prey(*this, *this); // Klonowanie z mutacją
    }
    return nullptr;
}
