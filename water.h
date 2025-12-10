#ifndef WATER_H
#define WATER_H

#include <QObject>
#include <QPointF>
#include <QColor>

class WaterSource : public QObject
{
    Q_OBJECT
public:
    explicit WaterSource(QPointF position, float radius = 50.0f, QObject *parent = nullptr);

    QPointF position() const { return m_position; }
    float radius() const { return m_radius; }
    float capacity() const { return m_capacity; }
    float currentLevel() const { return m_currentLevel; }
    bool isAvailable() const { return m_currentLevel > 0; }

    void consume(float amount);
    void regenerate(float amount);
    void update();

private:
    QPointF m_position;
    float m_radius;
    float m_capacity;
    float m_currentLevel;
    QColor m_color;
};

#endif // WATER_H
