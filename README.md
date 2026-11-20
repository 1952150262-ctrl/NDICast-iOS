# NDI Cast（iOS）

自写的 iOS 屏幕采集 → NDI 应用。**横屏自动识别 + 可选 320/416 方形裁剪**，只做 NDI，不含 UDP。

## 和现有方案的区别

| | 畅享助手 | NDIScreen | **NDI Cast** |
|---|---|---|---|
| 横屏方向 | 需补丁 | 需补丁 | **读 `RPVideoSampleOrientationKey` 自动转正** |
| 裁剪尺寸 | 固定 416（缩放不是裁剪） | 无 | **320 / 416 可选，真·居中裁剪** |
| 变形 | 有 | 无 | **无** |
| NDI | ✅ | ✅ | ✅ |

## 目录

```
NDICast/
├── project.yml                     XcodeGen 工程定义
├── NDICast/                        主 App（选尺寸 + 系统广播按钮）
├── BroadcastExtension/             广播扩展（采集 + 旋转 + 裁剪 + NDI）
└── .github/workflows/build.yml     GitHub Actions 自动打包
```

## 一、准备 NDI SDK（一次性）

1. 打开 <https://ndi.video/for-developers/ndi-sdk/>，下载 **NDI SDK for iOS**（免费，需接受许可）；
2. 解压后把下面两样放进工程的 `vendor/NDI/`：

```
vendor/NDI/
├── include/                ← Processing.NDI.Lib.h 等所有头文件
└── libndi_ios.a            ← 静态库（arm64）
```

最终应当是 `vendor/NDI/include/Processing.NDI.Lib.h` 存在。

## 二、打包（不需要 Mac）

1. 新建 GitHub 私有仓库，把本目录推上去（含 `vendor/NDI/`，文件较大可先提交）；
2. 仓库 → Actions → **build-ipa** → Run workflow；
3. 等 3~5 分钟，下载 artifact `NDICast-unsigned`。

## 三、安装

1. 解压得到 `NDICast-unsigned.ipa`；
2. 用 **Sideloadly** 签名安装（和你现在装其他包一样）；
   - 免费证书 7 天有效，到期重装；
   - 首次需要在 设置 → 通用 → VPN与设备管理 里信任证书。
3. 打开 App → 选 **320 / 416** → 点系统广播按钮 → 选「NDI Screen」开始广播；
4. 电脑 NDI Studio Monitor 里选 `iPhone NDI`。

## 四、实现要点

```swift
// 方向：系统给的是元数据，像素本身永远按面板方向（竖屏）投递
let orientation = CMGetAttachment(sb, key: kRPVideoSampleOrientationKey as CFString, nil)

// 一次仿射完成 旋转 + 居中方形裁剪 + 缩放
let s  = min(ext.width, ext.height)
let k  = CGFloat(side) / s                  // side = 320 或 416
let dx = -(ext.minX + (ext.width  - s) / 2) * k
let dy = -(ext.minY + (ext.height - s) / 2) * k
let tf = CGAffineTransform(a: k, b: 0, c: 0, d: k, tx: dx, ty: dy)

// 直接渲染成 UYVY，交给 NDI 异步发送（省内存、省一次转换）
context.render(src.transformed(by: tf), to: uyvyBuffer, bounds: ..., colorSpace: nil)
NDIlib_send_send_video_async_v2(sender, &frame)
```

## 四·五、iPhone 16 Pro 的分辨率说明

| 项 | 数值 |
|---|---|
| 面板物理像素 | **1206 × 2622** |
| 竖屏缓冲（ReplayKit 投递） | 1206 × 2622 |
| 横屏游戏时 | 缓冲**仍是** 1206 × 2622，内容在其中旋转 90° 绘制，方向由 `RPVideoSampleOrientationKey` 告知 |
| 居中方形可用像素 | **1206 × 1206** |
| 输出 320 | 1206 → 320，缩小 3.77×（清晰） |
| 输出 416 | 1206 → 416，缩小 2.90×（清晰） |

代码里的裁剪是**按实际 extent 动态计算**的（`s = min(ext.width, ext.height)`），
所以在 16 Pro 上是 1206×1206 → 320/416；换别的机型也自动适配，不需要改任何常量。

## 五、已知限制

- **帧率上限 60**：ReplayKit 广播扩展的系统限制；
- 广播扩展内存上限约 50MB，所以用 UYVY（每像素 2 字节）而不是 BGRA；
- 免费证书 7 天需重签。

## 六、想改的地方

| 需求 | 改哪里 |
|---|---|
| 增加 640/1280 档 | `ContentView.swift` 的 Picker + `SampleHandler` 的合法值判断 |
| 改成 4:3 裁剪（比方形视野宽） | `SampleHandler` 里把 `s = min(w,h)` 改成按目标比例算 `cw/ch` |
| 裁剪位置不在正中 | 调整 `dx/dy` 里的 `(ext.width - s)/2` 偏移 |
| 加音频 | `RPSampleBufferType.audioApp` → NDI audio |
