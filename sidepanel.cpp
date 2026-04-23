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
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDir>*
#include <QDateTime>

SidePanel::SidePanel(QWidget *parent)
    : QWidget(parent)
    , m_currentGeneration(0)
{
    setFixedWidth(400);

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

void SidePanel::resetCharts()
{
    // Wyczyść wykres populacji
    m_preySeries->clear();
    m_predatorSeries->clear();

    // Wyczyść wykresy ewolucyjne ofiar
    m_preySpeedSeries->clear();
    m_preySizeSeries->clear();
    m_preyVisionSeries->clear();

    // Wyczyść wykresy ewolucyjne drapieżników
    m_predatorSpeedSeries->clear();
    m_predatorSizeSeries->clear();
    m_predatorVisionSeries->clear();

    // Zresetuj osie Y do domyślnych zakresów
    auto preyAxesY = m_preyEvolutionChart->axes(Qt::Vertical);
    if (!preyAxesY.isEmpty()) {
        auto axisY = qobject_cast<QValueAxis*>(preyAxesY.first());
        if (axisY) {
            axisY->setRange(0, 10);  // Domyślny zakres
        }
    }

    auto predAxesY = m_predatorEvolutionChart->axes(Qt::Vertical);
    if (!predAxesY.isEmpty()) {
        auto axisY = qobject_cast<QValueAxis*>(predAxesY.first());
        if (axisY) {
            axisY->setRange(0, 10);  // Domyślny zakres
        }
    }

    // Zresetuj osie X
    auto preyAxesX = m_preyEvolutionChart->axes(Qt::Horizontal);
    if (!preyAxesX.isEmpty()) {
        auto axisX = qobject_cast<QValueAxis*>(preyAxesX.first());
        if (axisX) {
            axisX->setRange(0, 1);
        }
    }

    auto predAxesX = m_predatorEvolutionChart->axes(Qt::Horizontal);
    if (!predAxesX.isEmpty()) {
        auto axisX = qobject_cast<QValueAxis*>(predAxesX.first());
        if (axisX) {
            axisX->setRange(0, 1);
        }
    }

    auto popAxesY = m_chart->axes(Qt::Vertical);
    if (!popAxesY.isEmpty()) {
        auto axisY = qobject_cast<QValueAxis*>(popAxesY.first());
        if (axisY) {
            axisY->setRange(0, 10);
        }
    }

    auto popAxesX = m_chart->axes(Qt::Horizontal);
    if (!popAxesX.isEmpty()) {
        auto axisX = qobject_cast<QValueAxis*>(popAxesX.first());
        if (axisX) {
            axisX->setRange(0, 1);
        }
    }
}

void SidePanel::setupChartsTab()
{
    m_chartsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

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

    // Przycisk eksportu dla wykresu populacji
    m_exportPopulationButton = new QPushButton(" Eksportuj dane populacji");
    m_exportPopulationButton->setIcon(QIcon::fromTheme("document-save"));
    connect(m_exportPopulationButton, &QPushButton::clicked, this, &SidePanel::exportPopulationChart);
    populationLayout->addWidget(m_exportPopulationButton);

    populationGroup->setLayout(populationLayout);
    layout->addWidget(populationGroup);

    QGroupBox *preyEvolutionGroup = new QGroupBox("Ewolucja cech ofiar");
    QVBoxLayout *preyEvolutionLayout = new QVBoxLayout();

    m_preyEvolutionChart = new QChart();
    m_preyEvolutionChart->setTitle("Ewolucja cech ofiar w czasie");
    m_preyEvolutionChart->setAnimationOptions(QChart::SeriesAnimations);

    m_preySpeedSeries = new QLineSeries();
    m_preySpeedSeries->setName("Szybkość");
    m_preySpeedSeries->setColor(QColor(0, 100, 255));
    m_preySpeedSeries->setPen(QPen(QColor(0, 100, 255), 2));

    m_preySizeSeries = new QLineSeries();
    m_preySizeSeries->setName("Rozmiar");
    m_preySizeSeries->setColor(QColor(255, 140, 0));
    m_preySizeSeries->setPen(QPen(QColor(255, 140, 0), 2));

    m_preyVisionSeries = new QLineSeries();
    m_preyVisionSeries->setName("Zasięg wzroku");
    m_preyVisionSeries->setColor(QColor(128, 0, 128));
    m_preyVisionSeries->setPen(QPen(QColor(128, 0, 128), 2));

    m_preyEvolutionChart->addSeries(m_preySpeedSeries);
    m_preyEvolutionChart->addSeries(m_preySizeSeries);
    m_preyEvolutionChart->addSeries(m_preyVisionSeries);

    QValueAxis *preyAxisX = new QValueAxis();
    preyAxisX->setTitleText("Czas (kroki)");
    preyAxisX->setLabelFormat("%d");
    preyAxisX->setTickCount(10);

    QValueAxis *preyAxisY = new QValueAxis();
    preyAxisY->setTitleText("Wartość cechy");
    preyAxisY->setLabelFormat("%.2f");

    m_preyEvolutionChart->addAxis(preyAxisX, Qt::AlignBottom);
    m_preyEvolutionChart->addAxis(preyAxisY, Qt::AlignLeft);

    m_preySpeedSeries->attachAxis(preyAxisX);
    m_preySpeedSeries->attachAxis(preyAxisY);
    m_preySizeSeries->attachAxis(preyAxisX);
    m_preySizeSeries->attachAxis(preyAxisY);
    m_preyVisionSeries->attachAxis(preyAxisX);
    m_preyVisionSeries->attachAxis(preyAxisY);

    m_preyEvolutionChartView = new QChartView(m_preyEvolutionChart);
    m_preyEvolutionChartView->setRenderHint(QPainter::Antialiasing);
    m_preyEvolutionChartView->setMinimumHeight(200);

    preyEvolutionLayout->addWidget(m_preyEvolutionChartView);

    // Przycisk eksportu dla wykresu ewolucji ofiar
    m_exportPreyEvolutionButton = new QPushButton(" Eksportuj dane ewolucji ofiar");
    m_exportPreyEvolutionButton->setIcon(QIcon::fromTheme("document-save"));
    connect(m_exportPreyEvolutionButton, &QPushButton::clicked, this, &SidePanel::exportPreyEvolutionChart);
    preyEvolutionLayout->addWidget(m_exportPreyEvolutionButton);

    preyEvolutionGroup->setLayout(preyEvolutionLayout);
    layout->addWidget(preyEvolutionGroup);

    QGroupBox *predatorEvolutionGroup = new QGroupBox("Ewolucja cech drapieżników");
    QVBoxLayout *predatorEvolutionLayout = new QVBoxLayout();

    m_predatorEvolutionChart = new QChart();
    m_predatorEvolutionChart->setTitle("Ewolucja cech drapieżników w czasie");
    m_predatorEvolutionChart->setAnimationOptions(QChart::SeriesAnimations);

    m_predatorSpeedSeries = new QLineSeries();
    m_predatorSpeedSeries->setName("Szybkość");
    m_predatorSpeedSeries->setColor(QColor(200, 0, 0));
    m_predatorSpeedSeries->setPen(QPen(QColor(200, 0, 0), 2));

    m_predatorSizeSeries = new QLineSeries();
    m_predatorSizeSeries->setName("Rozmiar");
    m_predatorSizeSeries->setColor(QColor(255, 165, 0));
    m_predatorSizeSeries->setPen(QPen(QColor(255, 165, 0), 2));

    m_predatorVisionSeries = new QLineSeries();
    m_predatorVisionSeries->setName("Zasięg wzroku");
    m_predatorVisionSeries->setColor(QColor(75, 0, 130));
    m_predatorVisionSeries->setPen(QPen(QColor(75, 0, 130), 2));

    m_predatorEvolutionChart->addSeries(m_predatorSpeedSeries);
    m_predatorEvolutionChart->addSeries(m_predatorSizeSeries);
    m_predatorEvolutionChart->addSeries(m_predatorVisionSeries);

    QValueAxis *predatorAxisX = new QValueAxis();
    predatorAxisX->setTitleText("Czas (kroki)");
    predatorAxisX->setLabelFormat("%d");
    predatorAxisX->setTickCount(10);

    QValueAxis *predatorAxisY = new QValueAxis();
    predatorAxisY->setTitleText("Wartość cechy");
    predatorAxisY->setLabelFormat("%.2f");

    m_predatorEvolutionChart->addAxis(predatorAxisX, Qt::AlignBottom);
    m_predatorEvolutionChart->addAxis(predatorAxisY, Qt::AlignLeft);

    m_predatorSpeedSeries->attachAxis(predatorAxisX);
    m_predatorSpeedSeries->attachAxis(predatorAxisY);
    m_predatorSizeSeries->attachAxis(predatorAxisX);
    m_predatorSizeSeries->attachAxis(predatorAxisY);
    m_predatorVisionSeries->attachAxis(predatorAxisX);
    m_predatorVisionSeries->attachAxis(predatorAxisY);

    m_predatorEvolutionChartView = new QChartView(m_predatorEvolutionChart);
    m_predatorEvolutionChartView->setRenderHint(QPainter::Antialiasing);
    m_predatorEvolutionChartView->setMinimumHeight(200);

    predatorEvolutionLayout->addWidget(m_predatorEvolutionChartView);

    // Przycisk eksportu dla wykresu ewolucji drapieżników
    m_exportPredatorEvolutionButton = new QPushButton(" Eksportuj dane ewolucji drapieżników");
    m_exportPredatorEvolutionButton->setIcon(QIcon::fromTheme("document-save"));
    connect(m_exportPredatorEvolutionButton, &QPushButton::clicked, this, &SidePanel::exportPredatorEvolutionChart);
    predatorEvolutionLayout->addWidget(m_exportPredatorEvolutionButton);

    predatorEvolutionGroup->setLayout(predatorEvolutionLayout);
    layout->addWidget(predatorEvolutionGroup);

    layout->addStretch();
    m_chartsTab->setLayout(layout);
}

void SidePanel::updateEvolutionCharts(
    const QVector<float> &preySpeedHistory,
    const QVector<float> &predatorSpeedHistory,
    const QVector<float> &preySizeHistory,
    const QVector<float> &predatorSizeHistory,
    const QVector<float> &preyVisionHistory,
    const QVector<float> &predatorVisionHistory)
{
    // Aktualizuj wykres ofiar
    m_preySpeedSeries->clear();
    m_preySizeSeries->clear();
    m_preyVisionSeries->clear();

    // Znajdź maksymalne wartości dla skalowania osi Y ofiar
    float maxPreySpeed = 0.1f;
    float maxPreySize = 0.1f;
    float maxPreyVision = 0.1f;

    for (int i = 0; i < preySpeedHistory.size(); ++i) {
        m_preySpeedSeries->append(i, preySpeedHistory[i]);
        m_preySizeSeries->append(i, preySizeHistory[i]);
        m_preyVisionSeries->append(i, preyVisionHistory[i]);

        maxPreySpeed = qMax(maxPreySpeed, preySpeedHistory[i]);
        maxPreySize = qMax(maxPreySize, preySizeHistory[i]);
        maxPreyVision = qMax(maxPreyVision, preyVisionHistory[i]);
    }

    // Aktualizuj wykres drapieżników
    m_predatorSpeedSeries->clear();
    m_predatorSizeSeries->clear();
    m_predatorVisionSeries->clear();

    // Znajdź maksymalne wartości dla skalowania osi Y drapieżników
    float maxPredatorSpeed = 0.1f;
    float maxPredatorSize = 0.1f;
    float maxPredatorVision = 0.1f;

    for (int i = 0; i < predatorSpeedHistory.size(); ++i) {
        m_predatorSpeedSeries->append(i, predatorSpeedHistory[i]);
        m_predatorSizeSeries->append(i, predatorSizeHistory[i]);
        m_predatorVisionSeries->append(i, predatorVisionHistory[i]);

        maxPredatorSpeed = qMax(maxPredatorSpeed, predatorSpeedHistory[i]);
        maxPredatorSize = qMax(maxPredatorSize, predatorSizeHistory[i]);
        maxPredatorVision = qMax(maxPredatorVision, predatorVisionHistory[i]);
    }

    // Dostosuj osie X i Y dla wykresu ofiar
    int maxPreyDataSize = qMax(preySpeedHistory.size(),
                               qMax(preySizeHistory.size(), preyVisionHistory.size()));

    // Oś X dla ofiar
    auto preyAxesX = m_preyEvolutionChart->axes(Qt::Horizontal);
    if (!preyAxesX.isEmpty()) {
        auto axisX = qobject_cast<QValueAxis*>(preyAxesX.first());
        if (axisX) {
            axisX->setRange(0, qMax(1, maxPreyDataSize));
        }
    }

    // Oś Y dla ofiar - dynamiczne skalowanie
    auto preyAxesY = m_preyEvolutionChart->axes(Qt::Vertical);
    if (!preyAxesY.isEmpty()) {
        auto axisY = qobject_cast<QValueAxis*>(preyAxesY.first());
        if (axisY) {
            float maxPreyValue = qMax(maxPreySpeed, qMax(maxPreySize, maxPreyVision));
            // Dodaj 10% marginesu na górze
            axisY->setRange(0, maxPreyValue * 1.1f);
        }
    }

    // Dostosuj osie X i Y dla wykresu drapieżników
    int maxPredatorDataSize = qMax(predatorSpeedHistory.size(),
                                   qMax(predatorSizeHistory.size(), predatorVisionHistory.size()));

    // Oś X dla drapieżników
    auto predAxesX = m_predatorEvolutionChart->axes(Qt::Horizontal);
    if (!predAxesX.isEmpty()) {
        auto axisX = qobject_cast<QValueAxis*>(predAxesX.first());
        if (axisX) {
            axisX->setRange(0, qMax(1, maxPredatorDataSize));
        }
    }

    // Oś Y dla drapieżników - dynamiczne skalowanie
    auto predAxesY = m_predatorEvolutionChart->axes(Qt::Vertical);
    if (!predAxesY.isEmpty()) {
        auto axisY = qobject_cast<QValueAxis*>(predAxesY.first());
        if (axisY) {
            float maxPredatorValue = qMax(maxPredatorSpeed,
                                          qMax(maxPredatorSize, maxPredatorVision));
            // Dodaj 10% marginesu na górze
            axisY->setRange(0, maxPredatorValue * 1.1f);
        }
    }
}

void SidePanel::exportPopulationChart()
{
    QVector<QPointF> preyData, predatorData;

    for (int i = 0; i < m_preySeries->count(); ++i) {
        preyData.append(m_preySeries->at(i));
    }

    for (int i = 0; i < m_predatorSeries->count(); ++i) {
        predatorData.append(m_predatorSeries->at(i));
    }

    exportChartData("populacja", preyData, "Ofiary", predatorData, "Drapiezniki");
}

void SidePanel::exportPreyEvolutionChart()
{
    QVector<QPointF> speedData, sizeData, visionData;

    for (int i = 0; i < m_preySpeedSeries->count(); ++i) {
        speedData.append(m_preySpeedSeries->at(i));
        sizeData.append(m_preySizeSeries->at(i));
        visionData.append(m_preyVisionSeries->at(i));
    }

    // Eksportuj do osobnego pliku z wieloma kolumnami
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = QString("ewolucja_ofiar_%1.csv").arg(timestamp);
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString filePath = QDir(downloadsPath).filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku:\n" + filePath);
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Nagłówek
    out << "Czas;Szybkosc_ofiar;Rozmiar_ofiar;Zasieg_wzroku_ofiar\n";

    // Dane
    int maxSize = qMax(speedData.size(), qMax(sizeData.size(), visionData.size()));
    for (int i = 0; i < maxSize; ++i) {
        out << i << ";";
        out << (i < speedData.size() ? QString::number(speedData[i].y(), 'f', 3) : "") << ";";
        out << (i < sizeData.size() ? QString::number(sizeData[i].y(), 'f', 3) : "") << ";";
        out << (i < visionData.size() ? QString::number(visionData[i].y(), 'f', 3) : "") << "\n";
    }

    file.close();

    QMessageBox::information(this, "Eksport zakończony",
                             QString("Dane zostały zapisane w pliku:\n%1").arg(filePath));
}

void SidePanel::exportPredatorEvolutionChart()
{
    QVector<QPointF> speedData, sizeData, visionData;

    for (int i = 0; i < m_predatorSpeedSeries->count(); ++i) {
        speedData.append(m_predatorSpeedSeries->at(i));
        sizeData.append(m_predatorSizeSeries->at(i));
        visionData.append(m_predatorVisionSeries->at(i));
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = QString("ewolucja_drapieznikow_%1.csv").arg(timestamp);
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString filePath = QDir(downloadsPath).filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku:\n" + filePath);
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "Czas;Szybkosc_drapieznikow;Rozmiar_drapieznikow;Zasieg_wzroku_drapieznikow\n";

    int maxSize = qMax(speedData.size(), qMax(sizeData.size(), visionData.size()));
    for (int i = 0; i < maxSize; ++i) {
        out << i << ";";
        out << (i < speedData.size() ? QString::number(speedData[i].y(), 'f', 3) : "") << ";";
        out << (i < sizeData.size() ? QString::number(sizeData[i].y(), 'f', 3) : "") << ";";
        out << (i < visionData.size() ? QString::number(visionData[i].y(), 'f', 3) : "") << "\n";
    }

    file.close();

    QMessageBox::information(this, "Eksport zakończony",
                             QString("Dane zostały zapisane w pliku:\n%1").arg(filePath));
}

void SidePanel::exportChartData(const QString &chartName,
                                const QVector<QPointF> &series1Data,
                                const QString &series1Name,
                                const QVector<QPointF> &series2Data,
                                const QString &series2Name)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = QString("%1_%2.csv").arg(chartName).arg(timestamp);
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString filePath = QDir(downloadsPath).filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku:\n" + filePath);
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Nagłówek
    out << "Czas;" << series1Name;
    if (!series2Data.isEmpty()) {
        out << ";" << series2Name;
    }
    out << "\n";

    // Dane
    int maxSize = qMax(series1Data.size(), series2Data.size());
    for (int i = 0; i < maxSize; ++i) {
        out << i << ";";
        out << (i < series1Data.size() ? QString::number(series1Data[i].y(), 'f', 2) : "") << ";";
        if (!series2Data.isEmpty()) {
            out << (i < series2Data.size() ? QString::number(series2Data[i].y(), 'f', 2) : "");
        }
        out << "\n";
    }

    file.close();

    QMessageBox::information(this, "Eksport zakończony",
                             QString("Dane zostały zapisane w pliku:\n%1").arg(filePath));
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
