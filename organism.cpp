#include "organism.h"
#include "water.h"
#include "bush.h"
#include <QRandomGenerator>
#include <QDebug>

Organism::Organism(QObject *parent)
    : QObject(parent)
    , m_position(0, 0)
    , m_color(0, 0, 0)
    , m_type(PREY)
    , m_currentState(IDLE)
    , m_ageStage(BABY)
    , m_energy(100.0f)
    , m_speed(1.0f)
    , m_size(5.0f)
    , m_visionRange(50.0f)
    , m_age(0)
    , m_hunger(0.0f)
    , m_thirst(0.0f)
    , m_fatigue(0.0f)
    , m_reproductionUrge(0.0f)
    , m_pregnancyTimer(0)
    , m_pregnancyDuration(100)
    , m_partner(nullptr)
    , m_timeSinceLastReproduction(0)
    , m_speedGene(1.0f)
    , m_sizeGene(1.0f)
    , m_visionGene(1.0f)
    , m_reproductionRateGene(1.0f)
    , m_metabolismGene(1.0f)
    , m_staminaGene(1.0f)
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

        int geneToMutate = rand->bounded(6); // 6 genów
        switch (geneToMutate) {
        case 0: m_speedGene += mutation; break;
        case 1: m_sizeGene += mutation; break;
        case 2: m_visionGene += mutation; break;
        case 3: m_reproductionRateGene += mutation; break;
        case 4: m_metabolismGene += mutation; break;
        case 5: m_staminaGene += mutation; break;
        }

        // Ograniczenie wartości genów
        m_speedGene = std::max(0.5f, std::min(2.0f, m_speedGene));
        m_sizeGene = std::max(0.5f, std::min(2.0f, m_sizeGene));
        m_visionGene = std::max(0.5f, std::min(2.0f, m_visionGene));
        m_reproductionRateGene = std::max(0.5f, std::min(2.0f, m_reproductionRateGene));
        m_metabolismGene = std::max(0.5f, std::min(2.0f, m_metabolismGene));
        m_staminaGene = std::max(0.5f, std::min(2.0f, m_staminaGene));
    }
}

void Organism::drink(float amount)
{
    m_thirst -= amount;
    if (m_thirst < 0) m_thirst = 0;
    m_energy += amount * 0.5f;
    if (m_energy > 200.0f) m_energy = 200.0f;
}

void Organism::eat(float amount)
{
    m_hunger -= amount;
    if (m_hunger < 0) m_hunger = 0;
    m_energy += amount;
    if (m_energy > 200.0f) m_energy = 200.0f;
}

void Organism::rest(float amount)
{
    m_fatigue -= amount;
    if (m_fatigue < 0) m_fatigue = 0;
    m_energy += amount * 0.2f;
    if (m_energy > 200.0f) m_energy = 200.0f;
}

void Organism::ageOneStep()
{
    m_age++;

    // Zwiększ potrzeby z wiekiem
    m_hunger += 0.2f;
    m_thirst += 0.3f;
    m_fatigue += 0.1f;
    m_reproductionUrge += 0.05f;

    // Ogranicz wartości do zakresu 0-100
    if (m_hunger > 100.0f) m_hunger = 100.0f;
    if (m_thirst > 100.0f) m_thirst = 100.0f;
    if (m_fatigue > 100.0f) m_fatigue = 100.0f;
    if (m_reproductionUrge > 100.0f) m_reproductionUrge = 100.0f;

    // Zużywaj energię
    m_energy -= 0.1f * m_metabolismGene;
    if (m_energy < 0) m_energy = 0;
}

void Organism::updateNeeds()
{
    // Bazowe zużycie potrzeb
    m_hunger += 0.1f * m_metabolismGene;
    m_thirst += 0.2f * m_metabolismGene;
    m_fatigue += 0.05f * (2.0f - m_staminaGene);

    // Ogranicz wartości do zakresu 0-100
    if (m_hunger > 100.0f) m_hunger = 100.0f;
    if (m_thirst > 100.0f) m_thirst = 100.0f;
    if (m_fatigue > 100.0f) m_fatigue = 100.0f;

    // Zużywaj energię
    m_energy -= 0.05f * m_metabolismGene;
    if (m_energy < 0) m_energy = 0;
}

bool Organism::canReproduce() const
{
    return m_ageStage == ADULT &&
           m_energy > 120.0f &&
           m_hunger < 60.0f &&
           m_thirst < 60.0f &&
           !isPregnant() &&
           m_timeSinceLastReproduction > 100;
}

void Organism::startPregnancy()
{
    m_pregnancyTimer = 1;
    m_currentState = PREGNANT;
}

void Organism::updatePregnancy()
{
    if (m_pregnancyTimer > 0) {
        m_pregnancyTimer++;
        if (m_pregnancyTimer > m_pregnancyDuration) {
            m_pregnancyTimer = 0;
            m_currentState = CARING_FOR_YOUNG;
        }
    }
}

void Organism::updateAgeStage()
{
    if (m_age < 50) {
        m_ageStage = BABY;
        m_speed *= 0.5f;
    } else if (m_age < 200) {
        m_ageStage = YOUNG;
        m_speed *= 0.8f;
    } else if (m_age < 500) {
        m_ageStage = ADULT;
        // W wieku dorosłym przywróć normalną prędkość
        if (m_type == PREY) {
            m_speed = m_speedGene * 2.0f;
        } else {
            m_speed = m_speedGene * 3.0f;
        }
    } else {
        m_ageStage = ELDER;
        m_speed *= 0.7f;
        m_visionRange *= 0.8f;
    }
}
