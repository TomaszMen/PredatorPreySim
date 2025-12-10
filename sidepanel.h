#ifndef SIDEPANEL_H
#define SIDEPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QtCharts>

// Użyj Qt namespace bezpośrednio
QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;
class QPushButton;
QT_END_NAMESPACE

QT_USE_NAMESPACE

    class SidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit SidePanel(QWidget *parent = nullptr);

signals:
    void simulationParametersChanged(
        float preyReproductionRate,
        float predatorReproductionRate,
        float foodRegenerationRate,
        float energyConsumptionRate,
        float mutationRate,
        int initialPrey,
        int initialPredators
        );
    void addPreyRequested(int count);
    void addPredatorRequested(int count);
    void clearAllRequested();

public slots:
    void updateStatistics(int preyCount, int predatorCount, int generation);
    void updateCharts(const QVector<int> &preyHistory, const QVector<int> &predatorHistory);

private slots:
    void applyParameters();
    void resetToDefaults();

private:
    void setupParametersTab();
    void setupChartsTab();
    void setupControlTab();

    QTabWidget *m_tabWidget;

    // Karty
    QWidget *m_parametersTab;
    QWidget *m_chartsTab;
    QWidget *m_controlTab;

    // Wykresy
    QChartView *m_chartView;
    QChart *m_chart;
    QLineSeries *m_preySeries;
    QLineSeries *m_predatorSeries;

    // Parametry
    QDoubleSpinBox *m_preyReproductionSpin;
    QDoubleSpinBox *m_predatorReproductionSpin;
    QDoubleSpinBox *m_foodRegenerationSpin;
    QDoubleSpinBox *m_energyConsumptionSpin;
    QDoubleSpinBox *m_mutationRateSpin;
    QSpinBox *m_initialPreySpin;
    QSpinBox *m_initialPredatorsSpin;

    // Statystyki
    QLabel *m_preyCountLabel;
    QLabel *m_predatorCountLabel;
    QLabel *m_generationLabel;
    QLabel *m_avgPreySpeedLabel;
    QLabel *m_avgPredatorSpeedLabel;

    int m_currentGeneration;
};

#endif // SIDEPANEL_H
