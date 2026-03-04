#include "environment.h"

Environment::Environment(QPointF position, EnvironmentType type, float size, QObject *parent)
    : QObject(parent), m_position(position), m_type(type), m_size(size)
{
    QRandomGenerator *rand = QRandomGenerator::global();

    switch(type) {
    case WATER:
        m_color = QColor(30, 144, 255, 180); // Niebieski z przezroczystością
        m_foodLevel = 0;
        m_maxFood = 0;
        break;
    case BUSH:
        m_color = QColor(34, 139, 34); // Zielony
        m_maxFood = 100.0f + rand->bounded(100);
        m_foodLevel = m_maxFood;
        break;
    default:
        m_color = QColor(240, 240, 220);
        m_foodLevel = 0;
        m_maxFood = 0;
    }
}

Environment::~Environment()
{
}

float Environment::consumeFood(float amount)
{
    if (m_type == BUSH) {
        float consumed = std::min(amount, m_foodLevel);
        m_foodLevel -= consumed;
        return consumed;
    }
    return 0.0f;
}

void Environment::regenerateFood(float rate)
{
    if (m_type == BUSH) {
        m_foodLevel = std::min(m_maxFood, m_foodLevel + rate);
        // Możliwa regeneracja krzaków
        if (m_foodLevel <= 0 && QRandomGenerator::global()->bounded(1000) < 1) {
            m_foodLevel = m_maxFood * 0.5f; // Odrastanie
        }
    }
}
