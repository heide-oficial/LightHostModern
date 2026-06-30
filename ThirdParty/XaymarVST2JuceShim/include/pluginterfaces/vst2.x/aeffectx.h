/*
  Experimental JUCE compatibility surface for Xaymar/vst2sdk.

  The declarations here are the legacy names used by JUCE's VST2 host. Numeric
  opcode/flag values match the VST2 ABI so existing Windows VST2 plugins can be
  tested without relying on the discontinued Steinberg headers.
*/

#pragma once

#include "aeffect.h"

enum
{
    VST_BUFFER_SIZE_PARAM_LABEL = 8,
    VST_BUFFER_SIZE_STREAM_LABEL = 8,
    VST_BUFFER_SIZE_CATEGORY_LABEL = 24,
    VST_BUFFER_SIZE_EFFECT_NAME = 64,
    VST_BUFFER_SIZE_PRODUCT_NAME = 64,
    VST_BUFFER_SIZE_VENDOR_NAME = 64,

    VST_HOST_OPCODE_AUTOMATE = 0,
    VST_HOST_OPCODE_VST_VERSION = 1,
    VST_HOST_OPCODE_CURRENT_EFFECT_ID = 2,
    VST_HOST_OPCODE_KEEPALIVE_OR_IDLE = 3,
    VST_HOST_OPCODE_IO_MODIFIED = 13,
    VST_HOST_OPCODE_EDITOR_RESIZE = 15,
    VST_HOST_OPCODE_GET_SAMPLE_RATE = 16,
    VST_HOST_OPCODE_GET_BLOCK_SIZE = 17,
    VST_HOST_OPCODE_INPUT_LATENCY = 18,
    VST_HOST_OPCODE_OUTPUT_LATENCY = 19,
    VST_HOST_OPCODE_INPUT_GET_ATTACHED_EFFECT = 20,
    VST_HOST_OPCODE_OUTPUT_GET_ATTACHED_EFFECT = 21,
    VST_HOST_OPCODE_GET_ACTIVE_THREAD = 23,
    VST_HOST_OPCODE_OUTPUT_GET_SPEAKER_ARRANGEMENT = 31,
    VST_HOST_OPCODE_VENDOR_NAME = 32,
    VST_HOST_OPCODE_PRODUCT_NAME = 33,
    VST_HOST_OPCODE_VENDOR_VERSION = 34,
    VST_HOST_OPCODE_CUSTOM = 35,
    VST_HOST_OPCODE_SUPPORTS = 37,
    VST_HOST_OPCODE_LANGUAGE = 38,
    VST_HOST_OPCODE_GET_EFFECT_DIRECTORY = 41,
    VST_HOST_OPCODE_REFRESH = 42,
    VST_HOST_OPCODE_PARAM_START_EDIT = 43,
    VST_HOST_OPCODE_PARAM_STOP_EDIT = 44,

    VST_EVENT_TYPE_MIDI = 1,
    VST_EVENT_TYPE_MIDI_SYSEX = 6,

    VST_EFFECT_CATEGORY_UNCATEGORIZED = 0,
    VST_EFFECT_CATEGORY_EFFECT = 1,
    VST_EFFECT_CATEGORY_INSTRUMENT = 2,
    VST_EFFECT_CATEGORY_METERING = 3,
    VST_EFFECT_CATEGORY_MASTERING = 4,
    VST_EFFECT_CATEGORY_SPATIAL = 5,
    VST_EFFECT_CATEGORY_DELAY_OR_ECHO = 6,
    VST_EFFECT_CATEGORY_EXTERNAL = 7,
    VST_EFFECT_CATEGORY_RESTORATION = 8,
    VST_EFFECT_CATEGORY_OFFLINE = 9,
    VST_EFFECT_CATEGORY_CONTAINER = 10,
    VST_EFFECT_CATEGORY_WAVEGENERATOR = 11,
    VST_EFFECT_CATEGORY_MAX = 12,

    VST_STREAM_FLAG_STEREO = 1 << 1,
    VST_STREAM_FLAG_USE_TYPE = 1 << 2,

