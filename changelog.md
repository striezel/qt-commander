# Version history of Qt Commander

_(Note: This changelog focuses on the major changes between the different
versions. Therefore, it may not contain all changes. Especially smaller fixes or
improvements may be omitted.)_

## Next Version (2026-04-??)

* __[new feature]__
  A PDF viewer is added to view PDF files.

* __[new feature]__
  The program can now calculate common checksums (MD5, SHA-1, SHA-2, SHA-3) of
  files and create the corresponding checksum files.

* __[fix]__
  Adjust width of labels in file information window so that text does not get
  cut off.

* __[fix]__
  Fix a bug where user could get "stuck" in directory with insufficient file
  permissions to view the directory.

## Version 0.1.3 (2026-04-19)

* __[new feature]__
  A video player is added to view video files. Supported video formats may vary
  depending on the system's multimedia capabilities.

## Version 0.1.2 (2026-04-06)

* __[new feature]__
  Size values are now shown in a formatted, human-readable format by default
  instead of just showing the numeric value. For example, the size of a file of
  1634 bytes will now show up as "1.60 KiB" (or a similar localized string)
  instead of as "1634".

  This feature can be enabled and disabled via the view menu, so users can
  switch back to the plain numeric size, if desired.

* __[new feature]__
  A new setting to loop audio files endlessly has been added to the audio
  player. It is disabled by default.

* __[improvement]__
  When navigating to a parent directory, the previous directory is now
  auto-selected in the directory list. This makes navigation a bit easier /
  smoother.

* __[improvement]__
  Releases will now provide packages for Ubuntu 26.04, too.

## Version 0.1.1 (2026-03-28)

First release of Qt Commander.

## Version 0.1.0 (2026-03-13)

This was an internal version with basic functionality. It was never released.
