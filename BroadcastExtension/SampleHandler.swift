import ReplayKit
import CoreImage
import CoreVideo
import CoreMedia
import UIKit

/// 广播扩展：读取系统给的方向元数据 → 旋转 → 居中方形裁剪 → 缩放到 N×N → 送 NDI
final class SampleHandler: RPBroadcastSampleHandler {

    private var sender: NDISender?
    private var context: CIContext?
    private var side: Int = 416

    /// ReplayKit 的方向附件键（用字面量避免不同 SDK 里符号名不一致）
    private let orientationKey = "RPVideoSampleOrientationKey" as CFString

    override func broadcastStarted(withSetupInfo setupInfo: [String: NSObject]?) {
        let defaults = UserDefaults(suiteName: "group.com.local.ndicast")
        side = defaults?.integer(forKey: "cropSize") ?? 0
        if side != 320 && side != 416 { side = 416 }

        sender = NDISender(side: side)
        context = CIContext(options: [.cacheIntermediates: false])
    }

    override func broadcastFinished() {
        sender = nil
        context = nil
    }

    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        guard sampleBufferType == .video,
              let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer),
              let sender = sender,
              let context = context else { return }

        // 1) 方向元数据（6 = 需顺时针 90°，8 = 需逆时针 90°，1 = 正常）
        var orientation: CGImagePropertyOrientation = .up
        if let num = CMGetAttachment(sampleBuffer, key: orientationKey, attachmentModeOut: nil) as? NSNumber,
           let o = CGImagePropertyOrientation(rawValue: num.uint32Value) {
            orientation = o
        }

        // 2) 应用方向
        let src = CIImage(cvPixelBuffer: imageBuffer).oriented(orientation)
        let ext = src.extent
        guard ext.width > 1, ext.height > 1 else { return }

        // 3) 居中方形裁剪 + 缩放到 side×side（一次仿射完成）
        let s = min(ext.width, ext.height)
        let k = CGFloat(side) / s
        let dx = -(ext.minX + (ext.width - s) / 2) * k
        let dy = -(ext.minY + (ext.height - s) / 2) * k
        let tf = CGAffineTransform(a: k, b: 0, c: 0, d: k, tx: dx, ty: dy)
        let out = src.transformed(by: tf)

        // 4) 渲染到 UYVY 缓冲，直接交给 NDI
        guard let pb = sender.dequeueBuffer() else { return }
        let bounds = CGRect(x: 0, y: 0, width: side, height: side)
        context.render(out, to: pb, bounds: bounds, colorSpace: nil)

        sender.send(pb)
    }
}