    VST_SPEAKER_ARRANGEMENT_TYPE_CUSTOM = -2,
    VST_SPEAKER_ARRANGEMENT_TYPE_UNKNOWN = -1,
    VST_SPEAKER_ARRANGEMENT_TYPE_MONO = 0,
    VST_SPEAKER_ARRANGEMENT_TYPE_STEREO = 1,
    VST_SPEAKER_ARRANGEMENT_TYPE_5_0 = 14,
    VST_SPEAKER_ARRANGEMENT_TYPE_5_1 = 15,
    VST_SPEAKER_ARRANGEMENT_TYPE_7_1 = 23,

    VST_SPEAKER_TYPE_LEFT = 1,
    VST_SPEAKER_TYPE_RIGHT = 2,
    VST_SPEAKER_TYPE_CENTER = 3,
    VST_SPEAKER_TYPE_LFE = 4,
    VST_SPEAKER_TYPE_LEFT_REAR = 5,
    VST_SPEAKER_TYPE_RIGHT_REAR = 6,
    VST_SPEAKER_TYPE_LEFT_SIDE = 10,
    VST_SPEAKER_TYPE_RIGHT_SIDE = 11
};

enum AudioMasterOpcodes
{
    audioMasterAutomate = VST_HOST_OPCODE_AUTOMATE,
    audioMasterVersion = VST_HOST_OPCODE_VST_VERSION,
    audioMasterCurrentId = VST_HOST_OPCODE_CURRENT_EFFECT_ID,
    audioMasterIdle = VST_HOST_OPCODE_KEEPALIVE_OR_IDLE,
    audioMasterPinConnected = 4,
    audioMasterWantMidi = 6,
    audioMasterGetTime = 7,
    audioMasterProcessEvents = 8,
    audioMasterSetTime = 9,
    audioMasterTempoAt = 10,
    audioMasterGetNumAutomatableParameters = 11,
    audioMasterGetParameterQuantization = 12,
    audioMasterIOChanged = VST_HOST_OPCODE_IO_MODIFIED,
    audioMasterNeedIdle = 14,
    audioMasterSizeWindow = VST_HOST_OPCODE_EDITOR_RESIZE,
    audioMasterGetSampleRate = VST_HOST_OPCODE_GET_SAMPLE_RATE,
    audioMasterGetBlockSize = VST_HOST_OPCODE_GET_BLOCK_SIZE,
    audioMasterGetInputLatency = VST_HOST_OPCODE_INPUT_LATENCY,
    audioMasterGetOutputLatency = VST_HOST_OPCODE_OUTPUT_LATENCY,
    audioMasterGetPreviousPlug = VST_HOST_OPCODE_INPUT_GET_ATTACHED_EFFECT,
    audioMasterGetNextPlug = VST_HOST_OPCODE_OUTPUT_GET_ATTACHED_EFFECT,
    audioMasterWillReplaceOrAccumulate = 22,
    audioMasterGetCurrentProcessLevel = VST_HOST_OPCODE_GET_ACTIVE_THREAD,
    audioMasterGetAutomationState = 24,
    audioMasterOfflineStart = 25,
    audioMasterOfflineRead = 26,
    audioMasterOfflineWrite = 27,
    audioMasterOfflineGetCurrentPass = 28,
    audioMasterOfflineGetCurrentMetaPass = 29,
    audioMasterSetOutputSampleRate = 30,
    audioMasterGetOutputSpeakerArrangement = VST_HOST_OPCODE_OUTPUT_GET_SPEAKER_ARRANGEMENT,
    audioMasterGetVendorString = VST_HOST_OPCODE_VENDOR_NAME,
    audioMasterGetProductString = VST_HOST_OPCODE_PRODUCT_NAME,
    audioMasterGetVendorVersion = VST_HOST_OPCODE_VENDOR_VERSION,
    audioMasterVendorSpecific = VST_HOST_OPCODE_CUSTOM,
    audioMasterSetIcon = 36,
    audioMasterCanDo = VST_HOST_OPCODE_SUPPORTS,
    audioMasterGetLanguage = VST_HOST_OPCODE_LANGUAGE,
    audioMasterOpenWindow = 39,
    audioMasterCloseWindow = 40,
    audioMasterGetDirectory = VST_HOST_OPCODE_GET_EFFECT_DIRECTORY,
    audioMasterUpdateDisplay = VST_HOST_OPCODE_REFRESH,
    audioMasterBeginEdit = VST_HOST_OPCODE_PARAM_START_EDIT,
    audioMasterEndEdit = VST_HOST_OPCODE_PARAM_STOP_EDIT
};

