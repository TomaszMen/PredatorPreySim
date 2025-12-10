#include "bush.h"
#include <QRandomGenerator>

Bush::Bush(QPointF position, float radius, QObject *parent)
    : QObject(parent)
    , m_position(position)
    , m_radius(radius)
    , m_maxFood(50.0f)
    , m_foodAmount(50.0f)
    , m_color(34, 139, 34, 150) // Zielony, półprzezroczysty
    , m_regenerationTimer(0)
{
}

float Bush::consume(float amount)
{
    if (m_foodAmount >= amount) {
        m_foodAmount -= amount;
        return amount;
    } else {
        float remaining = m_foodAmount;
        m_foodAmount = 0;
        return remaining;
    }
}

void Bush::regenerate(float amount)
{
    if (m_foodAmount < m_maxFood) {
        m_foodAmount = qMin(m_maxFood, m_foodAmount + amount);
    }
}

void Bush::update()
{
    m_regenerationTimer++;
    if (m_regenerationTimer >= 100) { // Co 100 cykli regeneruj
        regenerate(1.0f);
        m_regenerationTimer = 0;
    }
}
