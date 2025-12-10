#include "organism.h"
#include <QRandomGenerator>
#include <QDebug>

Organism::Organism(QObject *parent)
    : QObject(parent)
    , m_position(0, 0)
    , m_energy(100.0f)
    , m_speed(1.0f)
    , m_size(5.0f)
    , m_visionRange(50.0f)
    , m_age(0)
    , m_speedGene(1.0f)
    , m_sizeGene(1.0f)
    , m_visionGene(1.0f)
    , m_reproductionRateGene(1.0f)
{
}

Organism::~Organism()
{
}

void Organism::mutateGenes()
{
    QRandomGenerator *rand = QRandomGenerator::global();

    // Mutacja z małym prawdopodobieństwem
    if (rand->bounded(100) < 5) { // 5% szansy na mutację
        float mutation = (rand->bounded(200) - 100) / 1000.0f; // +/- 10%

        int geneToMutate = rand->bounded(4);
        switch (geneToMutate) {
        case 0: m_speedGene += mutation; break;
        case 1: m_sizeGene += mutation; break;
        case 2: m_visionGene += mutation; break;
        case 3: m_reproductionRateGene += mutation; break;
        }

        // Ograniczenie wartości genów
        m_speedGene = std::max(0.5f, std::min(2.0f, m_speedGene));
        m_sizeGene = std::max(0.5f, std::min(2.0f, m_sizeGene));
        m_visionGene = std::max(0.5f, std::min(2.0f, m_visionGene));
        m_reproductionRateGene = std::max(0.5f, std::min(2.0f, m_reproductionRateGene));
    }
}
