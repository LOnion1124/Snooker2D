#include "GameUiBus.h"
#include "GameViewState.h"

#include <QMetaType>

namespace Snooker2D {

// 在应用启动阶段注册所有跨线程传递的自定义类型
// 调用一次 qRegisterMetaType<T>() 即可，重复调用幂等。
void registerContractTypes() {
    qRegisterMetaType<BallViewState>("BallViewState");
    qRegisterMetaType<TableViewState>("TableViewState");
    qRegisterMetaType<CueViewState>("CueViewState");
    qRegisterMetaType<ScoreViewState>("ScoreViewState");
    qRegisterMetaType<GameInfoViewState>("GameInfoViewState");
}

} // namespace Snooker2D
