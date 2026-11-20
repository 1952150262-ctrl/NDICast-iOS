import Foundation

/// 读取广播扩展写下的状态
enum Status {
    static let groupId = "group.com.local.ndicast"
    static let fileName = "ndicast-status.txt"

    static var fileURL: URL? {
        FileManager.default
            .containerURL(forSecurityApplicationGroupIdentifier: groupId)?
            .appendingPathComponent(fileName)
    }

    static func read() -> String {
        guard let url = fileURL else { return "(App Group 不可用 —— 签名时没带上 group.com.local.ndicast)" }
        guard let s = try? String(contentsOf: url, encoding: .utf8) else {
            return "(还没有状态文件：请先在系统里开始一次广播)"
        }
        return s
    }
}
