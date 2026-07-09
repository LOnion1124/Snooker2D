#include "Table.h"
#include "../common/Constants.h"

namespace Snooker2D {

Table::Table(QObject* parent)
    : QObject(parent)
    , m_width(TABLE_WIDTH)
    , m_height(TABLE_HEIGHT)
{
    initCushions();
    initPockets();
}

double Table::width() const {
    return m_width;
}

double Table::height() const {
    return m_height;
}

bool Table::isInsideTable(const Vector2D& pos) const {
    double halfW = m_width / 2.0;
    double halfH = m_height / 2.0;
    return pos.x >= -halfW && pos.x <= halfW &&
           pos.y >= -halfH && pos.y <= halfH;
}

void Table::initCushions() {
    double hw = m_width / 2.0;
    double hh = m_height / 2.0;
    double pocketCut = POCKET_RADIUS * 1.5; // 袋口处的库边缺口

    // 上库边（两段，中间留角袋缺口）
    m_cushions.push_back({Vector2D(-hw + pocketCut, -hh), Vector2D(-pocketCut, -hh)});
    m_cushions.push_back({Vector2D(pocketCut, -hh), Vector2D(hw - pocketCut, -hh)});
    // 下库边
    m_cushions.push_back({Vector2D(-hw + pocketCut, hh), Vector2D(-pocketCut, hh)});
    m_cushions.push_back({Vector2D(pocketCut, hh), Vector2D(hw - pocketCut, hh)});
    // 左库边
    m_cushions.push_back({Vector2D(-hw, -hh + pocketCut), Vector2D(-hw, -pocketCut)});
    m_cushions.push_back({Vector2D(-hw, pocketCut), Vector2D(-hw, hh - pocketCut)});
    // 右库边
    m_cushions.push_back({Vector2D(hw, -hh + pocketCut), Vector2D(hw, -pocketCut)});
    m_cushions.push_back({Vector2D(hw, pocketCut), Vector2D(hw, hh - pocketCut)});
}

void Table::initPockets() {
    double hw = m_width / 2.0;
    double hh = m_height / 2.0;

    // 6 个袋口：4 角 + 2 中袋
    m_pockets.push_back({Vector2D(-hw, -hh), POCKET_RADIUS}); // 左上角
    m_pockets.push_back({Vector2D( hw, -hh), POCKET_RADIUS}); // 右上角
    m_pockets.push_back({Vector2D(-hw,  hh), POCKET_RADIUS}); // 左下角
    m_pockets.push_back({Vector2D( hw,  hh), POCKET_RADIUS}); // 右下角
    m_pockets.push_back({Vector2D(0.0, -hh), POCKET_RADIUS}); // 上中袋
    m_pockets.push_back({Vector2D(0.0,  hh), POCKET_RADIUS}); // 下中袋
}

} // namespace Snooker2D