enum VstEventTypes
{
    kVstMidiType = VST_EVENT_TYPE_MIDI,
    kVstSysExType = VST_EVENT_TYPE_MIDI_SYSEX
};

enum VstTimeInfoFlags
{
    kVstTransportChanged = 1 << 0,
    kVstTransportPlaying = 1 << 1,
    kVstTransportCycleActive = 1 << 2,
    kVstTransportRecording = 1 << 3,
    kVstAutomationWriting = 1 << 6,
    kVstAutomationReading = 1 << 7,
    kVstNanosValid = 1 << 8,
    kVstPpqPosValid = 1 << 9,
    kVstTempoValid = 1 << 10,
    kVstBarsValid = 1 << 11,
    kVstCyclePosValid = 1 << 12,
    kVstTimeSigValid = 1 << 13,
    kVstSmpteValid = 1 << 14,
    kVstClockValid = 1 << 15
};

typedef VstInt32 VstSmpteFrameRate;
static constexpr VstSmpteFrameRate kVstSmpte24fps = 0;
static constexpr VstSmpteFrameRate kVstSmpte25fps = 1;
static constexpr VstSmpteFrameRate kVstSmpte2997fps = 2;
static constexpr VstSmpteFrameRate kVstSmpte30fps = 3;
static constexpr VstSmpteFrameRate kVstSmpte2997dfps = 4;
static constexpr VstSmpteFrameRate kVstSmpte30dfps = 5;
static constexpr VstSmpteFrameRate kVstSmpteFilm16mm = 6;
static constexpr VstSmpteFrameRate kVstSmpteFilm35mm = 7;
static constexpr VstSmpteFrameRate kVstSmpte239fps = 10;
static constexpr VstSmpteFrameRate kVstSmpte249fps = 11;
static constexpr VstSmpteFrameRate kVstSmpte599fps = 12;
static constexpr VstSmpteFrameRate kVstSmpte60fps = 13;

enum VstProcessPrecision
{
    kVstProcessPrecision32 = 0,
    kVstProcessPrecision64 = 1
};

typedef VstInt32 VstPlugCategory;
static constexpr VstPlugCategory kPlugCategUnknown = VST_EFFECT_CATEGORY_UNCATEGORIZED;
static constexpr VstPlugCategory kPlugCategEffect = VST_EFFECT_CATEGORY_EFFECT;
static constexpr VstPlugCategory kPlugCategSynth = VST_EFFECT_CATEGORY_INSTRUMENT;
static constexpr VstPlugCategory kPlugCategAnalysis = VST_EFFECT_CATEGORY_METERING;
static constexpr VstPlugCategory kPlugCategMastering = VST_EFFECT_CATEGORY_MASTERING;
static constexpr VstPlugCategory kPlugCategSpacializer = VST_EFFECT_CATEGORY_SPATIAL;
static constexpr VstPlugCategory kPlugCategRoomFx = VST_EFFECT_CATEGORY_DELAY_OR_ECHO;
static constexpr VstPlugCategory kPlugSurroundFx = VST_EFFECT_CATEGORY_EXTERNAL;
static constexpr VstPlugCategory kPlugCategRestoration = VST_EFFECT_CATEGORY_RESTORATION;
static constexpr VstPlugCategory kPlugCategOfflineProcess = VST_EFFECT_CATEGORY_OFFLINE;
static constexpr VstPlugCategory kPlugCategShell = VST_EFFECT_CATEGORY_CONTAINER;
static constexpr VstPlugCategory kPlugCategGenerator = VST_EFFECT_CATEGORY_WAVEGENERATOR;
static constexpr VstPlugCategory kPlugCategMaxCount = VST_EFFECT_CATEGORY_MAX;

