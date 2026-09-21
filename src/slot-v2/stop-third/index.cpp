#include "index.hpp"
#include "../stop-first/index.hpp"

namespace slotv2::stop_third {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    // 最終停止での中段1ライン入賞保護は、中・右配列確定後にここへ限定実装する。
    // 停止後に内部フラグを書き換える処理は置かない。
    return stop_first::resolve(ctx);
}

} // namespace slotv2::stop_third
