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

    // Środowisko
    m_foodRegenerationSpin = new QDoubleSpinBox();
    m_foodRegenerationSpin->setRange(0.0, 2.0);
    m_foodRegenerationSpin->setValue(0.5);
    m_foodRegenerationSpin->setSingleStep(0.1);
    formLayout->addRow("Regeneracja jedzenia:", m_foodRegenerationSpin);

    m_waterCoverageSpin = new QDoubleSpinBox();
    m_waterCoverageSpin->setRange(0.0, 50.0);
    m_waterCoverageSpin->setValue(15.0);
    m_waterCoverageSpin->setSingleStep(1.0);
    formLayout->addRow("Pokrycie wodą (%):", m_waterCoverageSpin);

    m_bushDensitySpin = new QDoubleSpinBox();
    m_bushDensitySpin->setRange(0.1, 10.0);
    m_bushDensitySpin->setValue(2.0);
    m_bushDensitySpin->setSingleStep(0.5);
    formLayout->addRow("Gęstość krzaków:", m_bushDensitySpin);

    // Organizmy
    m_energyConsumptionSpin = new QDoubleSpinBox();
    m_energyConsumptionSpin->setRange(0.1, 3.0);
    m_energyConsumptionSpin->setValue(1.0);
    m_energyConsumptionSpin->setSingleStep(0.1);
    formLayout->addRow("Zużycie energii:", m_energyConsumptionSpin);

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

    // Wykres ewolucji
    QGroupBox *evolutionGroup = new QGroupBox("Ewolucja cech");
    QVBoxLayout *evolutionLayout = new QVBoxLayout();

    m_evolutionChart = new QChart();
    m_evolutionChart->setTitle("Średnie cechy populacji");

    m_speedSeries = new QLineSeries();
    m_speedSeries->setName("Szybkość");
    m_speedSeries->setColor(QColor(255, 165, 0));

    m_sizeSeries = new QLineSeries();
    m_sizeSeries->setName("Rozmiar");
    m_sizeSeries->setColor(QColor(138, 43, 226));

    m_visionSeries = new QLineSeries();
    m_visionSeries->setName("Wzrok");
    m_visionSeries->setColor(QColor(0, 191, 255));

    m_evolutionChart->addSeries(m_speedSeries);
    m_evolutionChart->addSeries(m_sizeSeries);
    m_evolutionChart->addSeries(m_visionSeries);

    QValueAxis *evoAxisX = new QValueAxis();
    evoAxisX->setTitleText("Pokolenie");
    evoAxisX->setLabelFormat("%d");

    QValueAxis *evoAxisY = new QValueAxis();
    evoAxisY->setTitleText("Wartość cechy");
    evoAxisY->setLabelFormat("%.2f");

    m_evolutionChart->addAxis(evoAxisX, Qt::AlignBottom);
    m_evolutionChart->addAxis(evoAxisY, Qt::AlignLeft);

    m_speedSeries->attachAxis(evoAxisX);
    m_speedSeries->attachAxis(evoAxisY);
    m_sizeSeries->attachAxis(evoAxisX);
    m_sizeSeries->attachAxis(evoAxisY);
    m_visionSeries->attachAxis(evoAxisX);
    m_visionSeries->attachAxis(evoAxisY);

    m_evolutionChartView = new QChartView(m_evolutionChart);
    m_evolutionChartView->setRenderHint(QPainter::Antialiasing);
    m_evolutionChartView->setMinimumHeight(200);

    evolutionLayout->addWidget(m_evolutionChartView);
    evolutionGroup->setLayout(evolutionLayout);

    layout->addWidget(evolutionGroup);
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

    // Tabela historii ewolucji
    QGroupBox *evolutionTableGroup = new QGroupBox("Historia ewolucji");
    QVBoxLayout *tableLayout = new QVBoxLayout();

    m_evolutionTable = new QTableWidget();
    m_evolutionTable->setColumnCount(4);
    m_evolutionTable->setHorizontalHeaderLabels(QStringList() << "Pokolenie" << "Szybkość" << "Rozmiar" << "Wzrok");
    m_evolutionTable->horizontalHeader()->setStretchLastSection(true);
    m_evolutionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_evolutionTable->setMaximumHeight(150);

    tableLayout->addWidget(m_evolutionTable);
    evolutionTableGroup->setLayout(tableLayout);
    layout->addWidget(evolutionTableGroup);

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

    // Dodaj tooltipy dla wyjaśnienia
    addBushButton->setToolTip("Dodaje nowy krzak w losowym miejscu na mapie.\nKrzaki są źródłem pożywienia dla roślinożerców.");
    addWaterButton->setToolTip("Dodaje nowy zbiornik wodny w losowym miejscu.\nZwierzęta muszą pić wodę, aby przetrwać.\nNiektóre zwierzęta mogą pływać, inne toną.");

    connect(addPreyButton, &QPushButton::clicked, [this]() {
        emit addPreyRequested(10);
    });

    connect(addPredatorButton, &QPushButton::clicked, [this]() {
        emit addPredatorRequested(3);
    });

    connect(addBushButton, &QPushButton::clicked, [this]() {
        // Emituj sygnał do dodania krzaka
        // (musisz dodać odpowiedni sygnał w sidepanel.h i połączyć go w mainwindow.cpp)
    });

    connect(addWaterButton, &QPushButton::clicked, [this]() {
        // Emituj sygnał do dodania wody
    });

    connect(clearAllButton, &QPushButton::clicked, this, &SidePanel::clearAllRequested);

    layout->addWidget(addPreyButton);
    layout->addWidget(addPredatorButton);
    layout->addWidget(addBushButton);
    layout->addWidget(addWaterButton);
    layout->addWidget(clearAllButton);

    // Dodaj wyjaśnienie
    QLabel *infoLabel = new QLabel("Przyciski do manipulacji środowiskiem:");
    infoLabel->setWordWrap(true);
    QLabel *bushInfo = new QLabel("• Krzak: źródło pożywienia dla roślinożerców");
    QLabel *waterInfo = new QLabel("• Woda: konieczna do picia, niektóre zwierzęta toną");

    layout->addWidget(infoLabel);
    layout->addWidget(bushInfo);
    layout->addWidget(waterInfo);
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
    m_waterCoverageSpin->setValue(15.0);
    m_bushDensitySpin->setValue(2.0);

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

    // Aktualizuj historię ewolucji co 50 pokoleń
    if (generation % 50 == 0) {
        float avgSpeed = (avgPreySpeed + avgPredatorSpeed) / 2.0f;
        float avgSize = (avgPreySize + avgPredatorSize) / 2.0f;

        m_avgSpeedHistory.append(avgSpeed);
        m_avgSizeHistory.append(avgSize);
        m_avgVisionHistory.append(avgSpeed * 10.0f); // Przykładowe dane

        // Ogranicz historię
        if (m_avgSpeedHistory.size() > 20) {
            m_avgSpeedHistory.removeFirst();
            m_avgSizeHistory.removeFirst();
            m_avgVisionHistory.removeFirst();
        }

        // Aktualizuj tabelę
        m_evolutionTable->insertRow(0);
        m_evolutionTable->setItem(0, 0, new QTableWidgetItem(QString::number(generation)));
        m_evolutionTable->setItem(0, 1, new QTableWidgetItem(QString::number(avgSpeed, 'f', 2)));
        m_evolutionTable->setItem(0, 2, new QTableWidgetItem(QString::number(avgSize, 'f', 2)));
        m_evolutionTable->setItem(0, 3, new QTableWidgetItem(QString::number(avgSpeed * 10.0f, 'f', 2)));

        // Ogranicz tabelę do 10 wierszy
        if (m_evolutionTable->rowCount() > 10) {
            m_evolutionTable->removeRow(10);
        }
    }
}