enum VstPinPropertiesFlags
{
    kVstPinIsStereo = VST_STREAM_FLAG_STEREO,
    kVstPinUseSpeaker = VST_STREAM_FLAG_USE_TYPE
};

typedef VstInt32 VstSpeakerArrangementType;
static constexpr VstSpeakerArrangementType kSpeakerArrUserDefined = VST_SPEAKER_ARRANGEMENT_TYPE_CUSTOM;
static constexpr VstSpeakerArrangementType kSpeakerArrEmpty = VST_SPEAKER_ARRANGEMENT_TYPE_UNKNOWN;
static constexpr VstSpeakerArrangementType kSpeakerArrMono = VST_SPEAKER_ARRANGEMENT_TYPE_MONO;
static constexpr VstSpeakerArrangementType kSpeakerArrStereo = VST_SPEAKER_ARRANGEMENT_TYPE_STEREO;
static constexpr VstSpeakerArrangementType kSpeakerArrStereoSurround = 2;
static constexpr VstSpeakerArrangementType kSpeakerArrStereoCenter = 3;
static constexpr VstSpeakerArrangementType kSpeakerArrStereoSide = 4;
static constexpr VstSpeakerArrangementType kSpeakerArrStereoCLfe = 5;
static constexpr VstSpeakerArrangementType kSpeakerArr30Cine = 6;
static constexpr VstSpeakerArrangementType kSpeakerArr30Music = 7;
static constexpr VstSpeakerArrangementType kSpeakerArr31Cine = 8;
static constexpr VstSpeakerArrangementType kSpeakerArr31Music = 9;
static constexpr VstSpeakerArrangementType kSpeakerArr40Cine = 10;
static constexpr VstSpeakerArrangementType kSpeakerArr40Music = 11;
static constexpr VstSpeakerArrangementType kSpeakerArr41Cine = 12;
static constexpr VstSpeakerArrangementType kSpeakerArr41Music = 13;
static constexpr VstSpeakerArrangementType kSpeakerArr50 = VST_SPEAKER_ARRANGEMENT_TYPE_5_0;
static constexpr VstSpeakerArrangementType kSpeakerArr51 = VST_SPEAKER_ARRANGEMENT_TYPE_5_1;
static constexpr VstSpeakerArrangementType kSpeakerArr60Cine = 16;
static constexpr VstSpeakerArrangementType kSpeakerArr60Music = 17;
static constexpr VstSpeakerArrangementType kSpeakerArr61Cine = 18;
static constexpr VstSpeakerArrangementType kSpeakerArr61Music = 19;
static constexpr VstSpeakerArrangementType kSpeakerArr70Cine = 20;
static constexpr VstSpeakerArrangementType kSpeakerArr70Music = 21;
static constexpr VstSpeakerArrangementType kSpeakerArr71Cine = 22;
static constexpr VstSpeakerArrangementType kSpeakerArr71Music = VST_SPEAKER_ARRANGEMENT_TYPE_7_1;
static constexpr VstSpeakerArrangementType kSpeakerArr80Cine = 24;
static constexpr VstSpeakerArrangementType kSpeakerArr80Music = 25;
static constexpr VstSpeakerArrangementType kSpeakerArr81Cine = 26;
static constexpr VstSpeakerArrangementType kSpeakerArr81Music = 27;
static constexpr VstSpeakerArrangementType kSpeakerArr102 = 28;

