#include "simulationwidgets.h"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>

SimulationWidget::SimulationWidget(QWidget *parent)
    : QWidget(parent)
    , m_cameraPosition(1000, 750)
    , m_zoom(1.0f)
    , m_isDragging(false)
    , m_isRunning(false)
    , m_generation(0)
    , m_totalBirths(0)
    , m_totalDeaths(0)
    , m_maxPopulation(0)
    , m_foodRegenerationRate(2.0f)
    , m_preyReproductionRate(1.0f)
    , m_predatorReproductionRate(1.0f)
    , m_energyConsumptionRate(1.0f)
    , m_mutationRate(5.0f)
{
    setFixedSize(800, 600);
    setMouseTracking(true);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::updateSimulation);
    m_timer->setInterval(50); // 20 FPS

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
    qDeleteAll(m_environment);
}

void SimulationWidget::initializeSimulation()
{
    qDeleteAll(m_prey);
    qDeleteAll(m_predators);
    qDeleteAll(m_environment);

    m_prey.clear();
    m_predators.clear();
    m_environment.clear();

    m_preyHistory.clear();
    m_predatorHistory.clear();
    m_birthHistory.clear();
    m_deathHistory.clear();

    m_generation = 0;
    m_totalBirths = 0;
    m_totalDeaths = 0;

    // Generuj środowisko
    generateEnvironment();

    // Początkowa populacja
    addPrey(120);
    addPredator(15);

    m_maxPopulation = m_prey.size() + m_predators.size();
    updateStatistics();
}

void SimulationWidget::generateEnvironment()
{
    QRandomGenerator *rand = QRandomGenerator::global();

    // Generuj jeziora (woda)
    for (int i = 0; i < 5; ++i) {
        float radius = 50 + rand->bounded(100);
        QPointF position(rand->bounded(3000), rand->bounded(2000));
        m_environment.append(new Environment(position, Environment::WATER, radius));
    }

    // Generuj rzekę
    for (int i = 0; i < 10; ++i) {
        float radius = 20 + rand->bounded(30);
        QPointF position(200 + i * 160, 400 + rand->bounded(200) - 100);
        m_environment.append(new Environment(position, Environment::WATER, radius));
    }

    // Generuj krzaki (jedzenie)
    for (int i = 0; i < 50; ++i) {
        float size = 10 + rand->bounded(20);
        QPointF position(rand->bounded(3000), rand->bounded(2000));

        // Unikaj umieszczania krzaków w wodzie
        bool tooCloseToWater = false;
        for (Environment* env : m_environment) {
            if (env->type() == Environment::WATER) {
                float dx = env->position().x() - position.x();
                float dy = env->position().y() - position.y();
                float distance = qSqrt(dx * dx + dy * dy);
                if (distance < env->size() + 30) {
                    tooCloseToWater = true;
                    break;
                }
            }
        }

        if (!tooCloseToWater) {
            m_environment.append(new Environment(position, Environment::BUSH, size));
        }
    }
}

void SimulationWidget::addPrey(int count)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        float speed = 0.8f + (rand->bounded(120) / 100.0f);
        float size = 4.0f + (rand->bounded(80) / 10.0f);
        float vision = 40.0f + rand->bounded(80);

        // Znajdź pozycję nie w wodzie
        QPointF position;
        int attempts = 0;
        do {
            position = QPointF(rand->bounded(2000), rand->bounded(1500));
            attempts++;

            bool inWater = false;
            for (Environment* env : m_environment) {
                if (env->type() == Environment::WATER) {
                    float dx = env->position().x() - position.x();
                    float dy = env->position().y() - position.y();
                    float distance = qSqrt(dx * dx + dy * dy);
                    if (distance < env->size()) {
                        inWater = true;
                        break;
                    }
                }
            }

            if (!inWater || attempts > 50) break;
        } while (true);

        Prey *prey = new Prey(position, speed, size, vision);
        m_prey.append(prey);
    }
}

void SimulationWidget::addPredator(int count)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        float speed = 1.2f + (rand->bounded(150) / 100.0f);
        float size = 6.0f + (rand->bounded(100) / 10.0f);
        float vision = 60.0f + rand->bounded(120);

        QPointF position(rand->bounded(2000), rand->bounded(1500));
        Predator *predator = new Predator(position, speed, size, vision);
        m_predators.append(predator);
    }
}

QPointF SimulationWidget::screenToWorld(const QPoint &screenPos) const
{
    float worldX = (screenPos.x() - width() / 2.0f) / m_zoom + m_cameraPosition.x();
    float worldY = (screenPos.y() - height() / 2.0f) / m_zoom + m_cameraPosition.y();
    return QPointF(worldX, worldY);
}

