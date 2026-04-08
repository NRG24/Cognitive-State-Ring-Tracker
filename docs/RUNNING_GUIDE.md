# Running the Cognitive Ring Tracker App

This guide covers every way you can build and run the Flutter iOS app, including local macOS + Xcode, pure Flutter CLI, and GitHub Codespaces / VS Code.

---

## Prerequisites (all methods)

| Tool | Version required | Install |
|------|-----------------|---------|
| Flutter SDK | ≥ 3.29.0 | [flutter.dev/docs/get-started/install](https://flutter.dev/docs/get-started/install) |
| Dart SDK | ≥ 3.9.2 (bundled with Flutter) | included with Flutter |
| Xcode | ≥ 15 (for iOS 13+ deployment) | Mac App Store |
| CocoaPods | any recent | `sudo gem install cocoapods` |
| Xcode Command Line Tools | latest | `xcode-select --install` |

> **iOS only** – Bluetooth Low Energy (BLE) is required and is only available on a physical device or, with limitations, the iOS Simulator (simulated BLE in Simulator does not work for real hardware). You must run on a real iPhone or iPad connected over USB to develop/test with the actual ring sensor.

---

## 1. Clone and get packages

```bash
git clone https://github.com/NRG24/Cognitive-State-Ring-Tracker.git
cd Cognitive-State-Ring-Tracker/app

# Download Dart/Flutter packages
flutter pub get

# Install iOS CocoaPods dependencies
cd ios && pod install && cd ..
```

Always use `Runner.xcworkspace` (not `Runner.xcodeproj`) after running `pod install`.

---

## 2. Running with Xcode (recommended for iOS)

1. Open the workspace in Xcode:

   ```bash
   open ios/Runner.xcworkspace
   ```

2. In the Xcode project navigator, select the **Runner** target → **Signing & Capabilities**.

3. Under **Team**, choose your Apple Developer account (free personal team is fine for development). The bundle ID is already set to `com.ryanschreiber.gsrstreamer` — you can keep it or change it to match your own Apple ID.

4. Connect your iPhone/iPad via USB and trust the Mac on the device.

5. Select your physical device from the device dropdown at the top of Xcode (Simulator will not work for BLE).

6. Press **▶ Run** (⌘R). Xcode will build, sign, and install the app.

### Building from the Flutter CLI instead of Xcode UI

```bash
# List connected devices
flutter devices

# Run on your connected iPhone (replace <device-id> with the ID shown above)
flutter run -d <device-id>

# Build a release IPA (requires a paid developer account for distribution)
flutter build ipa
```

---

## 3. Signing without a paid Apple Developer account

A **free** Apple ID works for on-device testing with the following restrictions:
- App expires after 7 days (re-install needed).
- Maximum 3 apps per device.
- No push notifications or certain entitlements.

Steps in Xcode:
1. **Signing & Capabilities** → uncheck **Automatically manage signing** if it gives errors.
2. Re-check it, choose your personal team, and let Xcode create a provisioning profile.
3. On your iPhone: **Settings → General → VPN & Device Management** → trust your developer certificate.

---

## 4. Permissions already configured

The following iOS permissions are already declared in `ios/Runner/Info.plist` — no changes needed:

| Permission key | Reason |
|---|---|
| `NSBluetoothAlwaysUsageDescription` | Connect to the ring's BLE module (nRF52840) |
| `NSBluetoothPeripheralUsageDescription` | Legacy key for iOS < 13 |
| `UIBackgroundModes: bluetooth-central` | Keep BLE connection alive while the app is backgrounded |

---

## 5. VS Code Codespaces — what works and what doesn't

GitHub Codespaces gives you a Linux-based cloud development environment accessed through VS Code in the browser (or the VS Code desktop app via the **GitHub Codespaces** extension).

### ✅ What you CAN do in Codespaces

| Task | Notes |
|------|-------|
| Edit Dart/Flutter source code | Full Dart language server, autocomplete, error highlighting via the Flutter/Dart extensions |
| Run `flutter pub get` | Downloads packages into the codespace |
| Run `flutter analyze` | Dart static analysis works fine |
| Run `flutter test` | Unit/widget tests that don't need native platform code run fine |
| Commit and push changes | Full git workflow |

### ❌ What you CANNOT do in Codespaces

| Task | Reason |
|------|--------|
| Run on iOS Simulator | Simulators require macOS + Xcode; Codespaces runs Linux |
| Run on a physical iPhone | USB passthrough to a cloud VM is not supported |
| Build an IPA | Requires macOS + Xcode toolchain |
| Test real BLE | Requires a physical device and Bluetooth hardware |

### Setting up VS Code Codespaces for editing

1. Open the repo on GitHub and click **Code → Codespaces → Create codespace on main** (or your branch).
2. Inside the codespace terminal, install Flutter:
   ```bash
   git clone https://github.com/flutter/flutter.git -b stable ~/flutter
   export PATH="$PATH:$HOME/flutter/bin"
   flutter precache --no-ios --no-android  # only downloads web/linux artifacts
   ```
3. Navigate to the app and get packages:
   ```bash
   cd app && flutter pub get
   ```
4. Install the **Flutter** and **Dart** VS Code extensions from the Extensions panel.

> **Recommended workflow**: Use Codespaces for reading and editing code, then push your changes and build/run on a Mac with Xcode. You can also use [GitHub Actions](https://docs.flutter.dev/deployment/cd) to automate IPA builds on a macOS runner.

---

## 6. Project structure at a glance

```
app/
├── lib/
│   ├── main.dart               # App entry point, tab scaffold
│   ├── ble_service.dart        # BLE scan + connection logic
│   ├── cognitive_scorer.dart   # Cognitive score algorithm
│   ├── gsr_analyzer.dart       # GSR signal processing
│   ├── models/                 # Data models
│   ├── screens/                # UI screens (Overview, Cognitive, Trends, Settings)
│   ├── services/               # BiometricMonitor and related services
│   └── widgets/                # Reusable UI components
├── ios/
│   ├── Runner.xcworkspace      # ← Always open this in Xcode (not .xcodeproj)
│   ├── Runner/
│   │   └── Info.plist          # iOS permissions & app metadata
│   └── Podfile                 # CocoaPods dependencies (iOS 13+)
├── pubspec.yaml                # Flutter/Dart dependencies
└── pubspec.lock                # Pinned dependency versions
```

---

## 7. Quick-start checklist

- [ ] macOS machine with Xcode ≥ 15 installed
- [ ] `flutter doctor` shows no errors for iOS toolchain
- [ ] Ran `flutter pub get` in `app/`
- [ ] Ran `pod install` in `app/ios/`
- [ ] Signed in to an Apple account in Xcode (free tier is fine)
- [ ] Physical iPhone/iPad connected and trusted
- [ ] Selected physical device in Xcode (not Simulator)
- [ ] Pressed ▶ or ran `flutter run -d <device-id>`
- [ ] On device: trusted the developer certificate (Settings → General → VPN & Device Management)

---

## 8. Troubleshooting

**`pod install` fails with "Flutter ROOT not found"**
Run `flutter pub get` first, then `pod install`.

**Signing errors in Xcode**
Go to Signing & Capabilities, uncheck then re-check "Automatically manage signing" and pick your team.

**BLE permission denied at runtime**
The first time the app launches it will request Bluetooth permission. Grant it. If denied, go to Settings → Privacy & Security → Bluetooth and enable it for the app.

**App crashes on launch**
Run `flutter run` from the terminal (instead of Xcode) to see Dart stack traces in real time.

**`flutter doctor` shows Xcode issues**
Run `sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer` and accept the license with `sudo xcodebuild -license accept`.
