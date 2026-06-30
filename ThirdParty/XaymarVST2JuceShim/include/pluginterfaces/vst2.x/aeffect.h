/*
  Experimental JUCE compatibility surface for Xaymar/vst2sdk.

  This file intentionally does not include or copy Steinberg's discontinued
  VST2 SDK headers. It provides the legacy symbol names expected by JUCE while
  using the clean-room Xaymar VST2 SDK as the provider selected by CMake.
*/

#pragma once

#include <cstdint>

#ifndef VSTCALLBACK
 #if defined(_WIN32)
  #define VSTCALLBACK __cdecl
 #else
  #define VSTCALLBACK
 #endif
#endif

typedef int16_t VstInt16;
typedef int32_t VstInt32;
typedef int64_t VstInt64;
typedef intptr_t VstIntPtr;

struct AEffect;

enum
{
    VST_MAGICNUMBER = ('V' << 24) | ('s' << 16) | ('t' << 8) | 'P',

    VST_EFFECT_OPCODE_CREATE = 0x00,
    VST_EFFECT_OPCODE_DESTROY = 0x01,
    VST_EFFECT_OPCODE_PROGRAM_SET = 0x02,
    VST_EFFECT_OPCODE_PROGRAM_GET = 0x03,
    VST_EFFECT_OPCODE_PROGRAM_SET_NAME = 0x04,
    VST_EFFECT_OPCODE_PROGRAM_GET_NAME = 0x05,
    VST_EFFECT_OPCODE_PARAM_LABEL = 0x06,
    VST_EFFECT_OPCODE_PARAM_VALUE_TO_STRING = 0x07,
    VST_EFFECT_OPCODE_PARAM_NAME = 0x08,
    VST_EFFECT_OPCODE_09 = 0x09,
    VST_EFFECT_OPCODE_SET_SAMPLE_RATE = 0x0A,
    VST_EFFECT_OPCODE_SET_BLOCK_SIZE = 0x0B,
    VST_EFFECT_OPCODE_SUSPEND_RESUME = 0x0C,
    VST_EFFECT_OPCODE_EDITOR_GET_RECT = 0x0D,
    VST_EFFECT_OPCODE_EDITOR_OPEN = 0x0E,
    VST_EFFECT_OPCODE_EDITOR_CLOSE = 0x0F,
    VST_EFFECT_OPCODE_EDITOR_DRAW = 0x10,
    VST_EFFECT_OPCODE_EDITOR_MOUSE = 0x11,
    VST_EFFECT_OPCODE_EDITOR_KEYBOARD = 0x12,
    VST_EFFECT_OPCODE_EDITOR_KEEP_ALIVE = 0x13,
    VST_EFFECT_OPCODE_FOURCC = 0x16,
    VST_EFFECT_OPCODE_GET_CHUNK_DATA = 0x17,
    VST_EFFECT_OPCODE_SET_CHUNK_DATA = 0x18,
    VST_EFFECT_OPCODE_EVENT = 0x19,
    VST_EFFECT_OPCODE_PARAM_IS_AUTOMATABLE = 0x1A,
    VST_EFFECT_OPCODE_PARAM_VALUE_FROM_STRING = 0x1B,
    VST_EFFECT_OPCODE_1C = 0x1C,
    VST_EFFECT_OPCODE_1D = 0x1D,
    VST_EFFECT_OPCODE_1E = 0x1E,
    VST_EFFECT_OPCODE_1F = 0x1F,
    VST_EFFECT_OPCODE_20 = 0x20,
    VST_EFFECT_OPCODE_INPUT_GET_PROPERTIES = 0x21,
    VST_EFFECT_OPCODE_OUTPUT_GET_PROPERTIES = 0x22,
    VST_EFFECT_OPCODE_CATEGORY = 0x23,
    VST_EFFECT_OPCODE_24 = 0x24,
    VST_EFFECT_OPCODE_25 = 0x25,
    VST_EFFECT_OPCODE_26 = 0x26,
    VST_EFFECT_OPCODE_27 = 0x27,
    VST_EFFECT_OPCODE_28 = 0x28,
    VST_EFFECT_OPCODE_29 = 0x29,
    VST_EFFECT_OPCODE_SET_SPEAKER_ARRANGEMENT = 0x2A,
    VST_EFFECT_OPCODE_2B = 0x2B,
    VST_EFFECT_OPCODE_BYPASS = 0x2C,
    VST_EFFECT_OPCODE_EFFECT_NAME = 0x2D,
    VST_EFFECT_OPCODE_TRANSLATE_ERROR = 0x2E,
    VST_EFFECT_OPCODE_VENDOR_NAME = 0x2F,
    VST_EFFECT_OPCODE_PRODUCT_NAME = 0x30,
    VST_EFFECT_OPCODE_VENDOR_VERSION = 0x31,
    VST_EFFECT_OPCODE_CUSTOM = 0x32,
    VST_EFFECT_OPCODE_SUPPORTS = 0x33,
    VST_EFFECT_OPCODE_TAIL_SAMPLES = 0x34,
    VST_EFFECT_OPCODE_IDLE = 0x35,
    VST_EFFECT_OPCODE_36 = 0x36,
    VST_EFFECT_OPCODE_37 = 0x37,
    VST_EFFECT_OPCODE_PARAM_PROPERTIES = 0x38,
    VST_EFFECT_OPCODE_39 = 0x39,
    VST_EFFECT_OPCODE_VST_VERSION = 0x3A,
    VST_EFFECT_OPCODE_EDITOR_VKEY_DOWN = 0x3B,
    VST_EFFECT_OPCODE_EDITOR_VKEY_UP = 0x3C,
    VST_EFFECT_OPCODE_3D = 0x3D,
    VST_EFFECT_OPCODE_3E = 0x3E,
    VST_EFFECT_OPCODE_3F = 0x3F,
    VST_EFFECT_OPCODE_40 = 0x40,
    VST_EFFECT_OPCODE_41 = 0x41,
    VST_EFFECT_OPCODE_42 = 0x42,
    VST_EFFECT_OPCODE_PROGRAM_SET_BEGIN = 0x43,
    VST_EFFECT_OPCODE_PROGRAM_SET_END = 0x44,
    VST_EFFECT_OPCODE_GET_SPEAKER_ARRANGEMENT = 0x45,
    VST_EFFECT_OPCODE_CONTAINER_NEXT_EFFECT_ID = 0x46,
    VST_EFFECT_OPCODE_PROCESS_BEGIN = 0x47,
    VST_EFFECT_OPCODE_PROCESS_END = 0x48,
    VST_EFFECT_OPCODE_49 = 0x49,
    VST_EFFECT_OPCODE_4A = 0x4A,
    VST_EFFECT_OPCODE_BANK_LOAD = 0x4B,
    VST_EFFECT_OPCODE_PROGRAM_LOAD = 0x4C,
    VST_EFFECT_OPCODE_4D = 0x4D,