QPoint SimulationWidget::worldToScreen(const QPointF &worldPos) const
{
    int screenX = (worldPos.x() - m_cameraPosition.x()) * m_zoom + width() / 2.0f;
    int screenY = (worldPos.y() - m_cameraPosition.y()) * m_zoom + height() / 2.0f;
    return QPoint(screenX, screenY);
}

void SimulationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Transformacja kamery
    painter.translate(width() / 2.0f, height() / 2.0f);
    painter.scale(m_zoom, m_zoom);
    painter.translate(-m_cameraPosition.x(), -m_cameraPosition.y());

    // Tło świata
    painter.fillRect(0, 0, 3000, 2000, QColor(240, 240, 220)); // Powiększony świat

    // Rysowanie środowiska
    for (Environment* env : m_environment) {
        painter.setBrush(env->color());

        if (env->type() == Environment::WATER) {
            painter.setPen(QColor(30, 144, 255, 100));
            painter.drawEllipse(env->position(), env->size(), env->size());
        } else if (env->type() == Environment::BUSH) {
            painter.setPen(QColor(34, 139, 34, 150));

            // Rysuj krzak z poziomem jedzenia
            float foodRatio = env->foodLevel() / 100.0f;
            QColor bushColor = env->color();
            bushColor.setAlpha(100 + (int)(155 * foodRatio));
            painter.setBrush(bushColor);

            painter.drawEllipse(env->position(), env->size(), env->size());

            // Jeśli ma mało jedzenia, rysuj "wyschnięty" krzak
            if (foodRatio < 0.3f) {
                painter.setPen(QColor(139, 69, 19, 200));
                painter.drawEllipse(env->position(), env->size() * 0.7f, env->size() * 0.7f);
            }
        }
    }

    // Rysowanie ofiar (trójkąty skierowane w kierunku ruchu)
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            painter.setBrush(prey->color());
            painter.setPen(QPen(Qt::darkGreen, 1));

            QPointF pos = prey->position();
            float size = prey->size();

            // Oblicz kierunek (kąt) z wektora kierunku
            float angle = qAtan2(prey->direction().y(), prey->direction().x());

            // Tworzymy trójkąt skierowany w kierunku ruchu
            QPolygonF triangle;
            triangle << pos + QPointF(qCos(angle) * size, qSin(angle) * size); // Przód
            triangle << pos + QPointF(qCos(angle + 2.0 * M_PI / 3.0) * size * 0.7,
                                      qSin(angle + 2.0 * M_PI / 3.0) * size * 0.7); // Lewy tył
            triangle << pos + QPointF(qCos(angle - 2.0 * M_PI / 3.0) * size * 0.7,
                                      qSin(angle - 2.0 * M_PI / 3.0) * size * 0.7); // Prawy tył

            painter.drawPolygon(triangle);

            // Oczy dla roślinożerców (szersze pole widzenia)
            painter.setBrush(Qt::white);
            float eyeSize = size * 0.2f;
            // Oczy bardziej z boku dla szerszego pola widzenia
            painter.drawEllipse(pos + QPointF(qCos(angle) * size * 0.5 + qCos(angle + M_PI/4) * size * 0.3,
                                              qSin(angle) * size * 0.5 + qSin(angle + M_PI/4) * size * 0.3),
                                eyeSize, eyeSize);
            painter.drawEllipse(pos + QPointF(qCos(angle) * size * 0.5 + qCos(angle - M_PI/4) * size * 0.3,
                                              qSin(angle) * size * 0.5 + qSin(angle - M_PI/4) * size * 0.3),
                                eyeSize, eyeSize);
        }
    }

    // Rysowanie drapieżników (trójkąty skierowane w kierunku ruchu)
    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            painter.setBrush(predator->color());
            painter.setPen(QPen(Qt::black, 2));

            QPointF pos = predator->position();
            float size = predator->size();

            // Oblicz kierunek (kąt) z wektora kierunku
            float angle = qAtan2(predator->direction().y(), predator->direction().x());

            // Węższy trójkąt dla drapieżników (lepsza aerodynamika)
            QPolygonF triangle;
            triangle << pos + QPointF(qCos(angle) * size * 1.2, qSin(angle) * size * 1.2); // Dłuższy przód
            triangle << pos + QPointF(qCos(angle + 2.2 * M_PI / 3.0) * size * 0.6,
                                      qSin(angle + 2.2 * M_PI / 3.0) * size * 0.6); // Węższy tył
            triangle << pos + QPointF(qCos(angle - 2.2 * M_PI / 3.0) * size * 0.6,
                                      qSin(angle - 2.2 * M_PI / 3.0) * size * 0.6);

            painter.drawPolygon(triangle);

            // Oczy dla drapieżników (przednie pole widzenia)
            painter.setBrush(Qt::yellow);
            float eyeSize = size * 0.25f;
            // Oczy bliżej przodu dla węższego pola widzenia
            painter.drawEllipse(pos + QPointF(qCos(angle) * size * 0.7 + qCos(angle + M_PI/6) * size * 0.2,
                                              qSin(angle) * size * 0.7 + qSin(angle + M_PI/6) * size * 0.2),
                                eyeSize, eyeSize);
            painter.drawEllipse(pos + QPointF(qCos(angle) * size * 0.7 + qCos(angle - M_PI/6) * size * 0.2,
                                              qSin(angle) * size * 0.7 + qSin(angle - M_PI/6) * size * 0.2),
                                eyeSize, eyeSize);
        }
    }

    // Reset transformacji dla statystyk
    painter.resetTransform();

    // Statystyki na ekranie
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));

    painter.drawText(10, 20, QString("Ofiary: %1").arg(m_prey.size()));
    painter.drawText(10, 40, QString("Drapieżniki: %1").arg(m_predators.size()));
    painter.drawText(10, 60, QString("Tick: %1").arg(m_generation));
    painter.drawText(10, 80, QString("Urodzenia: %1").arg(m_totalBirths));
    painter.drawText(10, 100, QString("Śmierci: %1").arg(m_totalDeaths));

    // Informacje o kamerze
    painter.drawText(width() - 200, 20, QString("Zoom: %1x").arg(m_zoom, 0, 'f', 1));
    painter.drawText(width() - 200, 40, QString("Kamera: %1, %2")
                                            .arg((int)m_cameraPosition.x()).arg((int)m_cameraPosition.y()));
}

