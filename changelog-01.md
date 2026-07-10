# Light Host Modern 1.1.0

- Added device persistence with disabled, last selected device, and custom device modes.
- Added configurable retry interval and max retry attempts for unavailable audio devices.
- Added enabled-device filtering so specific backends, inputs, outputs, or ASIO devices can be excluded from automatic selection.
- Added check/uncheck all controls for input and output channel selection.
- Improved ASIO behavior by avoiding forced fallback to arbitrary devices when a preferred device becomes unavailable.
- Improved ASIO diagnostics and device switching logs.
- Fixed the WinUI window material preference resetting to Mica after reopening the interface.
- Fixed MSI installer directory handling that could trigger Windows Installer error 2727.
- Improved the Enabled devices modal by separating input and output devices into independent cards.
- Improved checkbox alignment in device lists.
- Updated application version metadata and release packaging to 1.1.0.
