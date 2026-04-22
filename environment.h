#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QRandomGenerator>

class Organism;

class Environment : public QObject
{
    Q_OBJECT
public:
    enum EnvironmentType { WATER, BUSH, EMPTY };

    Environment(QPointF position, EnvironmentType type, float size, QObject *parent = nullptr);
    ~Environment();

    QPointF position() const { return m_position; }
    EnvironmentType type() const { return m_type; }
    float size() const { return m_size; }
    QColor color() const { return m_color; }

    float foodLevel() const { return m_foodLevel; }
    float consumeFood(float amount);
    void regenerateFood(float rate);

    bool isWalkable() const { return m_type == WATER; }

private:
    QPointF m_position;
    EnvironmentType m_type;
    float m_size;
    QColor m_color;
    float m_foodLevel;
    float m_maxFood;
};

#endif // ENVIRONMENT_H