void SimulationWidget::updateSimulation()
{
    m_generation++;

    // Aktualizuj środowisko
    updateEnvironment();

    // Przygotuj listy dla AI
    QVector<Organism*> allPrey;
    QVector<Organism*> allPredators;

    for (Prey* prey : m_prey) {
        allPrey.append(prey);
    }
    for (Predator* predator : m_predators) {
        allPredators.append(predator);
    }

    // Aktualizuj drapieżniki
    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            predator->setAvailablePrey(allPrey);
            predator->setAvailablePredators(allPredators);
            predator->setEnvironment(m_environment);
            predator->update();
        }
    }

    // Aktualizuj ofiary
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            prey->setAvailablePrey(allPrey);
            prey->setAvailablePredators(allPredators);
            prey->setEnvironment(m_environment);
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

void SimulationWidget::updateEnvironment()
{
    for (Environment* env : m_environment) {
        if (env->type() == Environment::BUSH) {
            env->regenerateFood(m_foodRegenerationRate * 0.1f);
        }
    }
}

void SimulationWidget::removeDeadOrganisms()
{
    int deaths = 0;

    auto preyIt = m_prey.begin();
    while (preyIt != m_prey.end()) {
        if ((*preyIt)->energy() <= 0 || (*preyIt)->hydration() <= 0 || (*preyIt)->age() > 10000) {   // ← 1200 zamiast 800
            delete *preyIt;
            preyIt = m_prey.erase(preyIt);
            deaths++;
        } else {
            ++preyIt;
        }
    }

    auto predatorIt = m_predators.begin();
    while (predatorIt != m_predators.end()) {
        if ((*predatorIt)->energy() <= 0 || (*predatorIt)->hydration() <= 0 || (*predatorIt)->age() > 7000) {   // ← 1000 zamiast 600
            delete *predatorIt;
            predatorIt = m_predators.erase(predatorIt);
            deaths++;
        } else {
            ++predatorIt;
        }
    }

    m_totalDeaths += deaths;
    m_deathHistory.append(deaths);
    if (m_deathHistory.size() > 1000) m_deathHistory.removeFirst();
}

void SimulationWidget::reproduceOrganisms()
{
    int births = 0;

    // Reprodukcja ofiar
    QVector<Prey*> newPrey;
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            Organism *offspring = prey->reproduce();
            if (offspring) {
                Prey *newPrey = qobject_cast<Prey*>(offspring);
                if (newPrey) {
                    this->m_prey.append(newPrey);
                    births++;
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
                    births++;
                }
            }
        }
    }

    m_totalBirths += births;
    m_birthHistory.append(births);
    if (m_birthHistory.size() > 1000) m_birthHistory.removeFirst();
}

