import Foundation

/// 广播扩展不能直接看日志 → 把状态写进 App Group，主 App 里显示出来
enum Status {
    static let groupId = "group.com.local.ndicast"
    static let fileName = "ndicast-status.txt"

    static var fileURL: URL? {
        FileManager.default
            .containerURL(forSecurityApplicationGroupIdentifier: groupId)?
            .appendingPathComponent(fileName)
    }

    static func write(_ line: String) {
        let ts = ISO8601DateFormatter().string(from: Date())
        let text = "[\(ts)] \(line)\n"
        guard let url = fileURL else {
            // App Group 不可用：退回 App 自己的 Documents（扩展写不进去，但便于发现）
            let fallback = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)[0]
                .appendingPathComponent(fileName)
            append(text, to: fallback)
            return
        }
        append(text, to: url)
    }

    static func reset() {
        if let url = fileURL { try? FileManager.default.removeItem(at: url) }
    }

    static func read() -> String {
        guard let url = fileURL, let s = try? String(contentsOf: url, encoding: .utf8) else {
            return "(无状态文件：App Group 可能不可用)"
        }
        return s
    }

    private static func append(_ text: String, to url: URL) {
        if let h = try? FileHandle(forWritingTo: url) {
            h.seekToEndOfFile()
            h.write(text.data(using: .utf8)!)
            try? h.close()
        } else {
            try? text.data(using: .utf8)!.write(to: url)
        }
    }
}
