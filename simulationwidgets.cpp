#include "simulationwidgets.h"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QtMath>

SimulationWidget::SimulationWidget(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(nullptr)
    , m_isRunning(false)
    , m_maxPopulation(0)
    , m_currentGeneration(0)
    , m_foodRegenerationRate(0.5f)
    , m_preyReproductionRate(1.0f)
    , m_predatorReproductionRate(1.0f)
    , m_energyConsumptionRate(1.0f)
    , m_waterConsumptionRate(1.0f)
    , m_mutationRate(5.0f)
    , m_addMode(NONE)
    , m_worldWidth(2000)
    , m_worldHeight(2000)
    , m_zoomLevel(1.0f)
{
    // Utwórz scenę
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_worldWidth, m_worldHeight);
    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::updateSimulation);

    initializeSimulation();
    setupEnvironment();
}

SimulationWidget::~SimulationWidget()
{
    pauseSimulation();

    qDeleteAll(m_prey);
    qDeleteAll(m_predators);
    qDeleteAll(m_waterSources);
    qDeleteAll(m_bushes);
}

void SimulationWidget::initializeSimulation()
{
    // Czyść wszystko
    qDeleteAll(m_prey);
    qDeleteAll(m_predators);
    m_prey.clear();
    m_predators.clear();

    // Czyść środowisko
    qDeleteAll(m_waterSources);
    qDeleteAll(m_bushes);
    m_waterSources.clear();
    m_bushes.clear();

    // Czyść grafikę
    for (auto item : m_waterGraphics) delete item;
    for (auto item : m_bushGraphics) delete item;
    m_waterGraphics.clear();
    m_bushGraphics.clear();

    m_scene->clear();

    // Resetuj historię
    m_preyHistory.clear();
    m_predatorHistory.clear();

    // Początkowa populacja
    addPrey(20);
    addPredator(5);

    // Ponownie utwórz środowisko
    setupEnvironment();

    m_maxPopulation = m_prey.size() + m_predators.size();
    m_currentGeneration = 0;
    updateStatistics();
    updateSceneGraphics();
}

void SimulationWidget::setupEnvironment()
{
    QRandomGenerator *rand = QRandomGenerator::global();

    // Utwórz źródła wody
    for (int i = 0; i < 5; ++i) {
        QPointF pos(rand->bounded(m_worldWidth), rand->bounded(m_worldHeight));
        WaterSource* water = new WaterSource(pos, 50.0f);
        m_waterSources.append(water);

        // Dodaj grafikę
        QGraphicsEllipseItem* item = new QGraphicsEllipseItem(
            pos.x() - water->radius(),
            pos.y() - water->radius(),
            water->radius() * 2,
            water->radius() * 2
            );
        item->setBrush(QBrush(QColor(100, 200, 255, 100)));
        item->setPen(QPen(Qt::blue, 2));
        m_scene->addItem(item);
        m_waterGraphics.append(item);
    }

    // Utwórz krzaki
    for (int i = 0; i < 10; ++i) {
        QPointF pos(rand->bounded(m_worldWidth), rand->bounded(m_worldHeight));
        Bush* bush = new Bush(pos, 40.0f);
        m_bushes.append(bush);

        // Dodaj grafikę
        QGraphicsEllipseItem* item = new QGraphicsEllipseItem(
            pos.x() - bush->radius(),
            pos.y() - bush->radius(),
            bush->radius() * 2,
            bush->radius() * 2
            );
        item->setBrush(QBrush(QColor(34, 139, 34, 150)));
        item->setPen(QPen(QColor(0, 100, 0), 2));
        m_scene->addItem(item);
        m_bushGraphics.append(item);
    }
}

void SimulationWidget::updateSimulation()
{
    // Aktualizuj środowisko
    for (WaterSource* water : m_waterSources) {
        water->update();
    }
    for (Bush* bush : m_bushes) {
        bush->update();
    }

    // Ustaw środowisko dla organizmów
    for (Prey *prey : m_prey) {
        if (prey->energy() > 0) {
            prey->setAvailableWater(m_waterSources);
            prey->setAvailableBushes(m_bushes);
            prey->update();
        }
    }

    for (Predator *predator : m_predators) {
        if (predator->energy() > 0) {
            predator->setAvailablePrey(m_prey);
            predator->setAvailableWater(m_waterSources);
            predator->update();
        }
    }

    // Usuń martwe organizmy
    removeDeadOrganisms();

    // Reprodukcja
    reproduceOrganisms();

    // Aktualizuj statystyki
    updateStatistics();

    // Aktualizuj pokolenie
    m_currentGeneration++;

    // Aktualizuj grafikę
    updateSceneGraphics();
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
    // Tymczasowe listy dla nowych organizmów
    QVector<Prey*> newPrey;
    QVector<Predator*> newPredators;

    // Reprodukcja ofiar
    for (Prey *prey : m_prey) {
        if (prey->canReproduce() && prey->energy() > 120.0f) {
            Organism *offspring = prey->reproduce();
            if (offspring) {
                Prey *newPreyObj = qobject_cast<Prey*>(offspring);
                if (newPreyObj) {
                    newPrey.append(newPreyObj);
                }
            }
        }
    }

    // Reprodukcja drapieżników
    for (Predator *predator : m_predators) {
        if (predator->canReproduce() && predator->energy() > 150.0f) {
            Organism *offspring = predator->reproduce();
            if (offspring) {
                Predator *newPredatorObj = qobject_cast<Predator*>(offspring);
                if (newPredatorObj) {
                    newPredators.append(newPredatorObj);
                }
            }
        }
    }

    // Dodaj nowe organizmy do głównych list
    m_prey.append(newPrey);
    m_predators.append(newPredators);
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

void SimulationWidget::addPrey(int count)
{
    QRandomGenerator *rand = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        float speed = 1.0f + (rand->bounded(100) / 100.0f);
        float size = 4.0f + (rand->bounded(60) / 10.0f);
        float vision = 30.0f + rand->bounded(70);

        Prey *prey = new Prey(
            QPointF(rand->bounded(m_worldWidth), rand->bounded(m_worldHeight)),
            speed,
            size,
            vision
            );
        prey->setAvailableWater(m_waterSources);
        prey->setAvailableBushes(m_bushes);
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
            QPointF(rand->bounded(m_worldWidth), rand->bounded(m_worldHeight)),
            speed,
            size,
            vision
            );
        predator->setAvailableWater(m_waterSources);
        predator->setAvailablePrey(m_prey);
        m_predators.append(predator);
    }
}