void SimulationWidget::updateStatistics()
{
    int totalPrey = m_prey.size();
    int totalPredators = m_predators.size();

    m_preyHistory.append(totalPrey);
    m_predatorHistory.append(totalPredators);

    // Ogranicz historię
    if (m_preyHistory.size() > 1000) {
        m_preyHistory.removeFirst();
        m_predatorHistory.removeFirst();
    }

    // Oblicz średnie statystyki
    float avgPreySpeed = 0, avgPredatorSpeed = 0;
    float avgPreySize = 0, avgPredatorSize = 0;

    for (Prey* prey : m_prey) {
        avgPreySpeed += prey->speed();
        avgPreySize += prey->size();
    }
    for (Predator* predator : m_predators) {
        avgPredatorSpeed += predator->speed();
        avgPredatorSize += predator->size();
    }

    if (totalPrey > 0) {
        avgPreySpeed /= totalPrey;
        avgPreySize /= totalPrey;
    }
    if (totalPredators > 0) {
        avgPredatorSpeed /= totalPredators;
        avgPredatorSize /= totalPredators;
    }

    m_maxPopulation = qMax(m_maxPopulation, totalPrey + totalPredators);

    // Wyślij sygnały do panelu bocznego
    emit statisticsUpdated(totalPrey, totalPredators, m_generation,
                           avgPreySpeed, avgPredatorSpeed,
                           avgPreySize, avgPredatorSize,
                           m_totalBirths, m_totalDeaths);

    if (m_generation % 5 == 0) { // Co 5 generacji aktualizuj wykresy
        emit historyUpdated(m_preyHistory, m_predatorHistory);
    }
}

void SimulationWidget::wheelEvent(QWheelEvent *event)
{
    float zoomFactor = 1.1f;
    QPointF mouseWorldPos = screenToWorld(event->position().toPoint());

    if (event->angleDelta().y() > 0) {
        m_zoom *= zoomFactor;
        m_zoom = qMin(m_zoom, 5.0f);
    } else {
        m_zoom /= zoomFactor;
        m_zoom = qMax(m_zoom, 0.2f);
    }

    // Skoryguj pozycję kamery, aby powiększenie było względem kursora
    QPointF newMouseWorldPos = screenToWorld(event->position().toPoint());
    QPointF delta = newMouseWorldPos - mouseWorldPos;
    m_cameraPosition -= delta;

    update();
}

void SimulationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void SimulationWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_cameraPosition -= QPointF(delta.x() / m_zoom, delta.y() / m_zoom);
        m_lastMousePos = event->pos();
        update();
    }
}

void SimulationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}

void SimulationWidget::setSimulationParameters(float preyReproduction, float predatorReproduction,
                                               float foodRegeneration, float energyConsumption,
                                               float mutationRate, int initialPrey, int initialPredators)
{
    m_preyReproductionRate = preyReproduction;
    m_predatorReproductionRate = predatorReproduction;
    m_foodRegenerationRate = foodRegeneration;
    m_energyConsumptionRate = energyConsumption;
    m_mutationRate = mutationRate;

    // Można dodać ustawianie parametrów organizmów
    Q_UNUSED(initialPrey);
    Q_UNUSED(initialPredators);
}

void SimulationWidget::addBush()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    float size = 15 + rand->bounded(25);
    QPointF position(rand->bounded(3000), rand->bounded(2000));

    // Sprawdź czy nie nakłada się na wodę
    bool tooCloseToWater = false;
    for (Environment* env : m_environment) {
        if (env->type() == Environment::WATER) {
            float dx = env->position().x() - position.x();
            float dy = env->position().y() - position.y();
            float distance = qSqrt(dx * dx + dy * dy);
            if (distance < env->size() + size + 50) {
                tooCloseToWater = true;
                break;
            }
        }
    }

    if (!tooCloseToWater) {
        m_environment.append(new Environment(position, Environment::BUSH, size));
        update();
    }
}

void SimulationWidget::addWater()
{
    QRandomGenerator *rand = QRandomGenerator::global();
    float size = 40 + rand->bounded(80);
    QPointF position(rand->bounded(3000), rand->bounded(2000));

    m_environment.append(new Environment(position, Environment::WATER, size));
    update();
}

void SimulationWidget::startSimulation()
{
    if (!m_isRunning) {
        m_timer->start();
        m_isRunning = true;
        m_startButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
    }
}

void SimulationWidget::pauseSimulation()
{
    if (m_isRunning) {
        m_timer->stop();
        m_isRunning = false;
        m_startButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
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
    // Wywołaj implementację klasy bazowej
    QWidget::resizeEvent(event);

    // Opcjonalnie: dostosuj pozycje przycisków jeśli okno zmieni rozmiar
    if (m_startButton && m_pauseButton && m_resetButton) {
        int buttonWidth = 80;
        int buttonHeight = 30;
        int spacing = 10;
        int totalWidth = buttonWidth * 3 + spacing * 2;

        int startX = (width() - totalWidth) / 2;
        int startY = height() - buttonHeight - 10;

        m_startButton->setGeometry(startX, startY, buttonWidth, buttonHeight);
        m_pauseButton->setGeometry(startX + buttonWidth + spacing, startY, buttonWidth, buttonHeight);
        m_resetButton->setGeometry(startX + (buttonWidth + spacing) * 2, startY, buttonWidth, buttonHeight);
    }
}
