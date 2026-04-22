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
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>

SidePanel::SidePanel(QWidget *parent)
    : QWidget(parent)
    , m_currentGeneration(0)
{
    setFixedWidth(350);

    m_tabWidget = new QTabWidget(this);

    setupParametersTab();
    setupChartsTab();
    setupStatisticsTab();
    setupControlTab();

    m_tabWidget->addTab(m_parametersTab, "Parametry");
    m_tabWidget->addTab(m_chartsTab, "Wykresy");
    m_tabWidget->addTab(m_statisticsTab, "Statystyki");
    m_tabWidget->addTab(m_controlTab, "Kontrola");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    setLayout(mainLayout);
}

void SidePanel::setupParametersTab()
{
    m_parametersTab = new QWidget();
    QFormLayout *formLayout = new QFormLayout();

    // Regeneracja jedzenia (mnożnik)
    m_foodRegenerationCombo = new QComboBox();
    m_foodRegenerationCombo->addItems({"0.5x", "0.75x", "1.0x", "1.5x", "2.0x", "5.0x"});
    m_foodRegenerationCombo->setCurrentIndex(2); // 1.0x
    m_foodRegenerationCombo->setToolTip("Mnożnik szybkości regeneracji jedzenia w krzakach");
    formLayout->addRow("Regeneracja jedzenia:", m_foodRegenerationCombo);

    // Limit jedzenia w krzakach
    m_bushFoodLimitSpin = new QSpinBox();
    m_bushFoodLimitSpin->setRange(50, 500);
    m_bushFoodLimitSpin->setValue(140);
    m_bushFoodLimitSpin->setSingleStep(10);
    m_bushFoodLimitSpin->setSuffix(" jednostek");
    m_bushFoodLimitSpin->setToolTip("Maksymalna ilość jedzenia w krzaku");
    formLayout->addRow("Limit jedzenia w krzaku:", m_bushFoodLimitSpin);

    // Zużycie energii drapieżników
    m_predatorEnergyCombo = new QComboBox();
    m_predatorEnergyCombo->addItems({"0.5x", "0.75x", "1.0x", "1.5x", "2.0x"});
    m_predatorEnergyCombo->setCurrentIndex(2); // 1.0x
    m_predatorEnergyCombo->setToolTip("Mnożnik zużycia energii przez drapieżniki");
    formLayout->addRow("Zużycie energii drapieżników:", m_predatorEnergyCombo);

    // Zużycie energii ofiar
    m_preyEnergyCombo = new QComboBox();
    m_preyEnergyCombo->addItems({"0.5x", "0.75x", "1.0x", "1.5x", "2.0x"});
    m_preyEnergyCombo->setCurrentIndex(2); // 1.0x
    m_preyEnergyCombo->setToolTip("Mnożnik zużycia energii przez ofiary");
    formLayout->addRow("Zużycie energii ofiar:", m_preyEnergyCombo);

    // Początkowa liczba ofiar
    m_initialPreySpin = new QSpinBox();
    m_initialPreySpin->setRange(1, 500);
    m_initialPreySpin->setValue(120);
    m_initialPreySpin->setToolTip("Początkowa liczba ofiar przy restarcie symulacji");
    formLayout->addRow("Początkowa liczba ofiar:", m_initialPreySpin);

    // Początkowa liczba drapieżników
    m_initialPredatorsSpin = new QSpinBox();
    m_initialPredatorsSpin->setRange(1, 100);
    m_initialPredatorsSpin->setValue(15);
    m_initialPredatorsSpin->setToolTip("Początkowa liczba drapieżników przy restarcie symulacji");
    formLayout->addRow("Początkowa liczba drapieżników:", m_initialPredatorsSpin);

    // Zasięg wzroku drapieżników
    m_predatorVisionCombo = new QComboBox();
    m_predatorVisionCombo->addItems({"0.75x", "1.0x", "1.5x", "2.0x", "3.0x", "5.0x"});
    m_predatorVisionCombo->setCurrentIndex(1); // 1.0x
    m_predatorVisionCombo->setToolTip("Mnożnik zasięgu wzroku drapieżników");
    formLayout->addRow("Zasięg wzroku drapieżników:", m_predatorVisionCombo);

    // Zasięg wzroku ofiar
    m_preyVisionCombo = new QComboBox();
    m_preyVisionCombo->addItems({"0.75x", "1.0x", "1.5x", "2.0x", "3.0x", "5.0x"});
    m_preyVisionCombo->setCurrentIndex(1); // 1.0x
    m_preyVisionCombo->setToolTip("Mnożnik zasięgu wzroku ofiar");
    formLayout->addRow("Zasięg wzroku ofiar:", m_preyVisionCombo);

    // Przyciski
    QPushButton *applyButton = new QPushButton("Restart symulacji z powyższymi parametrami");
    QPushButton *resetButton = new QPushButton("Przywróć domyślne");

    connect(applyButton, &QPushButton::clicked, this, &SidePanel::applyParameters);
    connect(resetButton, &QPushButton::clicked, this, &SidePanel::resetToDefaults);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_parametersTab);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(applyButton);
    mainLayout->addWidget(resetButton);
    mainLayout->addStretch();
}

