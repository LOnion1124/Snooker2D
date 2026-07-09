#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include <vector>
#include "../common/Types.h"

namespace Snooker2D {

// 库边结构
struct Cushion {
    Vector2D p1; // 端点 1
    Vector2D p2; // 端点 2
};

// 袋口结构
struct Pocket {
    Vector2D position;
    double radius;
};

class Table : public QObject {
    Q_OBJECT

public:
    explicit Table(QObject* parent = nullptr);
    ~Table() override = default;

    // 球桌尺寸
    double width() const;
    double height() const;

    // 库边与袋口
    const std::vector<Cushion>& cushions() const { return m_cushions; }
    const std::vector<Pocket>& pockets() const { return m_pockets; }

    // 判断球是否在台面内
    bool isInsideTable(const Vector2D& pos) const;

private:
    void initCushions();
    void initPockets();

    double m_width = 0.0;
    double m_height = 0.0;
    std::vector<Cushion> m_cushions; // 6 条库边
    std::vector<Pocket> m_pockets;   // 6 个袋口
};

} // namespace Snooker2D

#endif // TABLE_H
