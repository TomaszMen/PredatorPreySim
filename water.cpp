#include "water.h"
#include <QRandomGenerator>

WaterSource::WaterSource(QPointF position, float radius, QObject *parent)
    : QObject(parent)
    , m_position(position)
    , m_radius(radius)
    , m_capacity(100.0f)
    , m_currentLevel(100.0f)
    , m_color(100, 200, 255, 100) // Niebieski, półprzezroczysty
{
}

void WaterSource::consume(float amount)
{
    if (m_currentLevel >= amount) {
        m_currentLevel -= amount;
    } else {
        m_currentLevel = 0;
    }
}

void WaterSource::regenerate(float amount)
{
    m_currentLevel = qMin(m_capacity, m_currentLevel + amount);
}

void WaterSource::update()
{
    // Regeneruj wodę z czasem
    regenerate(0.1f);
}
