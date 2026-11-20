import Foundation

/// 触发 iOS「本地网络」权限弹窗，并顺手确认 NDI 运行时可用。
/// 广播扩展自己没法弹权限框，必须由主 App 先触发一次。
enum NDIPermission {
    @discardableResult
    static func warmup(timeoutMs: Int32 = 800) -> Int32 {
        guard NDIlib_initialize() else { return -1 }
        var settings = NDIlib_find_create_t()
        settings.show_local_sources = true
        guard let finder = NDIlib_find_create_v2(&settings) else { return -1 }
        NDIlib_find_wait_for_sources(finder, UInt32(timeoutMs))
        let count = Int32(NDIlib_find_get_current_sources(finder, nil))
        NDIlib_find_destroy(finder)
        return count
    }
}
