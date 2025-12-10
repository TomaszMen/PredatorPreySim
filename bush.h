#ifndef BUSH_H
#define BUSH_H

#include <QObject>
#include <QPointF>
#include <QColor>

class Bush : public QObject
{
    Q_OBJECT
public:
    explicit Bush(QPointF position, float radius = 40.0f, QObject *parent = nullptr);

    QPointF position() const { return m_position; }
    float radius() const { return m_radius; }
    float foodAmount() const { return m_foodAmount; }
    float maxFood() const { return m_maxFood; }
    bool hasFood() const { return m_foodAmount > 0; }

    float consume(float amount);
    void regenerate(float amount);
    void update();

private:
    QPointF m_position;
    float m_radius;
    float m_foodAmount;
    float m_maxFood;
    QColor m_color;
    int m_regenerationTimer;
};

#endif // BUSH_H
