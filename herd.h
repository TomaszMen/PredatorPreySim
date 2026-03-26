#ifndef HERD_H
#define HERD_H

#include <QObject>
#include <QVector>
#include <QPointF>

// Forward declaration
class Prey;
class Environment;

class Herd : public QObject
{
    Q_OBJECT

public:
    explicit Herd(QObject *parent = nullptr);

    void addMember(Prey* prey);
    void removeMember(Prey* prey);
    void update();

    Prey* getAlpha() const { return m_alpha; }
    QPointF getCenter() const;
    int getSize() const { return m_members.size(); }
    QVector<Prey*> getMembers() const { return m_members; }

    void setTargetPosition(const QPointF& target);
    void setTargetEnvironment(Environment* target);

    QPointF getTargetPosition() const { return m_targetPosition; }
    Environment* getTargetEnvironment() const { return m_targetEnvironment; }
    bool hasTarget() const { return m_targetPosition != QPointF(-1, -1) || m_targetEnvironment != nullptr; }
    void clearTarget();

signals:
    void alphaChanged(Prey* newAlpha);
    void herdMoved(const QPointF& center);
    void targetReached();

private:
    void selectNewAlpha();
    float calculateFitness(Prey* prey) const;
    void chooseNewTarget(bool force = false);  // DODANE: parametr domyślny
    bool isTargetReached() const;

    QVector<Prey*> m_members;
    Prey* m_alpha;
    QPointF m_targetPosition;
    Environment* m_targetEnvironment;
    int m_updateCounter;
    int m_targetPersistenceCounter;
    static const int TARGET_PERSISTENCE = 500;

    // DODANE: zmienne do śledzenia bezruchu
    int m_idleTicks;
    static const int IDLE_THRESHOLD = 400;
};

#endif // HERD_H