void SidePanel::updateCharts(const QVector<int> &preyHistory, const QVector<int> &predatorHistory)
{
    // Aktualizuj wykres populacji
    m_preySeries->clear();
    m_predatorSeries->clear();

    for (int i = 0; i < preyHistory.size(); ++i) {
        m_preySeries->append(i, preyHistory[i]);
        if (i < predatorHistory.size()) {
            m_predatorSeries->append(i, predatorHistory[i]);
        }
    }

    // Aktualizuj wykres ewolucji
    m_speedSeries->clear();
    m_sizeSeries->clear();
    m_visionSeries->clear();

    for (int i = 0; i < m_avgSpeedHistory.size(); ++i) {
        int generation = m_currentGeneration - (m_avgSpeedHistory.size() - i - 1) * 50;
        m_speedSeries->append(generation, m_avgSpeedHistory[i]);
        m_sizeSeries->append(generation, m_avgSizeHistory[i]);
        m_visionSeries->append(generation, m_avgVisionHistory[i]);
    }

    // POPRAWIAMY OSTRZEŻENIA - używamy zmiennej tymczasowej
    QList<QAbstractAxis*> axesList;

    // Dla osi Y
    axesList = m_chart->axes(Qt::Vertical);
    if (!axesList.isEmpty()) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(axesList.first());
        if (axisY) {
            int maxPopulation = 0;
            for (int count : preyHistory) {
                maxPopulation = qMax(maxPopulation, count);
            }
            for (int count : predatorHistory) {
                maxPopulation = qMax(maxPopulation, count);
            }
            axisY->setRange(0, qMax(10, maxPopulation + 10));
        }
    }

    // Dla osi X
    axesList = m_chart->axes(Qt::Horizontal);
    if (!axesList.isEmpty()) {
        QValueAxis *axisX = qobject_cast<QValueAxis*>(axesList.first());
        if (axisX) {
            axisX->setRange(0, qMax(1, preyHistory.size()));
        }
    }

    // Dla wykresu ewolucji - os Y
    axesList = m_evolutionChart->axes(Qt::Vertical);
    if (!axesList.isEmpty()) {
        QValueAxis *evoAxisY = qobject_cast<QValueAxis*>(axesList.first());
        if (evoAxisY) {
            float maxTrait = 0;
            // Używamy const reference dla uniknięcia ostrzeżeń
            const QVector<float>& speedHistory = m_avgSpeedHistory;
            const QVector<float>& sizeHistory = m_avgSizeHistory;
            const QVector<float>& visionHistory = m_avgVisionHistory;

            for (float speed : speedHistory) maxTrait = qMax(maxTrait, speed);
            for (float size : sizeHistory) maxTrait = qMax(maxTrait, size);
            for (float vision : visionHistory) maxTrait = qMax(maxTrait, vision);

            evoAxisY->setRange(0, qMax(1.0f, maxTrait + 1.0f));
        }
    }

    // Dla wykresu ewolucji - os X
    axesList = m_evolutionChart->axes(Qt::Horizontal);
    if (!axesList.isEmpty()) {
        QValueAxis *evoAxisX = qobject_cast<QValueAxis*>(axesList.first());
        if (evoAxisX) {
            int maxGen = m_currentGeneration;
            evoAxisX->setRange(qMax(0, maxGen - 1000), qMax(maxGen, 1));
        }
    }
}
