# Xaymar VST2 JUCE Shim

Internal compatibility shim for compiling JUCE VST2 hosting against the
clean-room `Xaymar/vst2sdk` headers instead of the discontinued Steinberg VST2
SDK headers.

This shim is experimental. It maps the JUCE-expected `aeffect.h` and
`aeffectx.h` surface to clean-room VST2 interface definitions and should be
validated with real VST2 plugins before shipping.

Upstream SDK:
https://github.com/Xaymar/vst2sdk

License basis:
`Xaymar/vst2sdk` is BSD-3-Clause licensed. Keep the upstream license notice
with distributed source/binary materials when this provider is used.