    VST_EFFECT_FLAG_EDITOR = 1 << 0,
    VST_EFFECT_FLAG_SUPPORTS_FLOAT = 1 << 4,
    VST_EFFECT_FLAG_CHUNKS = 1 << 5,
    VST_EFFECT_FLAG_INSTRUMENT = 1 << 8,
    VST_EFFECT_FLAG_SILENT_TAIL = 1 << 9,
    VST_EFFECT_FLAG_SUPPORTS_DOUBLE = 1 << 12
};

typedef VstIntPtr (VSTCALLBACK *audioMasterCallback) (AEffect*, VstInt32, VstInt32, VstIntPtr, void*, float);
typedef VstIntPtr (VSTCALLBACK *AEffectDispatcherProc) (AEffect*, VstInt32, VstInt32, VstIntPtr, void*, float);
typedef void (VSTCALLBACK *AEffectProcessProc) (AEffect*, float**, float**, VstInt32);
typedef void (VSTCALLBACK *AEffectProcessDoubleProc) (AEffect*, double**, double**, VstInt32);
typedef void (VSTCALLBACK *AEffectSetParameterProc) (AEffect*, VstInt32, float);
typedef float (VSTCALLBACK *AEffectGetParameterProc) (AEffect*, VstInt32);

enum
{
    kEffectMagic = VST_MAGICNUMBER
};

