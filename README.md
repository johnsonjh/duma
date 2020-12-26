# **D.U.M.A.**

-----------------------------

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-GPL)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-LGPL)
[![FOSSAStatus](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_shield)
[![LocCount](https://img.shields.io/tokei/lines/github/johnsonjh/duma.svg)](https://github.com/XAMPPRocky/tokei)
[![GitHubCodeSize](https://img.shields.io/github/languages/code-size/johnsonjh/OldCurve25519ScalarMult.svg)](https://github.com/johnsonjh/OldCurve25519ScalarMult)
[![GitHubRelease](https://img.shields.io/github/release/johnsonjh/duma.svg)](https://github.com/johnsonjh/duma/releases/)
[![GitHubTag](https://img.shields.io/github/tag/johnsonjh/duma.svg)](https://github.com/johnsonjh/duma/tags/)
[![LgtmAlerts](https://img.shields.io/lgtm/alerts/g/johnsonjh/duma.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/johnsonjh/duma/alerts/)
[![CodacyBadge](https://api.codacy.com/project/badge/Grade/f777934d666b4a6a9672d89b404c4953)](https://app.codacy.com/gh/johnsonjh/duma?utm_source=github.com&utm_medium=referral&utm_content=johnsonjh/duma&utm_campaign=Badge_Grade)
[![CodeBeat](https://codebeat.co/badges/a0be6809-acda-41a7-96a8-0d46698dc42c)](https://codebeat.co/projects/github-com-johnsonjh-duma-master)
[![DeepScanGrade](https://deepscan.io/api/teams/12184/projects/15170/branches/299583/badge/grade.svg)](https://deepscan.io/dashboard#view=project&tid=12184&pid=15170&bid=299583)
[![TickgitTODOs](https://img.shields.io/endpoint?url=https://api.tickgit.com/badge?repo=github.com/johnsonjh/duma)](https://www.tickgit.com/browse?repo=github.com/johnsonjh/duma)

-----------------------------

## **Detect Unintended Memory Access**

-----------------------------

### ***A Red-Zone memory allocator***

-----------------------------

#### Introduction

After deciding it was important to revive D.U.M.A., it quickly became
clear continued use of the `cvs` version control system would be a limiting
factor to the success of a new D.U.M.A. distribution. If the project was
going to survive in the long-term, it would be necessary to perform a full
conversion to a more modern, commonly used, and accessible VCS system.

Unaware of the efforts of the [original author](https://github.com/hayguen/duma),
I created this repository independently, and expended much effort to identify all
contributors and authors of the many patches and contributions that had been
produced by the community over many years. After many false starts, I'm now mostly
satisfied with the end result - especially considering that many of my conclusions
turned out to be very close (and sometimes identical) to those made in the original
author's own conversion efforts. It should be noted that I had no special knowledge
of this software, besides being a user, and no relationship or even any previous
contact with the author.

The most time consuming part of the conversion process was determining all the
non-canonical changes, figuring out who actually wrote them, and finally deciding
if these patches should be incorporated into a new official "mainline" repository.

This process required more than a few "educated guesses", and, when the necessary
details were not available, simple judgment calls. With such an old, widely
distributed, and hacked-upon code-base, which survived as long as it did, without
an official maintainer or source for new releases for many years, much of data
available is fragmentary and conflicting. Sometimes, the only sources of information
are the often fading memories of those originally involved with project. Often,
inferences had to be made, usually based on incomplete and inaccurate information.

Because of these uncertainties, I am certain that mistakes have been made and new
errors introduced. I welcome any contributions from the public that would help to
make the repository more historically accurate as well as improve functionality.

-----------------------------

#### The Future of D.U.M.A.

A large task, not yet complete, is reconciling this reconstruction against the
last version which was released by the [original author](https://github.com/hayguen/duma).
The intention is to focus on correctly identifying any significant deviations,
especially in regardings to the identification of contributors, and ensuring that
any disputes that might arise are fairly handled. As new information becomes available,
the commit logs for the project will be updated to provide an accurate record for posterity.

The structure of this conversion very closely resembles that of the original `cvs`
repository. In contrast, the orignal author's conversion efforts included a radical
refactoring the source tree and directory structures. I believe that this was a prudent
decision, and eventually plan to incorporate similar restructuring in this repository.

While every attempt was been made to ensure correctness, especially when acknowledging
contributors, extracting information where available from the original SourceForge site,
random web forums, other `cvs`, `svn`, and, `git` repositories, and mailing lists archives,
this release does incorporate changes that were never part of the original work, mostly
to ensure compatability with modern systems operating systems and environments.

-----------------------------

#### Official D.U.M.A.?

This repository should be considered the "official" source for D.U.M.A. releases.

Hayati Ayguen, the original author, is no longer actively maintaining D.U.M.A. and
has moved on to other projects. While he was happy to correspond about the project, it
seems unlikely that he will take on the role of primary maintainer of the package.

-----------------------------

#### History

D.U.M.A. started out as a fork of Bruce Perens' venerable "Electric Fence",
a tool created at Pixar, back in 1987. 

D.U.M.A. was ported to Linux as early as 1992.

-----------------------------

#### Conclusion

Now, almost 30 years later, we thank to Hayati Ayguen and all other
contributors, who put time and effort into this project, but allow them move
on to other endeavours. They can do so knowing that D.U.M.A. is not gone or forgotten. Even though it is a mature software and package, it will be properly maintained and updated as needed.

-----------------------------

#### Addendum

For some additional information on the history of D.U.M.A. and Electric Fence,
see <https://perens.com/2017/04/04/my-30th-anniversary-in-open-source/>, for short
blog entry by Bruce Perens' mentioning the creation of his Electric Fence tool.

The orginal author, [Hayati Ayguen](https://github.com/hayguen), remains an active developer.

-----------------------------

#### Availability

##### Source Code (Releases)

* [Gridfinity GitLab Releases](https://gitlab.gridfinity.com/jeff/duma/-/releases/)
* [SourceHut Releases](https://sr.ht/~trn/duma/)  *(available soon)*
* [GitHub Releases](https://github.com/johnsonjh/duma/releases/)

##### Source Code (Tags)

* [Gridfinity GitLab Tags](https://gitlab.gridfinity.com/jeff/duma/-/tags/)
* [SourceHut Git Refs](https://git.sr.ht/~trn/duma/refs)
* [GitHub Tags](https://github.com/johnsonjh/duma/tags/)

##### Source Code (Development Repositories)

* [Gridfinity GitLab](https://gitlab.gridfinity.com/jeff/duma)
* [SourceHut Git](https://git.sr.ht/~trn/duma)
* [GitHub](https://github.com/johnsonjh/duma)

-----------------------------

#### Issue Tracking

* [GitLab Issue Tracker](https://github.com/johnsonjh/duma/issues)
* [Gridfinity GitLab Issues](https://gitlab.gridfinity.com/jeff/duma/-/issues)

-----------------------------

#### Security Policy

* [Security Policy and Vulnerability Reporting](https://github.com/johnsonjh/duma/SECURITY.md)

-----------------------------

#### License

* [GNU General Public License v2.0 (GPLv2)](https://tldrlegal.com/license/gnu-general-public-license-v2)
* [GNU Lesser General Public License v2.1 (LGPLv2.1)](https://tldrlegal.com/license/gnu-lesser-general-public-license-v2.1-(lgpl-2.1))
* [![FOSSASmallStatus](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=small)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_small)

-----------------------------

