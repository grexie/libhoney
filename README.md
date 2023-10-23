The Honeycomb (Honeycomb) is a simple framework for embedding Chromium-based
browsers in other applications.

# Quick Links

- Project Page - https://bitbucket.org/chromiumembedded/honey
- Tutorial - https://bitbucket.org/chromiumembedded/honey/wiki/Tutorial
- General Usage - https://bitbucket.org/chromiumembedded/honey/wiki/GeneralUsage
- Master Build Quick-Start -
  https://bitbucket.org/chromiumembedded/honey/wiki/MasterBuildQuickStart
- Branches and Building -
  https://bitbucket.org/chromiumembedded/honey/wiki/BranchesAndBuilding
- Announcements - https://groups.google.com/forum/#!forum/honey-announce
- Support Forum - http://www.magpcss.org/honeyorum/
- Issue Tracker - https://github.com/chromiumembedded/honey/issues
- C++ API Docs -
  [Stable release docs](https://honey-builds.spotifycdn.com/docs/stable.html) /
  [Beta release docs](https://honey-builds.spotifycdn.com/docs/beta.html)
- Downloads - https://honey-builds.spotifycdn.com/index.html
- Donations - http://www.magpcss.org/honeyorum/donate.php

# Introduction

Honeycomb is a BSD-licensed open source project founded by Marshall Greenblatt
in 2008 and based on the [Google Chromium](http://www.chromium.org/Home)
project. Unlike the Chromium project itself, which focuses mainly on Google
Chrome application development, Honeycomb focuses on facilitating embedded
browser use cases in third-party applications. Honeycomb insulates the user from
the underlying Chromium and Blink code complexity by offering production-quality
stable APIs, release branches tracking specific Chromium releases, and binary
distributions. Most features in Honeycomb have default implementations that
provide rich functionality while requiring little or no integration work from
the user. There are currently over 100 million installed instances of Honeycomb
around the world embedded in products from a wide range of companies and
industries. A partial list of companies and products using Honeycomb is
available on the
[Honeycomb Wikipedia page](http://en.wikipedia.org/wiki/Chromium_Embedded_Framework#Applications_using_Honeycomb).
Some use cases for Honeycomb include:

- Embedding an HTML5-compliant Web browser control in an existing native
  application.
- Creating a light-weight native “shell” application that hosts a user interface
  developed primarily using Web technologies.
- Rendering Web content “off-screen” in applications that have their own custom
  drawing frameworks.
- Acting as a host for automated testing of existing Web properties and
  applications.

Honeycomb supports a wide range of programming languages and operating systems
and can be easily integrated into both new and existing applications. It was
designed from the ground up with both performance and ease of use in mind. The
base framework includes C and C++ programming interfaces exposed via native
libraries that insulate the host application from Chromium and Blink
implementation details. It provides close integration between the browser and
the host application including support for custom plugins, protocols, JavaScript
objects and JavaScript extensions. The host application can optionally control
resource loading, navigation, context menus, printing and more, while taking
advantage of the same performance and HTML5 technologies available in the Google
Chrome Web browser.

Numerous individuals and organizations contribute time and resources to support
Honeycomb development, but more involvement from the community is always
welcome. This includes support for both the core Honeycomb project and external
projects that integrate Honeycomb with additional programming languages and
frameworks (see the "External Projects" section below). If you are interested in
donating time to help with Honeycomb development please see the "Helping Out"
section below. If you are interested in donating money to support general
Honeycomb development and infrastructure efforts please visit the
[Honeycomb Donations](http://www.magpcss.org/honeyorum/donate.php) page.

# Getting Started

Users new to Honeycomb development should start by reading the
[Tutorial](https://bitbucket.org/chromiumembedded/honey/wiki/Tutorial) Wiki page
for an overview of Honeycomb usage and then proceed to the
[GeneralUsage](https://bitbucket.org/chromiumembedded/honey/wiki/GeneralUsage)
Wiki page for a more in-depth discussion or architectural and usage issues.
Complete API documentation is available
[here](https://honey-builds.spotifycdn.com/docs/stable.html). Honeycomb support
and related discussion is available on the
[Honeycomb Forum](http://www.magpcss.org/honeyorum/).

# Binary Distributions

Binary distributions, which include all files necessary to build a
Honeycomb-based application, are available on the
[Downloads](https://honey-builds.spotifycdn.com/index.html) page. Binary
distributions are stand-alone and do not require the download of Honeycomb or
Chromium source code. Symbol files for debugging binary distributions of
libhoneycomb can also be downloaded from the above links.

# Source Distributions

The Honeycomb project is an extension of the Chromium project. Honeycomb
maintains development and release branches that track Chromium branches.
Honeycomb source code can be downloaded, built and packaged manually or with
automated tools. Visit the
[BranchesAndBuilding](https://bitbucket.org/chromiumembedded/honey/wiki/BranchesAndBuilding)
Wiki page for more information.

# External Projects

The base Honeycomb framework includes support for the C and C++ programming
languages. Thanks to the hard work of external maintainers Honeycomb can
integrate with a number of other programming languages and frameworks. These
external projects are not maintained by Honeycomb so please contact the
respective project maintainer if you have any questions or issues.

- .Net (Honeycomb3) - https://github.com/honeysharp/HoneycombSharp
- .Net (Honeycomb1) - https://bitbucket.org/fddima/honeyglue
- .Net/Mono (Honeycomb3) -
  https://gitlab.com/xiliumhq/chromiumembedded/honeyglue
- Delphi - https://github.com/hgourvest/dhoney3
- Delphi - https://github.com/salvadordf/Honeycomb4Delphi
- Go - https://github.com/CzarekTomczak/honey2go
- Go - https://github.com/energye/energy
- Java - https://bitbucket.org/chromiumembedded/java-honey
- Python - http://code.google.com/p/honeypython/

If you're the maintainer of a project not listed above and would like your
project listed here please either post to the
[Honeycomb Forum](http://www.magpcss.org/honeyorum/) or contact Marshall
directly.

# Helping Out

Honeycomb is still very much a work in progress. Some ways that you can help
out:

\- Vote for issues in the
[Honeycomb issue tracker](https://github.com/chromiumembedded/honey/issues) that
are important to you. This helps with development prioritization.

\- Report any bugs that you find or feature requests that are important to you.
Make sure to first search for existing issues before creating new ones. Please
use the [Honeycomb Forum](http://magpcss.org/honeyorum) and not the issue
tracker for usage questions. Each Honeycomb issue should:

- Include the Honeycomb revision or binary distribution version.
- Include information about your OS and compiler version.
- If the issue is a bug please provide detailed reproduction information.
- If the issue is a feature please describe why the feature is beneficial.

\- Write unit tests for new or existing functionality.

\- Pull requests and patches are welcome. View open issues in the
[Honeycomb issue tracker](https://github.com/chromiumembedded/honey/issues) or
search for TODO(honey) in the source code for ideas.

If you would like to contribute source code changes to Honeycomb please follow
the below guidelines:

\- Create or find an appropriate issue for each distinct bug, feature or change.

\- Submit a
[pull request](https://bitbucket.org/chromiumembedded/honey/wiki/ContributingWithGit)
or create a patch with your changes and attach it to the Honeycomb issue.
Changes should:

- Be submitted against the current
  [Honeycomb master branch](https://bitbucket.org/chromiumembedded/honey/src/?at=master)
  unless explicitly fixing a bug in a Honeycomb release branch.
- Follow the style of existing Honeycomb source files. In general Honeycomb uses
  the
  [Chromium C++ style guide](https://chromium.googlesource.com/chromium/src/+/master/styleguide/c++/c++.md).
- Include new or modified unit tests as appropriate to the functionality.
- Not include unnecessary or unrelated changes.
