#include "index.hpp"
#include "../stop-first/index.hpp"

namespace slotv2::stop_second {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    // 第2停止専用の組合せ制御は、中・右の21コマ配列確定後に追加する。
    // 現段階では0〜4コマ・役別引き込み・🍒非成立時の蹴りを共通適用。
    return stop_first::resolve(ctx);
}

} // namespace slotv2::stop_second
