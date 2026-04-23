#ifndef SIDEPANEL_H
#define SIDEPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QtCharts>
#include <QComboBox>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;
class QTableWidget;
QT_END_NAMESPACE

QT_USE_NAMESPACE

    class SidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit SidePanel(QWidget *parent = nullptr);

signals:
    void simulationParametersChanged(
        float foodRegenerationMultiplier,
        int bushFoodLimit,
        float predatorEnergyMultiplier,
        float preyEnergyMultiplier,
        int initialPrey,
        int initialPredators,
        float predatorVisionMultiplier,
        float preyVisionMultiplier
        );
    void addPreyRequested(int count);
    void addPredatorRequested(int count);
    void addBushRequested();
    void addWaterRequested();
    void clearAllRequested();
    void restartSimulationRequested();

public slots:
    void updateStatistics(int preyCount, int predatorCount, int generation,
                          float avgPreySpeed, float avgPredatorSpeed,
                          float avgPreySize, float avgPredatorSize,
                          int births, int deaths);
    void updateCharts(const QVector<int> &preyHistory, const QVector<int> &predatorHistory);
    void resetCharts();

    void updateEvolutionCharts(
        const QVector<float> &preySpeedHistory,
        const QVector<float> &predatorSpeedHistory,
        const QVector<float> &preySizeHistory,
        const QVector<float> &predatorSizeHistory,
        const QVector<float> &preyVisionHistory,
        const QVector<float> &predatorVisionHistory
        );

private slots:
    void applyParameters();
    void resetToDefaults();

    void exportPopulationChart();
    void exportPreyEvolutionChart();
    void exportPredatorEvolutionChart();

private:
    void setupParametersTab();
    void setupChartsTab();
    void setupStatisticsTab();
    void setupControlTab();

    // NOWA METODA POMOCNICZA
    void setupEvolutionCharts();
    void exportChartData(const QString &chartName,
                         const QVector<QPointF> &series1Data,
                         const QString &series1Name,
                         const QVector<QPointF> &series2Data = QVector<QPointF>(),
                         const QString &series2Name = QString());

    QTabWidget *m_tabWidget;

    // Karty
    QWidget *m_parametersTab;
    QWidget *m_chartsTab;
    QWidget *m_statisticsTab;
    QWidget *m_controlTab;

    // Wykres populacji
    QChartView *m_chartView;
    QChart *m_chart;
    QLineSeries *m_preySeries;
    QLineSeries *m_predatorSeries;
    QPushButton *m_exportPopulationButton;

    // Wykres dla ofiar
    QChartView *m_preyEvolutionChartView;
    QChart *m_preyEvolutionChart;
    QLineSeries *m_preySpeedSeries;
    QLineSeries *m_preySizeSeries;
    QLineSeries *m_preyVisionSeries;
    QPushButton *m_exportPreyEvolutionButton;

    // Wykres dla drapieżników
    QChartView *m_predatorEvolutionChartView;
    QChart *m_predatorEvolutionChart;
    QLineSeries *m_predatorSpeedSeries;
    QLineSeries *m_predatorSizeSeries;
    QLineSeries *m_predatorVisionSeries;
    QPushButton *m_exportPredatorEvolutionButton;

    QComboBox *m_foodRegenerationCombo;
    QSpinBox *m_bushFoodLimitSpin;
    QComboBox *m_predatorEnergyCombo;
    QComboBox *m_preyEnergyCombo;
    QSpinBox *m_initialPreySpin;
    QSpinBox *m_initialPredatorsSpin;
    QComboBox *m_predatorVisionCombo;
    QComboBox *m_preyVisionCombo;

    // Statystyki
    QLabel *m_preyCountLabel;
    QLabel *m_predatorCountLabel;
    QLabel *m_generationLabel;
    QLabel *m_avgPreySpeedLabel;
    QLabel *m_avgPredatorSpeedLabel;
    QLabel *m_avgPreySizeLabel;
    QLabel *m_avgPredatorSizeLabel;
    QLabel *m_totalBirthsLabel;
    QLabel *m_totalDeathsLabel;
    QLabel *m_preyToPredatorRatioLabel;

    // Tabela ewolucji
    QTableWidget *m_evolutionTable;

    int m_currentGeneration;
    QVector<float> m_avgSpeedHistory;
    QVector<float> m_avgSizeHistory;
    QVector<float> m_avgVisionHistory;
};

#endif // SIDEPANEL_H