struct ERect
{
    VstInt16 top;
    VstInt16 left;
    VstInt16 bottom;
    VstInt16 right;
};

enum AEffectOpcodes
{
    effOpen = VST_EFFECT_OPCODE_CREATE,
    effClose = VST_EFFECT_OPCODE_DESTROY,
    effSetProgram = VST_EFFECT_OPCODE_PROGRAM_SET,
    effGetProgram = VST_EFFECT_OPCODE_PROGRAM_GET,
    effSetProgramName = VST_EFFECT_OPCODE_PROGRAM_SET_NAME,
    effGetProgramName = VST_EFFECT_OPCODE_PROGRAM_GET_NAME,
    effGetParamLabel = VST_EFFECT_OPCODE_PARAM_LABEL,
    effGetParamDisplay = VST_EFFECT_OPCODE_PARAM_VALUE_TO_STRING,
    effGetParamName = VST_EFFECT_OPCODE_PARAM_NAME,
    effGetVu = VST_EFFECT_OPCODE_09,
    effSetSampleRate = VST_EFFECT_OPCODE_SET_SAMPLE_RATE,
    effSetBlockSize = VST_EFFECT_OPCODE_SET_BLOCK_SIZE,
    effMainsChanged = VST_EFFECT_OPCODE_SUSPEND_RESUME,
    effEditGetRect = VST_EFFECT_OPCODE_EDITOR_GET_RECT,
    effEditOpen = VST_EFFECT_OPCODE_EDITOR_OPEN,
    effEditClose = VST_EFFECT_OPCODE_EDITOR_CLOSE,
    effEditDraw = VST_EFFECT_OPCODE_EDITOR_DRAW,
    effEditMouse = VST_EFFECT_OPCODE_EDITOR_MOUSE,
    effEditKey = VST_EFFECT_OPCODE_EDITOR_KEYBOARD,
    effEditIdle = VST_EFFECT_OPCODE_EDITOR_KEEP_ALIVE,
    effIdentify = VST_EFFECT_OPCODE_FOURCC,
    effGetChunk = VST_EFFECT_OPCODE_GET_CHUNK_DATA,
    effSetChunk = VST_EFFECT_OPCODE_SET_CHUNK_DATA,
    effProcessEvents = VST_EFFECT_OPCODE_EVENT,
    effCanBeAutomated = VST_EFFECT_OPCODE_PARAM_IS_AUTOMATABLE,
    effString2Parameter = VST_EFFECT_OPCODE_PARAM_VALUE_FROM_STRING,
    effGetNumProgramCategories = VST_EFFECT_OPCODE_1C,
    effGetProgramNameIndexed = VST_EFFECT_OPCODE_1D,
    effCopyProgram = VST_EFFECT_OPCODE_1E,
    effConnectInput = VST_EFFECT_OPCODE_1F,
    effConnectOutput = VST_EFFECT_OPCODE_20,
    effGetInputProperties = VST_EFFECT_OPCODE_INPUT_GET_PROPERTIES,
    effGetOutputProperties = VST_EFFECT_OPCODE_OUTPUT_GET_PROPERTIES,
    effGetPlugCategory = VST_EFFECT_OPCODE_CATEGORY,
    effGetCurrentPosition = VST_EFFECT_OPCODE_24,
    effGetDestinationBuffer = VST_EFFECT_OPCODE_25,
    effOfflineNotify = VST_EFFECT_OPCODE_26,
    effOfflinePrepare = VST_EFFECT_OPCODE_27,
    effOfflineRun = VST_EFFECT_OPCODE_28,
    effProcessVarIo = VST_EFFECT_OPCODE_29,
    effSetSpeakerArrangement = VST_EFFECT_OPCODE_SET_SPEAKER_ARRANGEMENT,
    effSetBlockSizeAndSampleRate = VST_EFFECT_OPCODE_2B,
    effSetBypass = VST_EFFECT_OPCODE_BYPASS,
    effGetEffectName = VST_EFFECT_OPCODE_EFFECT_NAME,
    effGetErrorText = VST_EFFECT_OPCODE_TRANSLATE_ERROR,
    effGetVendorString = VST_EFFECT_OPCODE_VENDOR_NAME,
    effGetProductString = VST_EFFECT_OPCODE_PRODUCT_NAME,
    effGetVendorVersion = VST_EFFECT_OPCODE_VENDOR_VERSION,
    effVendorSpecific = VST_EFFECT_OPCODE_CUSTOM,
    effCanDo = VST_EFFECT_OPCODE_SUPPORTS,
    effGetTailSize = VST_EFFECT_OPCODE_TAIL_SAMPLES,
    effIdle = VST_EFFECT_OPCODE_IDLE,
    effGetIcon = VST_EFFECT_OPCODE_36,
    effSetViewPosition = VST_EFFECT_OPCODE_37,
    effGetParameterProperties = VST_EFFECT_OPCODE_PARAM_PROPERTIES,
    effKeysRequired = VST_EFFECT_OPCODE_39,
    effGetVstVersion = VST_EFFECT_OPCODE_VST_VERSION,
    effEditKeyDown = VST_EFFECT_OPCODE_EDITOR_VKEY_DOWN,
    effEditKeyUp = VST_EFFECT_OPCODE_EDITOR_VKEY_UP,
    effSetEditKnobMode = VST_EFFECT_OPCODE_3D,
    effGetMidiProgramName = VST_EFFECT_OPCODE_3E,
    effGetCurrentMidiProgram = VST_EFFECT_OPCODE_3F,
    effGetMidiProgramCategory = VST_EFFECT_OPCODE_40,
    effHasMidiProgramsChanged = VST_EFFECT_OPCODE_41,
    effGetMidiKeyName = VST_EFFECT_OPCODE_42,
    effBeginSetProgram = VST_EFFECT_OPCODE_PROGRAM_SET_BEGIN,
    effEndSetProgram = VST_EFFECT_OPCODE_PROGRAM_SET_END,
    effGetSpeakerArrangement = VST_EFFECT_OPCODE_GET_SPEAKER_ARRANGEMENT,
    effShellGetNextPlugin = VST_EFFECT_OPCODE_CONTAINER_NEXT_EFFECT_ID,
    effStartProcess = VST_EFFECT_OPCODE_PROCESS_BEGIN,
    effStopProcess = VST_EFFECT_OPCODE_PROCESS_END,
    effSetTotalSampleToProcess = VST_EFFECT_OPCODE_49,
    effSetPanLaw = VST_EFFECT_OPCODE_4A,
    effBeginLoadBank = VST_EFFECT_OPCODE_BANK_LOAD,
    effBeginLoadProgram = VST_EFFECT_OPCODE_PROGRAM_LOAD,
    effSetProcessPrecision = VST_EFFECT_OPCODE_4D
};

