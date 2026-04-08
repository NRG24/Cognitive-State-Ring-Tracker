# Code Organization - Implementation Plan

## Current Status
- ✅ Created directory structure
- ✅ Extracted models (SessionData, HapticFeedbackType)
- ✅ Extracted BiometricMonitor service (901 lines)
- ⏳ Need to extract screens
- ⏳ Need to extract widgets
- ⏳ Need to update main.dart

## File Structure

```
lib/
├── main.dart (120 lines - clean entry point)
├── models/
│   ├── models.dart (barrel file)
│   ├── session_data.dart
│   └── haptic_feedback_type.dart
├── services/
│   └── biometric_monitor.dart (901 lines)
├── screens/
│   ├── overview_tab.dart (~600 lines)
│   ├── cognitive_tab.dart (~600 lines)
│   ├── trends_tab.dart (~600 lines)
│   └── settings_tab.dart (~250 lines)
├── widgets/
│   ├── connection_status_card.dart
│   ├── session_info_card.dart
│   ├── metric_card.dart
│   ├── temperature_card.dart
│   └── biometric_cards.dart
└── utils/
    └── formatters.dart
```

## Line Count Breakdown

### Current main.dart: 3106 lines
- MyApp + BiometricTabs: ~120 lines
- SessionData + enums: ~80 lines (moved to models/)
- BiometricMonitor: ~830 lines (moved to services/)
- OverviewTab: ~600 lines (will move to screens/)
- CognitiveTab: ~600 lines (will move to screens/)
- TrendsTab: ~600 lines (will move to screens/)
- SettingsTab: ~250 lines (will move to screens/)

### After refactoring:
- main.dart: ~120 lines (96% reduction!)
- Each file: 250-900 lines (manageable)

## Dependencies Between Files

```
main.dart
├── imports services/biometric_monitor.dart
├── imports screens/*.dart
│
screens/overview_tab.dart
├── imports services/biometric_monitor.dart
├── imports widgets/connection_status_card.dart
├── imports widgets/session_info_card.dart
├── imports widgets/metric_card.dart
├── imports widgets/temperature_card.dart
│
screens/cognitive_tab.dart
├── imports services/biometric_monitor.dart
│
screens/trends_tab.dart
├── imports services/biometric_monitor.dart
├── imports packages (fl_chart)
│
screens/settings_tab.dart
├── imports services/biometric_monitor.dart
├── imports utils/formatters.dart
```

## Implementation Steps

### Step 1: ✅ Create Directory Structure
All directories created

### Step 2: ✅ Extract Models
- ✅ models/haptic_feedback_type.dart
- ✅ models/session_data.dart  
- ✅ models/models.dart (barrel file)

### Step 3: ✅ Extract BiometricMonitor Service
- ✅ services/biometric_monitor.dart

### Step 4: ⏳ Extract Screens (In Progress)
Need to extract from current main.dart:
- [ ] screens/overview_tab.dart (lines 1020-1615)
- [ ] screens/cognitive_tab.dart (lines 1616-2241)
- [ ] screens/trends_tab.dart (lines 2242-2846)
- [ ] screens/settings_tab.dart (lines 2847-3106)

### Step 5: Extract Widgets (Optional but recommended)
Can extract reusable widget builders:
- [ ] widgets/connection_status_card.dart
- [ ] widgets/session_info_card.dart
- [ ] widgets/metric_card.dart
- [ ] widgets/temperature_card.dart

### Step 6: Create Utils (Optional)
- [ ] utils/formatters.dart (for _formatDuration, etc.)

### Step 7: Update main.dart
- [ ] Replace entire main.dart with clean version
- [ ] Import all necessary files
- [ ] Test compilation

### Step 8: Testing
- [ ] Run flutter pub get
- [ ] Fix any import errors
- [ ] Test app runs successfully
- [ ] Verify all features work

## Benefits After Refactoring

### For Development:
- ✅ Easy to find specific code
- ✅ Can work on one tab without seeing others
- ✅ Reduced merge conflicts in team settings
- ✅ Easier to test individual components
- ✅ Faster IDE/editor performance

### For Adding New Features:
- ✅ Clear where to add new analytics code (BiometricMonitor)
- ✅ Clear where to add new screens
- ✅ Can reuse widgets across screens
- ✅ Easy to add new models

### For Maintenance:
- ✅ Bug fixes are localized to specific files
- ✅ Can refactor one component without affecting others
- ✅ Easier code reviews
- ✅ Better git history

## Next Actions

**Ready to proceed with:**
1. Extract OverviewTab → screens/overview_tab.dart
2. Extract CognitiveTab → screens/cognitive_tab.dart
3. Extract TrendsTab → screens/trends_tab.dart
4. Extract SettingsTab → screens/settings_tab.dart
5. Replace main.dart with refactored version
6. Test and fix any issues

**Would you like me to:**
A) Continue extracting all screens automatically
B) Extract one screen at a time so you can review
C) Skip widgets extraction and just do screens + main.dart
D) Something else

Let me know and I'll proceed!
