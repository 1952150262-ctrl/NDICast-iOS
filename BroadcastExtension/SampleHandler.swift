import ReplayKit
import CoreImage
import CoreVideo
import CoreMedia
import UIKit

/// 广播扩展：读方向元数据 → 旋转 → 居中方形裁剪 → 缩放到 N×N → NDI 发送
final class SampleHandler: RPBroadcastSampleHandler {

    private var sender: NDISender?
    private var context: CIContext?
    private var side: Int = 416
    private var frames = 0
    private var ndiReady = false
    private let orientationKey = "RPVideoSampleOrientationKey" as CFString

    override func broadcastStarted(withSetupInfo: [String: NSObject]?) {
        Status.reset()
        Status.write("broadcastStarted")

        let defaults = UserDefaults(suiteName: Status.groupId)
        Status.write("appGroup defaults: \(defaults == nil ? "nil" : "ok")")
        side = defaults?.integer(forKey: "cropSize") ?? 0
        if side != 320 && side != 416 { side = 416 }
        Status.write("crop size = \(side)")

        // 1) NDI 运行时初始化（不调这个，send_create 必然返回空）
        let ok = NDIlib_initialize()
        Status.write("NDIlib_initialize -> \(ok)")

        // 2) 创建发送器
        sender = NDISender(side: side)
        ndiReady = sender?.isReady ?? false
        Status.write("NDIlib_send_create -> \(ndiReady ? "ok" : "FAILED")")

        context = CIContext(options: [.cacheIntermediates: false])
        Status.write("CIContext ready")
    }

    override func broadcastFinished() {
        Status.write("broadcastFinished, frames=\(frames)")
        sender = nil
        context = nil
    }

    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        guard sampleBufferType == .video else { return }
        frames += 1
        if frames == 1 { Status.write("first video frame") }
        if frames % 60 == 0 { Status.write("frames=\(frames)") }

        guard let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer),
              let sender = sender,
              let context = context else {
            if frames == 1 { Status.write("guard failed: imageBuffer/sender/context 缺失") }
            return
        }

        // 系统附件拿不到时（ReplayKit 在部分场景不挂这个键）退回固定 90°，
        // 因为 iPhone 面板是竖的、横屏游戏的像素一定是转着的。
        var orientation: CGImagePropertyOrientation = .right
        if let num = CMGetAttachment(sampleBuffer, key: orientationKey, attachmentModeOut: nil) as? NSNumber,
           let o = CGImagePropertyOrientation(rawValue: num.uint32Value) {
            orientation = o
        }
        if frames == 1 { Status.write("orientation = \(orientation.rawValue)") }

        let src = CIImage(cvPixelBuffer: imageBuffer).oriented(orientation)
        let ext = src.extent
        guard ext.width > 1, ext.height > 1 else { return }
        if frames == 1 { Status.write("extent = \(Int(ext.width))x\(Int(ext.height))") }

        let s = min(ext.width, ext.height)
        let k = CGFloat(side) / s
        let dx = -(ext.minX + (ext.width - s) / 2) * k
        let dy = -(ext.minY + (ext.height - s) / 2) * k
        let tf = CGAffineTransform(a: k, b: 0, c: 0, d: k, tx: dx, ty: dy)
        let out = src.transformed(by: tf)

        guard let pb = sender.dequeueBuffer() else {
            if frames == 1 { Status.write("dequeueBuffer failed") }
            return
        }
        let bounds = CGRect(x: 0, y: 0, width: side, height: side)
        context.render(out, to: pb, bounds: bounds, colorSpace: CGColorSpaceCreateDeviceRGB())
        sender.send(pb)
    }
}
