#include "sidepanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>

SidePanel::SidePanel(QWidget *parent)
    : QWidget(parent)
    , m_currentGeneration(0)
{
    setFixedWidth(300);

    m_tabWidget = new QTabWidget(this);

    setupParametersTab();
    setupChartsTab();
    setupControlTab();

    m_tabWidget->addTab(m_parametersTab, "Parametry");
    m_tabWidget->addTab(m_chartsTab, "Wykresy");
    m_tabWidget->addTab(m_controlTab, "Kontrola");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    setLayout(mainLayout);
}

void SidePanel::setupParametersTab()
{
    m_parametersTab = new QWidget();
    QFormLayout *formLayout = new QFormLayout();

    // Parametry reprodukcji
    m_preyReproductionSpin = new QDoubleSpinBox();
    m_preyReproductionSpin->setRange(0.1, 5.0);
    m_preyReproductionSpin->setValue(1.0);
    m_preyReproductionSpin->setSingleStep(0.1);
    formLayout->addRow("Tempo reprodukcji ofiar:", m_preyReproductionSpin);

    m_predatorReproductionSpin = new QDoubleSpinBox();
    m_predatorReproductionSpin->setRange(0.1, 5.0);
    m_predatorReproductionSpin->setValue(1.0);
    m_predatorReproductionSpin->setSingleStep(0.1);
    formLayout->addRow("Tempo reprodukcji drapieżników:", m_predatorReproductionSpin);

    // Inne parametry
    m_foodRegenerationSpin = new QDoubleSpinBox();
    m_foodRegenerationSpin->setRange(0.1, 3.0);
    m_foodRegenerationSpin->setValue(0.5);
    m_foodRegenerationSpin->setSingleStep(0.1);
    formLayout->addRow("Tempo regeneracji jedzenia:", m_foodRegenerationSpin);

    m_energyConsumptionSpin = new QDoubleSpinBox();
    m_energyConsumptionSpin->setRange(0.1, 3.0);
    m_energyConsumptionSpin->setValue(1.0);
    m_energyConsumptionSpin->setSingleStep(0.1);
    formLayout->addRow("Tempo zużycia energii:", m_energyConsumptionSpin);

    m_mutationRateSpin = new QDoubleSpinBox();
    m_mutationRateSpin->setRange(0.0, 20.0);
    m_mutationRateSpin->setValue(5.0);
    m_mutationRateSpin->setSingleStep(0.5);
    formLayout->addRow("Stopień mutacji (%):", m_mutationRateSpin);

    // Początkowe populacje
    m_initialPreySpin = new QSpinBox();
    m_initialPreySpin->setRange(1, 200);
    m_initialPreySpin->setValue(20);
    formLayout->addRow("Początkowa liczba ofiar:", m_initialPreySpin);

    m_initialPredatorsSpin = new QSpinBox();
    m_initialPredatorsSpin->setRange(1, 50);
    m_initialPredatorsSpin->setValue(5);
    formLayout->addRow("Początkowa liczba drapieżników:", m_initialPredatorsSpin);

    // Przyciski
    QPushButton *applyButton = new QPushButton("Zastosuj");
    QPushButton *resetButton = new QPushButton("Przywróć domyślne");

    connect(applyButton, &QPushButton::clicked, this, &SidePanel::applyParameters);
    connect(resetButton, &QPushButton::clicked, this, &SidePanel::resetToDefaults);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(resetButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_parametersTab);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
}

