#include <QVector3D>
#include <QQuaternion>

class DualQuaternion {
public:
    QQuaternion real;
    QQuaternion dual;

    DualQuaternion() : real(1, 0, 0, 0), dual(0, 0, 0, 0) {}

    DualQuaternion(const QQuaternion& realPart, const QQuaternion& dualPart)
        : real(realPart), dual(dualPart) {}

    static DualQuaternion fromTranslationRotation(const QVector3D& translation, const QQuaternion& rotation) {
        QQuaternion t(0, translation.x(), translation.y(), translation.z());
        DualQuaternion dq;
        dq.real = rotation;
        dq.dual = 0.5f * (t * rotation);
        return dq;
    }

    DualQuaternion operator*(const DualQuaternion& dq) const {
        return DualQuaternion(real * dq.real, real * dq.dual + dual * dq.real);
    }

    DualQuaternion operator*(float scalar) const {
        return DualQuaternion(real * scalar, dual * scalar);
    }

    DualQuaternion operator+(const DualQuaternion& dq) const {
        return DualQuaternion(real + dq.real, dual + dq.dual);
    }

    DualQuaternion normalized() const {
        float norm = real.length();
        return DualQuaternion(real / norm, dual / norm);
    }

    QQuaternion getRotation() const {
        return real;
    }

    QVector3D getTranslation() const {
        QQuaternion t = 2.0f * (dual * real.conjugated());
        return QVector3D(t.x(), t.y(), t.z());
    }
};
