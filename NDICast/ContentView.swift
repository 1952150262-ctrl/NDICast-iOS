import SwiftUI
import ReplayKit

let APP_GROUP = "group.com.local.ndicast"
let KEY_SIZE = "cropSize"

struct ContentView: View {
    @AppStorage(KEY_SIZE, store: UserDefaults(suiteName: APP_GROUP)) private var size: Int = 416
    @State private var statusText: String = "(点刷新查看广播扩展状态)"
    @State private var timer: Timer?

    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    Text("输出尺寸（居中方形裁剪）").font(.subheadline).foregroundColor(.secondary)
                    Picker("尺寸", selection: $size) {
                        Text("320 × 320").tag(320)
                        Text("416 × 416").tag(416)
                    }
                    .pickerStyle(.segmented)

                    HStack(spacing: 16) {
                        BroadcastPicker()
                            .frame(width: 44, height: 44)
                        Text("点这里开始广播（选「NDI Screen」）").font(.footnote).foregroundColor(.secondary)
                    }

                    Divider()

                    HStack {
                        Text("广播扩展状态").font(.headline)
                        Spacer()
                        Button("刷新") { statusText = Status.read() }
                            .buttonStyle(.bordered)
                    }

                    Text(statusText)
                        .font(.system(.caption, design: .monospaced))
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(8)
                        .background(Color.secondary.opacity(0.1))
                        .cornerRadius(8)

                    Text("提示：若本地网络权限没开，去 设置 → 隐私与安全性 → 本地网络 打开本 App。")
                        .font(.caption2).foregroundColor(.secondary)
                }
                .padding()
            }
            .navigationBarTitle("NDI Cast", displayMode: .inline)
            .onAppear {
                let n = NDIPermission.warmup()
                print("NDI warmup, sources: \(n)")
                statusText = Status.read()
                timer?.invalidate()
                timer = Timer.scheduledTimer(withTimeInterval: 2.0, repeats: true) { _ in
                    statusText = Status.read()
                }
            }
            .onDisappear { timer?.invalidate() }
        }
    }
}

struct BroadcastPicker: UIViewRepresentable {
    func makeUIView(context: Context) -> RPSystemBroadcastPickerView {
        let v = RPSystemBroadcastPickerView(frame: CGRect(x: 0, y: 0, width: 44, height: 44))
        v.preferredExtension = "com.local.ndicast.broadcast"
        v.showsMicrophoneButton = false
        return v
    }
    func updateUIView(_ uiView: RPSystemBroadcastPickerView, context: Context) {}
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View { ContentView() }
}
