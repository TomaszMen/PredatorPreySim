#ifndef ORGANISM_H
#define ORGANISM_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QtMath>
#include <algorithm>

class Organism : public QObject
{
    Q_OBJECT

public:
    enum OrganismType { PREY, PREDATOR };

    Organism(QObject *parent = nullptr);
    virtual ~Organism();

    QPointF position() const { return m_position; }
    QColor color() const { return m_color; }
    OrganismType type() const { return m_type; }
    float energy() const { return m_energy; }
    float speed() const { return m_speed; }
    float size() const { return m_size; }
    float visionRange() const { return m_visionRange; }
    int age() const { return m_age; }

    void setPosition(const QPointF &pos) { m_position = pos; }
    void setEnergy(float energy) { m_energy = energy; }
    virtual void update() = 0;
    virtual Organism* reproduce() = 0;

protected:
    QPointF m_position;
    QColor m_color;
    OrganismType m_type;
    float m_energy;
    float m_speed;
    float m_size;
    float m_visionRange;
    int m_age;

    // Genetyczne cechy
    float m_speedGene;
    float m_sizeGene;
    float m_visionGene;
    float m_reproductionRateGene;

    void mutateGenes();
};

#endif // ORGANISM_H