void SimulationWidget::updateSceneGraphics()
{
    // Usuń stare grafiki organizmów
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        if (item->data(0).toString() == "organism") {
            m_scene->removeItem(item);
            delete item;
        }
    }

    // Rysuj ofiary
    for (Prey* prey : m_prey) {
        if (prey->energy() > 0) {
            QGraphicsEllipseItem* item = new QGraphicsEllipseItem(
                prey->position().x() - prey->size(),
                prey->position().y() - prey->size(),
                prey->size() * 2,
                prey->size() * 2
                );
            item->setBrush(prey->color());
            item->setPen(QPen(Qt::black));
            item->setData(0, "organism");
            item->setZValue(1);
            m_scene->addItem(item);
        }
    }

    // Rysuj drapieżniki
    for (Predator* predator : m_predators) {
        if (predator->energy() > 0) {
            QGraphicsEllipseItem* item = new QGraphicsEllipseItem(
                predator->position().x() - predator->size(),
                predator->position().y() - predator->size(),
                predator->size() * 2,
                predator->size() * 2
                );
            item->setBrush(predator->color());
            item->setPen(QPen(Qt::black));
            item->setData(0, "organism");
            item->setZValue(2);
            m_scene->addItem(item);
        }
    }
}

void SimulationWidget::addOrganismAtPosition(Organism::OrganismType type, QPointF position)
{
    QRandomGenerator *rand = QRandomGenerator::global();

    if (type == Organism::PREY) {
        float speed = 1.0f + (rand->bounded(100) / 100.0f);
        float size = 4.0f + (rand->bounded(60) / 10.0f);
        float vision = 30.0f + rand->bounded(70);

        Prey* prey = new Prey(position, speed, size, vision);
        prey->setAvailableWater(m_waterSources);
        prey->setAvailableBushes(m_bushes);
        m_prey.append(prey);

    } else if (type == Organism::PREDATOR) {
        float speed = 1.2f + (rand->bounded(100) / 100.0f);
        float size = 6.0f + (rand->bounded(80) / 10.0f);
        float vision = 60.0f + rand->bounded(90);

        Predator* predator = new Predator(position, speed, size, vision);
        predator->setAvailableWater(m_waterSources);
        predator->setAvailablePrey(m_prey);
        m_predators.append(predator);
    }

    updateSceneGraphics();
}

void SimulationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());

        if (m_addMode == ADD_PREY) {
            addOrganismAtPosition(Organism::PREY, scenePos);
            m_addMode = NONE;
        } else if (m_addMode == ADD_PREDATOR) {
            addOrganismAtPosition(Organism::PREDATOR, scenePos);
            m_addMode = NONE;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void SimulationWidget::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
        m_zoomLevel *= scaleFactor;
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        m_zoomLevel /= scaleFactor;
    }
}

void SimulationWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    // Rysuj tło
    painter->fillRect(rect, QColor(240, 240, 220));

    // Rysuj siatkę
    painter->setPen(QPen(QColor(200, 200, 200), 1));
    qreal left = int(rect.left()) - (int(rect.left()) % 100);
    qreal top = int(rect.top()) - (int(rect.top()) % 100);

    for (qreal x = left; x < rect.right(); x += 100) {
        painter->drawLine(x, rect.top(), x, rect.bottom());
    }
    for (qreal y = top; y < rect.bottom(); y += 100) {
        painter->drawLine(rect.left(), y, rect.right(), y);
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
}

void SimulationWidget::zoomIn()
{
    scale(1.2, 1.2);
    m_zoomLevel *= 1.2;
}

void SimulationWidget::zoomOut()
{
    scale(1/1.2, 1/1.2);
    m_zoomLevel /= 1.2;
}

void SimulationWidget::resetZoom()
{
    resetTransform();
    m_zoomLevel = 1.0f;
}
