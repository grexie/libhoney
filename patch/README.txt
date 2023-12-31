There may be instances where Honeycomb requires changes to the source code for
Chromium, Blink or third-party projects that are either not desired by those
projects or that have not yet been merged into the source code versions of those
projects used by Honeycomb. To address this situation the Honeycomb project adds a patch
capability as part of honey_create_projects[.bat|sh] build step. This patch
capability works as follows:

1. The Honeycomb developer creates one or more patch files containing all required
   code changes and places those patch files in the "patches" subdirectory.
2. The Honeycomb developer adds an entry for each patch file in the "patch.cfg" file.
3. When building Honeycomb from source code the patch files are applied by the
   patcher.py tool via the honey_create_projects[.bat|sh] build step
4. When updating Chromium the patch_updater.py tool is used to update all patch
   files. See https://bitbucket.org/chromiumembedded/honey/wiki/ChromiumUpdate.md
   for more information about the update process.