void SidePanel::setupChartsTab()
{
    m_chartsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    // Wykres populacji
    QGroupBox *populationGroup = new QGroupBox("Dynamika populacji");
    QVBoxLayout *populationLayout = new QVBoxLayout();

    m_chart = new QChart();
    m_chart->setTitle("Populacja w czasie");
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
    m_chartView->setMinimumHeight(200);

    populationLayout->addWidget(m_chartView);
    populationGroup->setLayout(populationLayout);

    layout->addWidget(populationGroup);
    layout->addStretch();

    m_chartsTab->setLayout(layout);
}

void SidePanel::setupStatisticsTab()
{
    m_statisticsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    // Bieżące statystyki
    QGroupBox *currentStatsGroup = new QGroupBox("Bieżące statystyki");
    QFormLayout *currentStatsLayout = new QFormLayout();

    m_preyCountLabel = new QLabel("0");
    m_predatorCountLabel = new QLabel("0");
    m_generationLabel = new QLabel("0");
    m_avgPreySpeedLabel = new QLabel("0.0");
    m_avgPredatorSpeedLabel = new QLabel("0.0");
    m_avgPreySizeLabel = new QLabel("0.0");
    m_avgPredatorSizeLabel = new QLabel("0.0");
    m_totalBirthsLabel = new QLabel("0");
    m_totalDeathsLabel = new QLabel("0");
    m_preyToPredatorRatioLabel = new QLabel("0.0");

    currentStatsLayout->addRow("Ofiary:", m_preyCountLabel);
    currentStatsLayout->addRow("Drapieżniki:", m_predatorCountLabel);
    currentStatsLayout->addRow("Pokolenie:", m_generationLabel);
    currentStatsLayout->addRow("Stosunek O:D:", m_preyToPredatorRatioLabel);
    currentStatsLayout->addRow("Śr. szybkość ofiar:", m_avgPreySpeedLabel);
    currentStatsLayout->addRow("Śr. szybkość drapieżników:", m_avgPredatorSpeedLabel);
    currentStatsLayout->addRow("Śr. rozmiar ofiar:", m_avgPreySizeLabel);
    currentStatsLayout->addRow("Śr. rozmiar drapieżników:", m_avgPredatorSizeLabel);
    currentStatsLayout->addRow("Łączne urodzenia:", m_totalBirthsLabel);
    currentStatsLayout->addRow("Łączne zgony:", m_totalDeathsLabel);

    currentStatsGroup->setLayout(currentStatsLayout);
    layout->addWidget(currentStatsGroup);
    layout->addStretch();

    m_statisticsTab->setLayout(layout);
}

void SidePanel::setupControlTab()
{
    m_controlTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    QPushButton *addPreyButton = new QPushButton("Dodaj ofiary (10)");
    QPushButton *addPredatorButton = new QPushButton("Dodaj drapieżniki (3)");
    QPushButton *addBushButton = new QPushButton("Dodaj krzak");
    QPushButton *addWaterButton = new QPushButton("Dodaj wodę");
    QPushButton *clearAllButton = new QPushButton("Wyczyść wszystko");

    addBushButton->setToolTip("Dodaje nowy krzak w losowym miejscu na mapie.");
    addWaterButton->setToolTip("Dodaje nowy zbiornik wodny w losowym miejscu.");

    connect(addPreyButton, &QPushButton::clicked, [this]() {
        emit addPreyRequested(10);
    });

    connect(addPredatorButton, &QPushButton::clicked, [this]() {
        emit addPredatorRequested(3);
    });

    connect(addBushButton, &QPushButton::clicked, this, &SidePanel::addBushRequested);
    connect(addWaterButton, &QPushButton::clicked, this, &SidePanel::addWaterRequested);
    connect(clearAllButton, &QPushButton::clicked, this, &SidePanel::clearAllRequested);

    layout->addWidget(addPreyButton);
    layout->addWidget(addPredatorButton);
    layout->addWidget(addBushButton);
    layout->addWidget(addWaterButton);
    layout->addWidget(clearAllButton);
    layout->addStretch();

    m_controlTab->setLayout(layout);
}