void SidePanel::setupChartsTab()
{
    m_chartsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    // Wykres
    m_chart = new QChart();
    m_chart->setTitle("Dynamika populacji");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    m_preySeries = new QLineSeries();
    m_preySeries->setName("Ofiary");
    m_preySeries->setColor(QColor(0, 255, 0));

    m_predatorSeries = new QLineSeries();
    m_predatorSeries->setName("Drapieżniki");
    m_predatorSeries->setColor(QColor(255, 0, 0));

    m_chart->addSeries(m_preySeries);
    m_chart->addSeries(m_predatorSeries);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Czas (kroki)");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(10);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Liczebność");
    axisY->setLabelFormat("%d");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);

    m_preySeries->attachAxis(axisX);
    m_preySeries->attachAxis(axisY);
    m_predatorSeries->attachAxis(axisX);
    m_predatorSeries->attachAxis(axisY);

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(m_chartView);

    // Statystyki
    QGroupBox *statsGroup = new QGroupBox("Bieżące statystyki");
    QFormLayout *statsLayout = new QFormLayout();

    m_preyCountLabel = new QLabel("0");
    m_predatorCountLabel = new QLabel("0");
    m_generationLabel = new QLabel("0");
    m_avgPreySpeedLabel = new QLabel("0.0");
    m_avgPredatorSpeedLabel = new QLabel("0.0");

    statsLayout->addRow("Ofiary:", m_preyCountLabel);
    statsLayout->addRow("Drapieżniki:", m_predatorCountLabel);
    statsLayout->addRow("Pokolenie:", m_generationLabel);
    statsLayout->addRow("Śr. szybkość ofiar:", m_avgPreySpeedLabel);
    statsLayout->addRow("Śr. szybkość drapieżników:", m_avgPredatorSpeedLabel);

    statsGroup->setLayout(statsLayout);
    layout->addWidget(statsGroup);

    m_chartsTab->setLayout(layout);
}

void SidePanel::setupControlTab()
{
    m_controlTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    QPushButton *addPreyButton = new QPushButton("Dodaj ofiary");
    QPushButton *addPredatorButton = new QPushButton("Dodaj drapieżniki");
    QPushButton *clearAllButton = new QPushButton("Wyczyść wszystko");

    connect(addPreyButton, &QPushButton::clicked, [this]() {
        emit addPreyRequested(10);
    });

    connect(addPredatorButton, &QPushButton::clicked, [this]() {
        emit addPredatorRequested(3);
    });

    connect(clearAllButton, &QPushButton::clicked, this, &SidePanel::clearAllRequested);

    layout->addWidget(addPreyButton);
    layout->addWidget(addPredatorButton);
    layout->addWidget(clearAllButton);
    layout->addStretch();

    m_controlTab->setLayout(layout);
}

void SidePanel::applyParameters()
{
    emit simulationParametersChanged(
        m_preyReproductionSpin->value(),
        m_predatorReproductionSpin->value(),
        m_foodRegenerationSpin->value(),
        m_energyConsumptionSpin->value(),
        m_mutationRateSpin->value(),
        m_initialPreySpin->value(),
        m_initialPredatorsSpin->value()
        );
}

void SidePanel::resetToDefaults()
{
    m_preyReproductionSpin->setValue(1.0);
    m_predatorReproductionSpin->setValue(1.0);
    m_foodRegenerationSpin->setValue(0.5);
    m_energyConsumptionSpin->setValue(1.0);
    m_mutationRateSpin->setValue(5.0);
    m_initialPreySpin->setValue(20);
    m_initialPredatorsSpin->setValue(5);

    applyParameters();
}

void SidePanel::updateStatistics(int preyCount, int predatorCount, int generation)
{
    m_preyCountLabel->setText(QString::number(preyCount));
    m_predatorCountLabel->setText(QString::number(predatorCount));
    m_generationLabel->setText(QString::number(generation));
    m_currentGeneration = generation;
}

void SidePanel::updateCharts(const QVector<int> &preyHistory, const QVector<int> &predatorHistory)
{
    m_preySeries->clear();
    m_predatorSeries->clear();

    for (int i = 0; i < preyHistory.size(); ++i) {
        m_preySeries->append(i, preyHistory[i]);
        if (i < predatorHistory.size()) {
            m_predatorSeries->append(i, predatorHistory[i]);
        }
    }

    // Dostosuj oś Y
    int maxPopulation = 0;
    for (int count : preyHistory) {
        maxPopulation = qMax(maxPopulation, count);
    }
    for (int count : predatorHistory) {
        maxPopulation = qMax(maxPopulation, count);
    }

    QValueAxis *axisY = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());
    if (axisY) {
        axisY->setRange(0, qMax(10, maxPopulation + 10));
    }

    QValueAxis *axisX = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
    if (axisX) {
        axisX->setRange(0, qMax(1, preyHistory.size()));
    }
}
