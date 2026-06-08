# Version history of Qt Commander

_(Note: This changelog focuses on the major changes between the different
versions. Therefore, it may not contain all changes. Especially smaller fixes or
improvements may be omitted.)_

## Next Version (2026-06-??)

* __[new feature]__
  The initial directories which are shown can now be set via the command-line.
  For example, `qt-commander /tmp` will show the directory `/tmp` on the left
  side (and the home directory on the right side). When two directory paths
  are given, the first one is opened on the left side and the second one on the
  right side.

* __[new feature]__
  The text viewer can now provide basic syntax highlighting for the Java
  programming language.

* __[improvement]__
  When the view button is clicked while a directory is selected, it will now trigger a change to
  that directory instead of showing a warning that only files can be viewed.

## Version 0.1.9 (2026-05-31)

* __[new feature]__
  A new configuration option allows to change the delete operation for files / directories from
  actual deletion to move to trash.

* __[improvement]__
  All settings can now be edited in a new dialog. This dialog also includes the
  settings used by the various file viewers.

## Version 0.1.8 (2026-05-21)

* __[new feature]__
  The text viewer can now provide basic syntax highlighting for a few common
  programming languages.

## Version 0.1.7 (2026-05-15)

* __[improvement]__
  Improve handling of file names in directory comparison for file systems with
  case-sensitive file names on non-Windows systems.

* __[new feature]__
  The directory comparison can now be used to copy missing / different files
  from one side / directory to the other side / directory. It also allows the
  deletion of files.

## Version 0.1.6 (2026-05-10)

* __[new feature]__
  The program can now compare the contents of the two directories.

  However, there are some limitations: Subdirectories are not compared (yet).
  The comparison only checks their existence, not their contents. Furthermore,
  a comparison can take relatively long (several seconds) when the two
  directories contain many similar or big files.

* __[new feature]__
  The checksum calculation can now also calculate BLAKE2 checksums (BLAKE2b-160,
  BLAKE2b-256, BLAKE2b-384, BLAKE2b-512, BLAKE2s-128, BLAKE2s-160, BLAKE2s-224,
  and BLAKE2s-256).

* __[improvement]__
  The checksum dialog will now remember the last used hash algorithm for the
  current session and preselect that whenever the checksum dialog is opened.

## Version 0.1.5 (2026-05-04)

* __[fix]__
  Avoid division by zero when showing file information of a file on a virtual
  file system (like `/proc` on Linux).

## Version 0.1.4 (2026-05-03)

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