void SidePanel::applyParameters()
{
    auto parseMultiplier = [](const QString& text) -> float {
        QString temp = text;
        return temp.remove("x").toFloat();
    };

    float foodRegenMult = parseMultiplier(m_foodRegenerationCombo->currentText());
    int bushFoodLimit = m_bushFoodLimitSpin->value();
    float predatorEnergyMult = parseMultiplier(m_predatorEnergyCombo->currentText());
    float preyEnergyMult = parseMultiplier(m_preyEnergyCombo->currentText());
    int initialPrey = m_initialPreySpin->value();
    int initialPredators = m_initialPredatorsSpin->value();
    float predatorVisionMult = parseMultiplier(m_predatorVisionCombo->currentText());
    float preyVisionMult = parseMultiplier(m_preyVisionCombo->currentText());

    emit simulationParametersChanged(
        foodRegenMult,
        bushFoodLimit,
        predatorEnergyMult,
        preyEnergyMult,
        initialPrey,
        initialPredators,
        predatorVisionMult,
        preyVisionMult
        );

    emit restartSimulationRequested();
}

void SidePanel::resetToDefaults()
{
    m_foodRegenerationCombo->setCurrentIndex(2); // 1.0x
    m_bushFoodLimitSpin->setValue(140);
    m_predatorEnergyCombo->setCurrentIndex(2); // 1.0x
    m_preyEnergyCombo->setCurrentIndex(2); // 1.0x
    m_initialPreySpin->setValue(120);
    m_initialPredatorsSpin->setValue(15);
    m_predatorVisionCombo->setCurrentIndex(1); // 1.0x
    m_preyVisionCombo->setCurrentIndex(1); // 1.0x

    applyParameters();
}

void SidePanel::updateStatistics(int preyCount, int predatorCount, int generation,
                                 float avgPreySpeed, float avgPredatorSpeed,
                                 float avgPreySize, float avgPredatorSize,
                                 int births, int deaths)
{
    m_preyCountLabel->setText(QString::number(preyCount));
    m_predatorCountLabel->setText(QString::number(predatorCount));
    m_generationLabel->setText(QString::number(generation));
    m_avgPreySpeedLabel->setText(QString::number(avgPreySpeed, 'f', 2));
    m_avgPredatorSpeedLabel->setText(QString::number(avgPredatorSpeed, 'f', 2));
    m_avgPreySizeLabel->setText(QString::number(avgPreySize, 'f', 2));
    m_avgPredatorSizeLabel->setText(QString::number(avgPredatorSize, 'f', 2));
    m_totalBirthsLabel->setText(QString::number(births));
    m_totalDeathsLabel->setText(QString::number(deaths));

    float ratio = predatorCount > 0 ? (float)preyCount / predatorCount : preyCount;
    m_preyToPredatorRatioLabel->setText(QString::number(ratio, 'f', 2));

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

    QList<QAbstractAxis*> axesList = m_chart->axes(Qt::Vertical);
    if (!axesList.isEmpty()) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(axesList.first());
        if (axisY) {
            int maxPopulation = 0;
            for (int count : preyHistory) maxPopulation = qMax(maxPopulation, count);
            for (int count : predatorHistory) maxPopulation = qMax(maxPopulation, count);
            axisY->setRange(0, qMax(10, maxPopulation + 10));
        }
    }

    axesList = m_chart->axes(Qt::Horizontal);
    if (!axesList.isEmpty()) {
        QValueAxis *axisX = qobject_cast<QValueAxis*>(axesList.first());
        if (axisX) {
            axisX->setRange(0, qMax(1, preyHistory.size()));
        }
    }
}