enum AEffectFlags
{
    effFlagsHasEditor = VST_EFFECT_FLAG_EDITOR,
    effFlagsCanReplacing = VST_EFFECT_FLAG_SUPPORTS_FLOAT,
    effFlagsProgramChunks = VST_EFFECT_FLAG_CHUNKS,
    effFlagsIsSynth = VST_EFFECT_FLAG_INSTRUMENT,
    effFlagsNoSoundInStop = VST_EFFECT_FLAG_SILENT_TAIL,
    effFlagsCanDoubleReplacing = VST_EFFECT_FLAG_SUPPORTS_DOUBLE
};

struct AEffect
{
    VstInt32 magic;
    AEffectDispatcherProc dispatcher;
    AEffectProcessProc process;
    AEffectSetParameterProc setParameter;
    AEffectGetParameterProc getParameter;
    VstInt32 numPrograms;
    VstInt32 numParams;
    VstInt32 numInputs;
    VstInt32 numOutputs;
    VstInt32 flags;
    VstIntPtr resvd1;
    VstIntPtr resvd2;
    VstInt32 initialDelay;
    VstInt32 realQualities;
    VstInt32 offQualities;
    float ioRatio;
    void* object;
    void* user;
    VstInt32 uniqueID;
    VstInt32 version;
    AEffectProcessProc processReplacing;
    AEffectProcessDoubleProc processDoubleReplacing;
    char future[56];
};
