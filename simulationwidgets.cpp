#include "simulationwidgets.h"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QDebug>

SimulationWidget::SimulationWidget(QWidget *parent)
    : QWidget(parent)
    , m_isRunning(false)
    , m_maxPopulation(0)
    , m_foodRegenerationRate(0.5f)
    , m_preyReproductionRate(1.0f)
    , m_predatorReproductionRate(1.0f)
    , m_energyConsumptionRate(1.0f)
{
    setFixedSize(800, 600);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::updateSimulation);

    initializeSimulation();

    // Przyciski kontrolne
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_startButton = new QPushButton("Start", this);
    m_pauseButton = new QPushButton("Pause", this);
    m_resetButton = new QPushButton("Reset", this);

    connect(m_startButton, &QPushButton::clicked, this, &SimulationWidget::startSimulation);
    connect(m_pauseButton, &QPushButton::clicked, this, &SimulationWidget::pauseSimulation);
    connect(m_resetButton, &QPushButton::clicked, this, &SimulationWidget::resetSimulation);

    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_resetButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

SimulationWidget::~SimulationWidget()
{
    qDeleteAll(m_prey);
    qDeleteAll(m_predators);
}

void SimulationWidget::initializeSimulation()
{
    qDeleteAll(m_prey);
    qDeleteAll(m_predators);
    m_prey.clear();
    m_predators.clear();

    m_preyHistory.clear();
    m_predatorHistory.clear();

    // Początkowa populacja
    addPrey(20);
    addPredator(5);

    m_maxPopulation = m_prey.size() + m_predators.size();
    updateStatistics();
}

void SimulationWidget::addPrey(int count)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        float speed = 1.0f + (rand->bounded(100) / 100.0f);
        float size = 4.0f + (rand->bounded(60) / 10.0f);
        float vision = 30.0f + rand->bounded(70);

        Prey *prey = new Prey(
            QPointF(rand->bounded(width()), rand->bounded(height())),
            speed,
            size,
            vision
            );
        m_prey.append(prey);
    }
}

void SimulationWidget::addPredator(int count)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        float speed = 1.2f + (rand->bounded(100) / 100.0f);
        float size = 6.0f + (rand->bounded(80) / 10.0f);
        float vision = 60.0f + rand->bounded(90);

        Predator *predator = new Predator(
            QPointF(rand->bounded(width()), rand->bounded(height())),
            speed,
            size,
            vision
            );
        m_predators.append(predator);
    }
}

void SimulationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Tło
    painter.fillRect(rect(), QColor(240, 240, 220));

    // Rysowanie ofiar
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            painter.setBrush(prey->color());
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(prey->position(), prey->size(), prey->size());
        }
    }

    // Rysowanie drapieżników
    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            painter.setBrush(predator->color());
            painter.setPen(Qt::black);
            painter.drawEllipse(predator->position(), predator->size(), predator->size());

            // Rysowanie oczu dla drapieżników
            painter.setBrush(Qt::white);
            painter.drawEllipse(predator->position() + QPointF(3, -3), 2, 2);
            painter.drawEllipse(predator->position() + QPointF(-3, -3), 2, 2);
        }
    }

    // Statystyki na ekranie
    painter.setPen(Qt::black);
    painter.drawText(10, 20, QString("Ofiary: %1").arg(m_prey.size()));
    painter.drawText(10, 40, QString("Drapieżniki: %1").arg(m_predators.size()));
    painter.drawText(10, 60, QString("Maks. populacja: %1").arg(m_maxPopulation));
}

void SimulationWidget::updateSimulation()
{
    // Aktualizuj drapieżniki (przekaż listę ofiar)
    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            predator->setAvailablePrey(m_prey);
            predator->update();
        }
    }

    // Aktualizuj ofiary
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            prey->update();
        }
    }

    // Usuń martwe organizmy
    removeDeadOrganisms();

    // Reprodukcja
    reproduceOrganisms();

    // Aktualizuj statystyki
    updateStatistics();

    update();
}

void SimulationWidget::removeDeadOrganisms()
{
    // Usuń martwe ofiary
    auto preyIt = m_prey.begin();
    while (preyIt != m_prey.end()) {
        if ((*preyIt)->energy() <= 0 || (*preyIt)->age() > 1000) {
            delete *preyIt;
            preyIt = m_prey.erase(preyIt);
        } else {
            ++preyIt;
        }
    }

    // Usuń martwe drapieżniki
    auto predatorIt = m_predators.begin();
    while (predatorIt != m_predators.end()) {
        if ((*predatorIt)->energy() <= 0 || (*predatorIt)->age() > 800) {
            delete *predatorIt;
            predatorIt = m_predators.erase(predatorIt);
        } else {
            ++predatorIt;
        }
    }
}

void SimulationWidget::reproduceOrganisms()
{
    // Reprodukcja ofiar
    QVector<Prey*> newPrey;
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            Organism *offspring = prey->reproduce();
            if (offspring) {
                Prey *newPrey = qobject_cast<Prey*>(offspring);
                if (newPrey) {
                    this->m_prey.append(newPrey);
                }
            }
        }
    }

    // Reprodukcja drapieżników
    QVector<Predator*> newPredators;
    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            Organism *offspring = predator->reproduce();
            if (offspring) {
                Predator *newPredator = qobject_cast<Predator*>(offspring);
                if (newPredator) {
                    this->m_predators.append(newPredator);
                }
            }
        }
    }
}

void SimulationWidget::updateStatistics()
{
    int totalPrey = m_prey.size();
    int totalPredators = m_predators.size();

    m_preyHistory.append(totalPrey);
    m_predatorHistory.append(totalPredators);

    m_maxPopulation = qMax(m_maxPopulation, totalPrey + totalPredators);

    // Ogranicz historię do 1000 punktów
    if (m_preyHistory.size() > 1000) {
        m_preyHistory.removeFirst();
        m_predatorHistory.removeFirst();
    }
}

void SimulationWidget::startSimulation()
{
    if (!m_isRunning) {
        m_timer->start(50); // 20 FPS
        m_isRunning = true;
    }
}

void SimulationWidget::pauseSimulation()
{
    if (m_isRunning) {
        m_timer->stop();
        m_isRunning = false;
    }
}

void SimulationWidget::resetSimulation()
{
    pauseSimulation();
    initializeSimulation();
    update();
}

void SimulationWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // Można dodać logikę dostosowania pozycji organizmów
}