typedef VstInt32 VstSpeakerType;
static constexpr VstSpeakerType kSpeakerL = VST_SPEAKER_TYPE_LEFT;
static constexpr VstSpeakerType kSpeakerR = VST_SPEAKER_TYPE_RIGHT;
static constexpr VstSpeakerType kSpeakerC = VST_SPEAKER_TYPE_CENTER;
static constexpr VstSpeakerType kSpeakerLfe = VST_SPEAKER_TYPE_LFE;
static constexpr VstSpeakerType kSpeakerLs = VST_SPEAKER_TYPE_LEFT_REAR;
static constexpr VstSpeakerType kSpeakerRs = VST_SPEAKER_TYPE_RIGHT_REAR;
static constexpr VstSpeakerType kSpeakerLc = 7;
static constexpr VstSpeakerType kSpeakerRc = 8;
static constexpr VstSpeakerType kSpeakerS = 9;
static constexpr VstSpeakerType kSpeakerSl = VST_SPEAKER_TYPE_LEFT_SIDE;
static constexpr VstSpeakerType kSpeakerSr = VST_SPEAKER_TYPE_RIGHT_SIDE;
static constexpr VstSpeakerType kSpeakerTm = 12;
static constexpr VstSpeakerType kSpeakerTfl = 13;
static constexpr VstSpeakerType kSpeakerTfc = 14;
static constexpr VstSpeakerType kSpeakerTfr = 15;
static constexpr VstSpeakerType kSpeakerTrl = 16;
static constexpr VstSpeakerType kSpeakerTrc = 17;
static constexpr VstSpeakerType kSpeakerTrr = 18;
static constexpr VstSpeakerType kSpeakerLfe2 = 19;

enum
{
    kVstMaxNameLen = VST_BUFFER_SIZE_EFFECT_NAME,
    kVstMaxLabelLen = VST_BUFFER_SIZE_PARAM_LABEL,
    kVstMaxShortLabelLen = VST_BUFFER_SIZE_STREAM_LABEL,
    kVstMaxCategLabelLen = VST_BUFFER_SIZE_CATEGORY_LABEL,
    kVstMaxFileNameLen = 100,
    kVstMaxProductStrLen = VST_BUFFER_SIZE_PRODUCT_NAME,
    kVstMaxVendorStrLen = VST_BUFFER_SIZE_VENDOR_NAME
};

struct VstEvent
{
    VstInt32 type;
    VstInt32 byteSize;
    VstInt32 deltaFrames;
    VstInt32 flags;
    char data[16];
};

struct VstMidiEvent
{
    VstInt32 type;
    VstInt32 byteSize;
    VstInt32 deltaFrames;
    VstInt32 flags;
    VstInt32 noteLength;
    VstInt32 noteOffset;
    char midiData[4];
    char detune;
    char noteOffVelocity;
    char reserved1;
    char reserved2;
};

struct VstMidiSysexEvent
{
    VstInt32 type;
    VstInt32 byteSize;
    VstInt32 deltaFrames;
    VstInt32 flags;
    VstInt32 dumpBytes;
    VstIntPtr resvd1;
    char* sysexDump;
    VstIntPtr resvd2;
};

struct VstEvents
{
    VstInt32 numEvents;
    VstIntPtr reserved;
    VstEvent* events[2];
};

struct VstTimeInfo
{
    double samplePos;
    double sampleRate;
    double nanoSeconds;
    double ppqPos;
    double tempo;
    double barStartPos;
    double cycleStartPos;
    double cycleEndPos;
    VstInt32 timeSigNumerator;
    VstInt32 timeSigDenominator;
    VstInt32 smpteOffset;
    VstSmpteFrameRate smpteFrameRate;
    VstInt32 samplesToNextClock;
    VstInt32 flags;
};

struct VstSpeakerProperties
{
    float azimuth;
    float elevation;
    float radius;
    float reserved;
    char name[kVstMaxNameLen];
    VstSpeakerType type;
    char future[28];
};

struct VstSpeakerArrangement
{
    VstInt32 type;
    VstInt32 numChannels;
    VstSpeakerProperties speakers[8];
};

struct VstPinProperties
{
    char label[kVstMaxNameLen];
    VstInt32 flags;
    VstSpeakerArrangementType arrangementType;
    char shortLabel[kVstMaxShortLabelLen];
    char future[48];
};

struct MidiKeyName
{
    VstInt32 thisProgramIndex;
    VstInt32 thisKeyNumber;
    char keyName[kVstMaxNameLen];
    VstInt32 reserved;
    char future[64];
};
