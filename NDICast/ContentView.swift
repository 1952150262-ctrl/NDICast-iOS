import SwiftUI
import ReplayKit

let APP_GROUP = "group.com.local.ndicast"
let KEY_SIZE = "cropSize"

struct ContentView: View {
    @AppStorage(KEY_SIZE, store: UserDefaults(suiteName: APP_GROUP)) private var size: Int = 416

    var body: some View {
        NavigationView {
            VStack(spacing: 24) {
                Text("NDI 屏幕采集")
                    .font(.title2).bold()

                VStack(alignment: .leading, spacing: 8) {
                    Text("输出尺寸（居中方形裁剪）").font(.subheadline).foregroundColor(.secondary)
                    Picker("尺寸", selection: $size) {
                        Text("320 × 320").tag(320)
                        Text("416 × 416").tag(416)
                    }
                    .pickerStyle(.segmented)
                }

                Text("开始广播后，在控制中心/系统广播面板里选择「NDI Screen」。\n电脑上打开 NDI Studio Monitor 选同名源即可。")
                    .font(.footnote)
                    .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                    .padding(.horizontal)

                BroadcastPicker()
                    .frame(width: 44, height: 44)
                    .padding()

                Text("当前尺寸：\(size) × \(size)")
                    .font(.footnote)
                    .foregroundColor(.secondary)

                Spacer()
            }
            .padding()
            .navigationBarTitle("NDI Cast", displayMode: .inline)
            .onAppear {
                // 触发本地网络权限 + 预热 NDI 运行时
                let n = NDIPermission.warmup()
                print("NDI warmup, discovered sources: \(n)")
            }
        }
    }
}

/// 系统广播按钮（RPSystemBroadcastPickerView 的 SwiftUI 封装）
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
