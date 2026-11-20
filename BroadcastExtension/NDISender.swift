import Foundation
import CoreVideo
import CoreGraphics

/// NDI 发送封装（BGRA 直发，避免 UYVY 渲染出绿屏的问题）
final class NDISender {
    private var instance: NDIlib_send_instance_t?
    private var pool: CVPixelBufferPool?
    private let side: Int
    private var lastBuffers: [CVPixelBuffer] = []
    private var nameBuffer: UnsafeMutablePointer<CChar>?

    var isReady: Bool { instance != nil }

    init(side: Int) {
        self.side = side

        nameBuffer = strdup("iPhone NDI")
        var settings = NDIlib_send_create_t()
        settings.p_ndi_name = UnsafePointer<CChar>(nameBuffer)
        settings.p_groups = nil
        settings.clock_video = true
        settings.clock_audio = false
        instance = NDIlib_send_create(&settings)

        let attrs: [String: Any] = [
            kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32BGRA,
            kCVPixelBufferWidthKey as String: side,
            kCVPixelBufferHeightKey as String: side,
            kCVPixelBufferIOSurfacePropertiesKey as String: [:],
            kCVPixelBufferCGBitmapContextCompatibilityKey as String: true
        ]
        var p: CVPixelBufferPool?
        CVPixelBufferPoolCreate(kCFAllocatorDefault, nil, attrs as CFDictionary, &p)
        pool = p
    }

    func dequeueBuffer() -> CVPixelBuffer? {
        guard let pool = pool else { return nil }
        var pb: CVPixelBuffer?
        guard CVPixelBufferPoolCreatePixelBuffer(kCFAllocatorDefault, pool, &pb) == kCVReturnSuccess,
              let buffer = pb else { return nil }
        lastBuffers.append(buffer)
        if lastBuffers.count > 4 { lastBuffers.removeFirst() }
        return buffer
    }

    func send(_ pb: CVPixelBuffer) {
        guard let instance = instance else { return }
        CVPixelBufferLockBaseAddress(pb, .readOnly)
        defer { CVPixelBufferUnlockBaseAddress(pb, .readOnly) }

        guard let base = CVPixelBufferGetBaseAddress(pb) else { return }
        let stride = CVPixelBufferGetBytesPerRow(pb)

        var frame = NDIlib_video_frame_v2_t()
        frame.xres = Int32(side)
        frame.yres = Int32(side)
        frame.FourCC = NDIlib_FourCC_video_type_BGRA
        frame.frame_rate_N = 60000
        frame.frame_rate_D = 1001
        frame.picture_aspect_ratio = 1.0
        frame.frame_format_type = NDIlib_frame_format_type_progressive
        frame.line_stride_in_bytes = Int32(stride)
        frame.p_data = base.assumingMemoryBound(to: UInt8.self)
        frame.timecode = NDIlib_send_timecode_synthesize

        NDIlib_send_send_video_async_v2(instance, &frame)
    }

    deinit {
        if let i = instance { NDIlib_send_destroy(i) }
        if let n = nameBuffer { free(n) }
    }
}
