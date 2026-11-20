import Foundation
import CoreVideo

/// NDI 发送封装（UYVY，异步发送，零转码交接）
final class NDISender {
    private var instance: NDIlib_send_instance_t?
    private var pool: CVPixelBufferPool?
    private let side: Int
    private var lastBuffers: [CVPixelBuffer] = []
    private var nameBuffer: UnsafeMutablePointer<CChar>?
    var isReady: Bool { instance != nil }

    init(side: Int) {
        self.side = side

        // NDI 需要 C 字符串，且创建期间要保持有效 → strdup 后自己持有
        nameBuffer = strdup("iPhone NDI")

        var settings = NDIlib_send_create_t()
        settings.p_ndi_name = UnsafePointer<CChar>(nameBuffer)
        settings.p_groups = nil
        settings.clock_video = true
        settings.clock_audio = false
        instance = NDIlib_send_create(&settings)

        let attrs: [String: Any] = [
            kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_422YpCbCr8,
            kCVPixelBufferWidthKey as String: side,
            kCVPixelBufferHeightKey as String: side,
            kCVPixelBufferIOSurfacePropertiesKey as String: [:]
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
        // NDI 异步发送会占用最近几帧，保留引用避免被回收
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
        frame.FourCC = NDIlib_FourCC_video_type_UYVY
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
